#include "OutboundMessageFragments.h"

#include "../UDPTransport.h"

namespace i2pcpp {
	namespace SSU {
		OutboundMessageFragments::OutboundMessageFragments(UDPTransport &transport) :
			m_transport(transport) {}

		void OutboundMessageFragments::sendData(PeerStatePtr const &ps, ByteArray const &data)
		{
			auto oms = std::make_shared<OutboundMessageState>(data);

			ps->addOutboundMessageState(oms);

			m_transport.post(boost::bind(&OutboundMessageFragments::sendDataCallback, this, ps, oms));
		}

		void OutboundMessageFragments::sendDataCallback(PeerStatePtr ps, OutboundMessageStatePtr oms)
		{
			std::vector<PacketBuilder::FragmentPtr> fragList;
			fragList.push_back(oms->getNextFragment());

			oms->markFragmentSent(fragList[0]->fragNum);

			PacketPtr p = PacketBuilder::buildData(ps->getEndpoint(), false, CompleteAckList(), PartialAckList(), fragList);
			p->encrypt(ps->getCurrentSessionKey(), ps->getCurrentMacKey());
			m_transport.sendPacket(p);

			if(!oms->allFragmentsSent())
				m_transport.post(boost::bind(&OutboundMessageFragments::sendDataCallback, this, ps, oms));
		}
	}
}
