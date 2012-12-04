#ifndef SSUOUTBOUNDESTABLISHMENTSTATE_H
#define SSUOUTBOUNDESTABLISHMENTSTATE_H

#include <memory>
#include <array>
#include <mutex>

#include <botan/dh.h>

#include "../datatypes/ByteArray.h"
#include "../datatypes/SessionKey.h"
#include "../datatypes/Endpoint.h"
#include "../datatypes/RouterInfo.h"

using namespace std;
using namespace Botan;

namespace i2pcpp {
	class I2PContext;

	namespace SSU {
		class OutboundEstablishmentState {
			public:
				OutboundEstablishmentState(I2PContext &ctx, RouterInfo const &ri);
				~OutboundEstablishmentState() { if(m_dhPrivateKey) delete m_dhPrivateKey; }

				enum State {
					UNKNOWN,
					REQUEST_SENT,
					CREATED_RECEIVED,
					CONFIRMED_PARTIALLY,
					CONFIRMED_COMPLETELY,
					PENDING_INTRO,
					INTRODUCED,
					VALIDATION_FAILED
				};

				void lock() { m_mutex.lock(); }
				void unlock() { m_mutex.unlock(); }

				State getState() { return m_state; }
				Endpoint getEndpoint() { return m_endpoint; }

				const RouterIdentity& getIdentity() const { return m_routerInfo.getIdentity(); }
				const SessionKey& getSessionKey() const { return m_sessionKey; }
				void setSessionKey(SessionKey const &sk) { m_sessionKey = sk; }
				const SessionKey& getMacKey() const { return m_macKey; }
				void setMacKey(SessionKey const &mk) { m_macKey = mk; }

//				const ByteArray& getIV() const { return m_iv; }
				void setIV(ByteArray::const_iterator iv_begin, ByteArray::const_iterator iv_end) { m_iv = ByteArray(iv_begin, iv_end); }

				/* SessionRequest */
				ByteArray getDHX() { return ByteArray(m_dhPrivateKey->public_value()); }

				/* SessionCreated */
				void setDHY(ByteArray::const_iterator y_begin, ByteArray::const_iterator y_end) { m_DHY = ByteArray(y_begin, y_end); }
//				const ByteArray& getDHY() const { return m_DHY; }
				void setMyEndpoint(Endpoint const &ep) { m_myEndpoint = ep; }
//				Endpoint getMyEndpoint() const { return m_myEndpoint; }
				void setRelayTag(ByteArray::const_iterator tag_begin, ByteArray::const_iterator tag_end) { m_relayTag = ByteArray(tag_begin, tag_end); }
//				const ByteArray& getRelayTag() const { return m_relayTag; }
				void setSignatureTimestamp(ByteArray::const_iterator ts_begin, ByteArray::const_iterator ts_end) { m_signatureTimestamp = ByteArray(ts_begin, ts_end); }
//				const ByteArray& getSignatureTimestamp() const { return m_signatureTimestamp; }
				void setSignature(ByteArray::const_iterator sig_begin, ByteArray::const_iterator sig_end) { m_signature = ByteArray(sig_begin, sig_end); }
//				const ByteArray& getSignature() const { return m_signature; }
				void calculateDHSecret();
				const ByteArray& getDHSecret() const { return m_dhSecret; }
				ByteArray calculateConfirmationSignature(const unsigned int signedOn) const;
				bool verifyCreationSignature() const;

				void requestSent() { m_state = REQUEST_SENT; }
				void createdReceived() { m_state = CREATED_RECEIVED; }
				void confirmedPartially() { m_state = CONFIRMED_PARTIALLY; }
				void confirmedCompletely() { m_state = CONFIRMED_COMPLETELY; }
				void pendingIntro() { m_state = PENDING_INTRO; }
				void introduced() { m_state = INTRODUCED; }
				void validationFailed() { m_state = VALIDATION_FAILED; }

			private:
				mutex m_mutex;

				Endpoint m_endpoint;
				State m_state;
				SessionKey m_sessionKey;
				SessionKey m_macKey;
				RouterInfo m_routerInfo;

				ByteArray m_iv;

				/* SessionCreated */
				ByteArray m_DHY;
				Endpoint m_myEndpoint;
				ByteArray m_relayTag;
				ByteArray m_signatureTimestamp;
				ByteArray m_signature;

				DH_PrivateKey *m_dhPrivateKey;
				ByteArray m_dhSecret;

				I2PContext &m_context;
		};

		typedef shared_ptr<OutboundEstablishmentState> OutboundEstablishmentStatePtr;
	}
}

#endif
