#include "OutboundMessageFragments.h"

#include "../UDPTransport.h"

namespace i2pcpp {
	namespace SSU {
		OutboundMessageFragments::OutboundMessageFragments(UDPTransport &transport) :
			m_transport(transport) {}

		void OutboundMessageFragments::sendData(PeerStatePtr const &ps, ByteArray const &data)
		{
			std::lock_guard<std::mutex> lock(ps->getMutex());

			auto oms = std::make_shared<OutboundMessageState>(data);

			ps->addOutboundMessageState(oms);

			FragmentTimerPtr timer(new boost::asio::deadline_timer(m_transport.m_ios, boost::posix_time::time_duration(0, 0, 2)));
			timer->async_wait(boost::bind(&OutboundMessageFragments::fragmentTimerCallback, this, ps, oms->getMsgId(), timer));

			m_transport.post(boost::bind(&OutboundMessageFragments::sendDataCallback, this, ps, oms->getMsgId()));
		}

		void OutboundMessageFragments::sendDataCallback(PeerStatePtr ps, uint32_t msgId)
		{
			std::lock_guard<std::mutex> lock(ps->getMutex());

			OutboundMessageStatePtr oms = ps->getOutboundMessageState(msgId);

			if(oms) {
				std::vector<PacketBuilder::FragmentPtr> fragList;
				fragList.push_back(oms->getNextFragment());

				oms->markFragmentSent(fragList[0]->fragNum);

				PacketPtr p = PacketBuilder::buildData(ps->getEndpoint(), false, CompleteAckList(), PartialAckList(), fragList);
				p->encrypt(ps->getCurrentSessionKey(), ps->getCurrentMacKey());
				m_transport.sendPacket(p);

				if(!oms->allFragmentsSent())
					m_transport.post(boost::bind(&OutboundMessageFragments::sendDataCallback, this, ps, oms->getMsgId()));
			}
		}

		void OutboundMessageFragments::fragmentTimerCallback(PeerStatePtr ps, uint32_t msgId, FragmentTimerPtr timer)
		{
			std::lock_guard<std::mutex> lock(ps->getMutex());

			OutboundMessageStatePtr oms = ps->getOutboundMessageState(msgId);

			if(oms) {
				PacketBuilder::FragmentPtr frag = oms->getNextUnackdFragment();
				if(frag) {
					std::vector<PacketBuilder::FragmentPtr> fragList;
					fragList.push_back(frag);

					oms->markFragmentSent(fragList[0]->fragNum);

					PacketPtr p = PacketBuilder::buildData(ps->getEndpoint(), false, CompleteAckList(), PartialAckList(), fragList);
					p->encrypt(ps->getCurrentSessionKey(), ps->getCurrentMacKey());
					m_transport.sendPacket(p);

					timer->expires_at(timer->expires_at() + boost::posix_time::time_duration(0, 0, 2));
					timer->async_wait(boost::bind(&OutboundMessageFragments::fragmentTimerCallback, this, ps, oms->getMsgId(), timer));
				}
			}
		}
	}
}
