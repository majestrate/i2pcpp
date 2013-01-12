#include "EstablishmentState.h"

#include <botan/pipe.h>
#include <botan/lookup.h>
#include <botan/pubkey.h>
#include <botan/auto_rng.h>
#include <botan/pk_filts.h>

#include "../datatypes/Mapping.h"
#include "../util/Base64.h"

namespace i2pcpp {
	namespace SSU {
		EstablishmentState::EstablishmentState(RouterContext &ctx, RouterInfo const &ri, bool isInbound) : m_ctx(ctx), m_routerInfo(ri), m_isInbound(isInbound)
		{
			Botan::AutoSeeded_RNG rng;
			Botan::DL_Group shared_domain("modp/ietf/2048");

			Mapping m = ri.getAddress(0).getOptions();
			std::string host = m.getValue("host");
			unsigned short port = stoi(m.getValue("port"));

			m_dhPrivateKey = new Botan::DH_PrivateKey(rng, shared_domain);
			m_theirEndpoint = Endpoint(host, port);
			m_sessionKey = ri.getIdentity().getHash();
			m_macKey = m_sessionKey;
		}

		void EstablishmentState::calculateDHSecret()
		{
			Botan::DH_KA_Operation keyop(*m_dhPrivateKey);
			Botan::SymmetricKey secret = keyop.agree(m_theirDH.data(), m_theirDH.size());

			m_dhSecret.resize(secret.length());
			copy(secret.begin(), secret.end(), m_dhSecret.begin());
			if(m_dhSecret[0] & 0x80)
				m_dhSecret.insert(m_dhSecret.begin(), 0x00); // 2's comlpement
		}

		ByteArray EstablishmentState::calculateConfirmationSignature(const uint32_t signedOn) const
		{
			Botan::AutoSeeded_RNG rng;
			const Botan::DSA_PrivateKey *key = m_ctx.getSigningKey();

			Botan::Pipe sigPipe(new Botan::Hash_Filter("SHA-1"), new Botan::PK_Signer_Filter(new Botan::PK_Signer(*key, "Raw"), rng));
			sigPipe.start_msg();

			const ByteArray& myDH(m_dhPrivateKey->public_value());
			sigPipe.write(myDH.data(), myDH.size());
			sigPipe.write(m_theirDH.data(), m_theirDH.size());

			const ByteArray&& myIP = m_myEndpoint.getRawIP();
			unsigned short myPort =  m_myEndpoint.getPort();
			sigPipe.write(myIP.data(), myIP.size());
			sigPipe.write(myPort >> 8);
			sigPipe.write(myPort);

			const ByteArray&& theirIP = m_theirEndpoint.getRawIP();
			unsigned short theirPort = m_theirEndpoint.getPort();
			sigPipe.write(theirIP.data(), theirIP.size());
			sigPipe.write(theirPort >> 8);
			sigPipe.write(theirPort);

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

		bool EstablishmentState::verifyCreationSignature() const
		{
			Botan::InitializationVector iv(m_iv.data(), m_iv.size());
			Botan::SymmetricKey key(m_dhSecret.data(), 32);
			Botan::Pipe cipherPipe(get_cipher("AES-256/CBC/NoPadding", key, iv, Botan::DECRYPTION));

			cipherPipe.process_msg(m_signature.data(), m_signature.size());

			size_t decryptedSize = cipherPipe.remaining() - 8;
			Botan::secure_vector<Botan::byte> decryptedSig(decryptedSize);
			cipherPipe.read(decryptedSig.data(), decryptedSize);

			const Botan::DL_Group& group = m_ctx.getDSAParameters();

			const ByteArray&& dsaKeyBytes = m_routerInfo.getIdentity().getSigningKey();
			Botan::DSA_PublicKey dsaKey(group, Botan::BigInt(dsaKeyBytes.data(), dsaKeyBytes.size()));

			Botan::Pipe sigPipe(new Botan::Hash_Filter("SHA-1"), new Botan::PK_Verifier_Filter(new Botan::PK_Verifier(dsaKey, "Raw"), decryptedSig));
			sigPipe.start_msg();

			const ByteArray& myDH(m_dhPrivateKey->public_value());
			sigPipe.write(myDH.data(), myDH.size());
			sigPipe.write(m_theirDH.data(), m_theirDH.size());

			const ByteArray&& myIP = m_myEndpoint.getRawIP();
			unsigned short myPort =  m_myEndpoint.getPort();
			sigPipe.write(myIP.data(), myIP.size());
			sigPipe.write(myPort >> 8);
			sigPipe.write(myPort);

			const ByteArray&& theirIP = m_theirEndpoint.getRawIP();
			unsigned short theirPort = m_theirEndpoint.getPort();
			sigPipe.write(theirIP.data(), theirIP.size());
			sigPipe.write(theirPort >> 8);
			sigPipe.write(theirPort);

			sigPipe.write(m_relayTag.data(), m_relayTag.size());

			sigPipe.write(m_signatureTimestamp.data(), m_signatureTimestamp.size());

			sigPipe.end_msg();

			unsigned char verified;
			sigPipe.read(&verified, 1);

			return verified;
		}
	}
}
