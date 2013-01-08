#ifndef SSUESTABLISHMENTSTATE_H
#define SSUESTABLISHMENTSTATE_H

#include <memory>
#include <mutex>

#include <botan/dh.h>

#include "../datatypes/ByteArray.h"
#include "../datatypes/SessionKey.h"
#include "../datatypes/Endpoint.h"
#include "../datatypes/RouterInfo.h"

#include "../RouterContext.h"

namespace i2pcpp {
	namespace SSU {
		class EstablishmentState {
			public:
				EstablishmentState(RouterContext &ctx, RouterInfo const &ri, bool isInbound);
				~EstablishmentState() { if(m_dhPrivateKey) delete m_dhPrivateKey; }

				enum State {
					UNKNOWN,
					REQUEST_SENT,
					REQUEST_RECEIVED,
					CREATED_SENT,
					CREATED_RECEIVED,
					CONFIRMED_PARTIALLY,
					CONFIRMED_COMPLETELY,
					PENDING_INTRO,
					INTRODUCED,
					VALIDATION_FAILED,
				};

				bool isInbound() const { return m_isInbound; }
				State getState() const { return m_state; }
				std::mutex& getMutex() const { return m_mutex; }
				const Endpoint& getTheirEndpoint() const { return m_theirEndpoint; }
				const RouterContext& getContext() const { return m_ctx; }
				const RouterIdentity& getIdentity() const { return m_routerInfo.getIdentity(); }

				void setIV(ByteArray::const_iterator iv_begin, ByteArray::const_iterator iv_end) { m_iv = ByteArray(iv_begin, iv_end); }

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
				void setRelayTag(ByteArray::const_iterator tag_begin, ByteArray::const_iterator tag_end) { m_relayTag = ByteArray(tag_begin, tag_end); }
				void setSignatureTimestamp(ByteArray::const_iterator ts_begin, ByteArray::const_iterator ts_end) { m_signatureTimestamp = ByteArray(ts_begin, ts_end); }
				void setSignature(ByteArray::const_iterator sig_begin, ByteArray::const_iterator sig_end) { m_signature = ByteArray(sig_begin, sig_end); }

				ByteArray calculateConfirmationSignature(const uint32_t signedOn) const;
				bool verifyCreationSignature() const;

				void requestSent() { m_state = REQUEST_SENT; }
				void requestReceived() { m_state = REQUEST_RECEIVED; }
				void createdSent() { m_state = CREATED_SENT; }
				void createdReceived() { m_state = CREATED_RECEIVED; }
				void confirmedPartially() { m_state = CONFIRMED_PARTIALLY; }
				void confirmedCompletely() { m_state = CONFIRMED_COMPLETELY; }
				void pendingIntro() { m_state = PENDING_INTRO; }
				void introduced() { m_state = INTRODUCED; }
				void validationFailed() { m_state = VALIDATION_FAILED; }

			private:
				mutable std::mutex m_mutex;
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
				RouterInfo m_routerInfo;

				ByteArray m_iv;
				ByteArray m_relayTag;
				ByteArray m_signatureTimestamp;
				ByteArray m_signature;
		};

		typedef std::shared_ptr<EstablishmentState> EstablishmentStatePtr;
	}
}

#endif
