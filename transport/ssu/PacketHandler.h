#ifndef SSUPACKETHANDLER_H
#define SSUPACKETHANDLER_H

#include "../../Log.h"

#include "Packet.h"
#include "PeerState.h"
#include "EstablishmentState.h"
#include "InboundMessageFragments.h"

namespace i2pcpp {
	class UDPTransport;

	namespace SSU {
		class PacketHandler {
			public:
				PacketHandler(UDPTransport &transport, SessionKey const &sk);

				void packetReceived(PacketPtr p);

			private:
				void handlePacket(PacketPtr const &packet, PeerStatePtr const &state);
				void handlePacket(PacketPtr const &packet, EstablishmentStatePtr const &state);
				void handlePacket(PacketPtr &p);

				void handleSessionRequest(ByteArrayConstItr &begin, ByteArrayConstItr end, EstablishmentStatePtr const &state);
				void handleSessionCreated(ByteArrayConstItr &begin, ByteArrayConstItr end, EstablishmentStatePtr const &state);
				void handleSessionConfirmed(ByteArrayConstItr &begin, ByteArrayConstItr end, EstablishmentStatePtr const &state);

				UDPTransport& m_transport;

				SessionKey m_inboundKey;

				InboundMessageFragments m_imf;

				i2p_logger_mt m_log;
		};
	}
}

#endif
