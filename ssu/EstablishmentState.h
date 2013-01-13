#ifndef SSUESTABLISHMENTSTATE_H
#define SSUESTABLISHMENTSTATE_H

#include <memory>
#include <mutex>

#include <botan/dh.h>

#include "../datatypes/ByteArray.h"
#include "../datatypes/SessionKey.h"
#include "../datatypes/Endpoint.h"
#include "../datatypes/RouterIdentity.h"

#include "../RouterContext.h"

namespace i2pcpp {
	namespace SSU {
		class EstablishmentState {
			public:
				EstablishmentState(RouterContext &ctx, Endpoint const &ep, SessionKey const &sessionKey);
				EstablishmentState(RouterContext &ctx, Endpoint const &ep, SessionKey const &sessionKey, RouterIdentity const &ri);
				~EstablishmentState() { if(m_dhPrivateKey) delete m_dhPrivateKey; }

				enum State {
					UNKNOWN,
					REQUEST_SENT,
					REQUEST_RECEIVED,
					CREATED_SENT,
					CREATED_RECEIVED,
					CONFIRMED_RECEIVED,
					CONFIRMED_PARTIALLY,
					CONFIRMED_COMPLETELY,
					PENDING_INTRO,
					INTRODUCED,
					VALIDATION_FAILED,
				};

				bool isInbound() const { return m_isInbound; }
				State getState() const { return m_state; }
				const Endpoint& getTheirEndpoint() const { return m_theirEndpoint; }
				void setTheirIdentity(RouterIdentity const &ri) { m_theirIdentity = ri; }
				const RouterIdentity& getTheirIdentity() const { return m_theirIdentity; }
				const RouterContext& getContext() const { return m_ctx; }

				void setIV(ByteArray::const_iterator iv_begin, ByteArray::const_iterator iv_end)
				{
					ByteArray b(iv_begin, iv_end);
					m_iv = Botan::InitializationVector(b.data(), b.size());
				}
				const Botan::InitializationVector& getIV() const { return m_iv; }

				const SessionKey& getSessionKey() const { return m_sessionKey; }
				void setSessionKey(SessionKey const &sk) { m_sessionKey = sk; }

				const SessionKey& getMacKey() const { return m_macKey; }
				void setMacKey(SessionKey const &mk) { m_macKey = mk; }

				ByteArray getMyDH() const { return ByteArray(m_dhPrivateKey->public_value()); }
				void setTheirDH(ByteArray::const_iterator begin, ByteArray::const_iterator end) { m_theirDH = ByteArray(begin, end); }
				void calculateDHSecret();
				const ByteArray& getDHSecret() const { return m_dhSecret; }

				void setMyEndpoint(Endpoint const &ep) { m_myEndpoint = ep; }
				void setTheirEndpoint(Endpoint const &ep) { m_theirEndpoint = ep; }

				void setRelayTag(const uint32_t rt) { m_relayTag = rt; }
				uint32_t getRelayTag() const { return m_relayTag; }

				void setSignatureTimestamp(const uint32_t ts) { m_signatureTimestamp = ts; }

				void setSignature(ByteArray::const_iterator sig_begin, ByteArray::const_iterator sig_end) { m_signature = ByteArray(sig_begin, sig_end); }

				ByteArray calculateCreationSignature(const uint32_t signedOn);
				ByteArray calculateConfirmationSignature(const uint32_t signedOn) const;
				bool verifyCreationSignature() const;
				bool verifyConfirmationSignature() const;

				void requestSent() { m_state = REQUEST_SENT; }
				void requestReceived() { m_state = REQUEST_RECEIVED; }
				void createdSent() { m_state = CREATED_SENT; }
				void createdReceived() { m_state = CREATED_RECEIVED; }
				void confirmedReceived() { m_state = CONFIRMED_RECEIVED; }
				void confirmedPartially() { m_state = CONFIRMED_PARTIALLY; }
				void confirmedCompletely() { m_state = CONFIRMED_COMPLETELY; }
				void pendingIntro() { m_state = PENDING_INTRO; }
				void introduced() { m_state = INTRODUCED; }
				void validationFailed() { m_state = VALIDATION_FAILED; }

			private:
				RouterContext &m_ctx;

				Botan::DH_PrivateKey *m_dhPrivateKey;
				ByteArray m_theirDH;
				ByteArray m_dhSecret;

				State m_state;
				bool m_isInbound;

				Endpoint m_myEndpoint;
				Endpoint m_theirEndpoint;

				SessionKey m_sessionKey;
				SessionKey m_macKey;
				RouterIdentity m_theirIdentity;

				Botan::InitializationVector m_iv;
				uint32_t m_relayTag;
				uint32_t m_signatureTimestamp;
				ByteArray m_signature;
		};

		typedef std::shared_ptr<EstablishmentState> EstablishmentStatePtr;
	}
}

#endif
