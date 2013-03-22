#ifndef SSUESTABLISHMENTSTATE_H
#define SSUESTABLISHMENTSTATE_H

#include <memory>
#include <mutex>

#include <botan/dh.h>
#include <botan/dsa.h>
#include <botan/botan.h>

#include "../../datatypes/Endpoint.h"
#include "../../datatypes/SessionKey.h"
#include "../../datatypes/RouterIdentity.h"

namespace i2pcpp {
	namespace SSU {
		class EstablishmentState {
			public:
				EstablishmentState(Botan::DSA_PrivateKey const &dsaKey, RouterIdentity const &myIdentity, Endpoint const &ep);
				EstablishmentState(Botan::DSA_PrivateKey const &dsaKey, RouterIdentity const &myIdentity, Endpoint const &ep, RouterIdentity const &theirIdentity);
				EstablishmentState(EstablishmentState const &state) = delete;
				~EstablishmentState();

				enum Direction {
					INBOUND,
					OUTBOUND
				};

				enum State {
					UNKNOWN,
					REQUEST_SENT,
					REQUEST_RECEIVED,
					CREATED_SENT,
					CREATED_RECEIVED,
					CONFIRMED_SENT,
					CONFIRMED_RECEIVED,
					FAILURE
				};

				Direction getDirection() const;

				State getState() const;
				void setState(State state);

				Botan::InitializationVector getIV() const;
				void setIV(ByteArrayConstItr iv_begin, ByteArrayConstItr iv_end);

				const SessionKey& getSessionKey() const;
				void setSessionKey(SessionKey const &sk);

				const SessionKey& getMacKey() const;
				void setMacKey(SessionKey const &mk);

				const Endpoint& getTheirEndpoint() const;
				void setMyEndpoint(Endpoint const &ep);

				uint32_t getRelayTag() const;
				void setRelayTag(const uint32_t rt);

				const RouterIdentity& getTheirIdentity() const;
				void setTheirIdentity(RouterIdentity const &ri);

				const RouterIdentity& getMyIdentity() const;

				ByteArray getMyDH() const;
				void setTheirDH(ByteArrayConstItr dh_begin, ByteArrayConstItr dh_end);

				void setSignatureTimestamp(const uint32_t ts);

				void setSignature(ByteArrayConstItr sig_begin, ByteArrayConstItr sig_end);

				ByteArray calculateCreationSignature(const uint32_t signedOn);
				ByteArray calculateConfirmationSignature(const uint32_t signedOn) const;
				bool verifyCreationSignature() const;
				bool verifyConfirmationSignature() const;

				void calculateDHSecret();
				const ByteArray& getDHSecret() const;

			private:
				State m_state = UNKNOWN;
				Direction m_direction;

				static const Botan::BigInt p;
				static const Botan::BigInt q;
				static const Botan::BigInt g;
				static const Botan::DL_Group m_group;

				const Botan::DSA_PrivateKey& m_dsaKey;
				const RouterIdentity& m_myIdentity;
				Endpoint m_myEndpoint;

				Botan::InitializationVector m_iv;
				Botan::DH_PrivateKey *m_dhKey;
				ByteArray m_dhSecret;
				SessionKey m_sessionKey;
				SessionKey m_macKey;

				RouterIdentity m_theirIdentity;
				Endpoint m_theirEndpoint;
				ByteArray m_theirDH;

				uint32_t m_relayTag;
				uint32_t m_signatureTimestamp;
				ByteArray m_signature;
		};

		typedef std::shared_ptr<EstablishmentState> EstablishmentStatePtr;
	}
}

#endif
