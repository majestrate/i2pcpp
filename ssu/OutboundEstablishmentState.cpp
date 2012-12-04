#include "OutboundEstablishmentState.h"

#include <botan/pipe.h>
#include <botan/lookup.h>
#include <botan/pubkey.h>
#include <botan/auto_rng.h>
#include <botan/pk_filts.h>

#include "../i2p.h"

#include <string>

namespace i2pcpp {
	namespace SSU {
		OutboundEstablishmentState::OutboundEstablishmentState(I2PContext &ctx, RouterInfo const &ri) : m_state(PENDING_INTRO), m_routerInfo(ri), m_context(ctx)
		{
			AutoSeeded_RNG rng;
			DL_Group shared_domain("modp/ietf/2048");

			m_dhPrivateKey = new DH_PrivateKey(rng, shared_domain);

			string host = ri.getAddress(0).getHost();
			unsigned short port = ri.getAddress(0).getPort();
			m_endpoint = Endpoint(host, port);

			m_sessionKey = ri.getRouterHash();
			m_macKey = m_sessionKey;
		}

		void OutboundEstablishmentState::calculateDHSecret()
		{
			DH_KA_Operation keyop(*m_dhPrivateKey);
			SymmetricKey secret = keyop.agree(m_DHY.data(), m_DHY.size());

			m_dhSecret.resize(secret.length());
			copy(secret.begin(), secret.end(), m_dhSecret.begin());
			if(m_dhSecret[0] & 0x80)
				m_dhSecret.insert(m_dhSecret.begin(), 0x00); // 2's comlpement
		}
	
		ByteArray OutboundEstablishmentState::calculateConfirmationSignature(const unsigned int signedOn) const
		{
			AutoSeeded_RNG rng;
			const DL_Group& group = m_context.getDSAParameters();
			const DSA_PrivateKey *key = m_context.getSigningKey();

			Pipe sigPipe(new Hash_Filter("SHA-1"), new PK_Signer_Filter(new PK_Signer(*key, "Raw"), rng));
			sigPipe.start_msg();

			ByteArray DHX(m_dhPrivateKey->public_value());
			sigPipe.write(DHX.data(), DHX.size());
			sigPipe.write(m_DHY.data(), m_DHY.size());

			ByteArray aliceIP = m_myEndpoint.getRawIP();
			unsigned short alicePort =  m_myEndpoint.getPort();
			sigPipe.write(aliceIP.data(), aliceIP.size());
			sigPipe.write(alicePort >> 8);
			sigPipe.write(alicePort);

			ByteArray bobIP = m_endpoint.getRawIP();
			unsigned short bobPort = m_endpoint.getPort();
			sigPipe.write(bobIP.data(), bobIP.size());
			bobPort = 10673;
			sigPipe.write(bobPort >> 8);
			sigPipe.write(bobPort);

			sigPipe.write(m_relayTag.data(), m_relayTag.size());

			sigPipe.write(signedOn >> 24);
			sigPipe.write(signedOn >> 16);
			sigPipe.write(signedOn >> 8);
			sigPipe.write(signedOn);

			sigPipe.end_msg();

			ByteArray signature(sigPipe.remaining());
			sigPipe.read(signature.data(), sigPipe.remaining());

			return signature;
		}

		bool OutboundEstablishmentState::verifyCreationSignature() const
		{
			InitializationVector iv(m_iv.data(), m_iv.size());
			SymmetricKey key(m_dhSecret.data(), 32);
			Pipe cipherPipe(get_cipher("AES-256/CBC/NoPadding", key, iv, DECRYPTION));

			cipherPipe.process_msg(m_signature.data(), m_signature.size());

			size_t decryptedSize = cipherPipe.remaining() - 8;
			secure_vector<byte> decryptedSig(decryptedSize);
			cipherPipe.read(decryptedSig.data(), decryptedSize);

			const DL_Group& group = m_context.getDSAParameters();

			ByteArray dsaKeyBytes = m_routerInfo.getIdentity().getSigningKey();
			DSA_PublicKey dsaKey(group, BigInt(dsaKeyBytes.data(), dsaKeyBytes.size()));
			PK_Verifier v(dsaKey, "Raw");

			Pipe sigPipe(new Hash_Filter("SHA-1"), new PK_Verifier_Filter(new PK_Verifier(dsaKey, "Raw"), decryptedSig));
			sigPipe.start_msg();

			ByteArray DHX(m_dhPrivateKey->public_value());
			sigPipe.write(DHX.data(), DHX.size());
			sigPipe.write(m_DHY.data(), m_DHY.size());

			ByteArray aliceIP = m_myEndpoint.getRawIP();
			unsigned short alicePort =  m_myEndpoint.getPort();
			sigPipe.write(aliceIP.data(), aliceIP.size());
			sigPipe.write(alicePort >> 8);
			sigPipe.write(alicePort);

			ByteArray bobIP = m_endpoint.getRawIP();
			unsigned short bobPort = m_endpoint.getPort();
			sigPipe.write(bobIP.data(), bobIP.size());
			bobPort = 10673;
			sigPipe.write(bobPort >> 8);
			sigPipe.write(bobPort);

			sigPipe.write(m_relayTag.data(), m_relayTag.size());

			sigPipe.write(m_signatureTimestamp.data(), m_signatureTimestamp.size());

			sigPipe.end_msg();

			unsigned char verified;
			sigPipe.read(&verified, 1);

			return verified;
		}
	}
}
