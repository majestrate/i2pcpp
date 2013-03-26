#include "OutboundMessageFragments.h"

#include "../UDPTransport.h"

namespace i2pcpp {
	namespace SSU {
		OutboundMessageFragments::OutboundMessageFragments(UDPTransport &transport) :
			m_transport(transport) {}

		void OutboundMessageFragments::sendData(PeerStatePtr const &ps, ByteArray const &data)
		{
		}
	}
}
