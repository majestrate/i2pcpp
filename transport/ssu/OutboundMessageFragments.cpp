#include "OutboundMessageFragments.h"

#include "../../util/make_unique.h"

#include "../UDPTransport.h"

namespace i2pcpp {
	namespace SSU {
		OutboundMessageFragments::OutboundMessageFragments(UDPTransport &transport) :
			m_transport(transport) {}

		void OutboundMessageFragments::sendData(PeerState const &ps, uint32_t const msgId, ByteArray const &data)
		{
			OutboundMessageState oms(msgId, data);

			std::lock_guard<std::mutex> lock(m_mutex);
			addState(ps, msgId, oms);
			m_transport.m_ios.post(boost::bind(&OutboundMessageFragments::sendDataCallback, this, ps, msgId));
		}

		void OutboundMessageFragments::addState(PeerState const &ps, uint32_t const msgId, OutboundMessageState &oms)
		{
			m_states.emplace(std::make_pair(msgId, std::move(oms)));

			m_timers[msgId] = std::make_unique<boost::asio::deadline_timer>(m_transport.m_ios, boost::posix_time::time_duration(0, 0, 2));
			m_timers[msgId]->async_wait(boost::bind(&OutboundMessageFragments::timerCallback, this, boost::asio::placeholders::error, ps, msgId));
		}

		void OutboundMessageFragments::delState(const uint32_t msgId)
		{
			auto itr = m_timers.find(msgId);
			if(itr != m_timers.end()) {
				m_timers[msgId]->cancel();
				m_timers.erase(msgId);
			}

			m_states.erase(msgId);
		}

		void OutboundMessageFragments::sendDataCallback(PeerState ps, uint32_t const msgId)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			auto itr = m_states.find(msgId);
			if(itr != m_states.end()) {
				OutboundMessageState& oms = itr->second;

				std::vector<PacketBuilder::FragmentPtr> fragList;
				fragList.push_back(oms.getNextFragment());

				oms.markFragmentSent(fragList[0]->fragNum);

				PacketPtr p = PacketBuilder::buildData(ps.getEndpoint(), false, CompleteAckList(), PartialAckList(), fragList);
				p->encrypt(ps.getCurrentSessionKey(), ps.getCurrentMacKey());
				m_transport.sendPacket(p);

				if(!oms.allFragmentsSent())
					m_transport.m_ios.post(boost::bind(&OutboundMessageFragments::sendDataCallback, this, ps, msgId));
			}
		}

		void OutboundMessageFragments::timerCallback(const boost::system::error_code& e, PeerState ps, uint32_t const msgId)
		{
			if(!e) {
				std::lock_guard<std::mutex> lock(m_mutex);

				auto itr = m_states.find(msgId);
				if(itr != m_states.end()) {
					OutboundMessageState& oms = itr->second;

					if(oms.getTries() > 5) {
						m_timers.erase(msgId);
						return;
					}

					PacketBuilder::FragmentPtr frag = oms.getNextUnackdFragment();
					if(frag) {
						std::vector<PacketBuilder::FragmentPtr> fragList;
						fragList.push_back(frag);

						oms.markFragmentSent(fragList[0]->fragNum);

						PacketPtr p = PacketBuilder::buildData(ps.getEndpoint(), false, CompleteAckList(), PartialAckList(), fragList);
						p->encrypt(ps.getCurrentSessionKey(), ps.getCurrentMacKey());
						m_transport.sendPacket(p);

						oms.incrementTries();

						m_timers[msgId]->expires_at(m_timers[msgId]->expires_at() + boost::posix_time::time_duration(0, 0, 2));
						m_timers[msgId]->async_wait(boost::bind(&OutboundMessageFragments::timerCallback, this, boost::asio::placeholders::error, ps, msgId));
					}
				}
			}
		}
	}
}
