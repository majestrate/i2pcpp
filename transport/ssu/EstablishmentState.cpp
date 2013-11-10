#include "EstablishmentState.h"

#include <botan/auto_rng.h>
#include <botan/pk_filts.h>

#include "../../util/I2PDH.h"

namespace i2pcpp {
	namespace SSU {
		EstablishmentState::EstablishmentState(Botan::DSA_PrivateKey const &dsaKey, RouterIdentity const &myIdentity, Endpoint const &ep) :
			m_direction(EstablishmentState::Direction::INBOUND),
			m_dsaKey(dsaKey),
			m_myIdentity(myIdentity),
			m_sessionKey((ByteArray)myIdentity.getHash()),
			m_macKey(m_sessionKey),
			m_theirEndpoint(ep)
		{
			Botan::AutoSeeded_RNG rng;
			Botan::DL_Group dh_group("modp/ietf/2048");

			m_dhKey = new Botan::DH_PrivateKey(rng, dh_group);
		}

		EstablishmentState::EstablishmentState(Botan::DSA_PrivateKey const &dsaKey, RouterIdentity const &myIdentity, Endpoint const &ep, RouterIdentity const &theirIdentity) :
			m_direction(EstablishmentState::Direction::OUTBOUND),
			m_dsaKey(dsaKey),
			m_myIdentity(myIdentity),
			m_sessionKey((ByteArray)theirIdentity.getHash()),
			m_macKey(m_sessionKey),
			m_theirEndpoint(ep),
			m_theirIdentity(std::make_shared<RouterIdentity>(theirIdentity))
		{
			Botan::AutoSeeded_RNG rng;
			Botan::DL_Group dh_group("modp/ietf/2048");

			m_dhKey = new Botan::DH_PrivateKey(rng, dh_group);
		}

		EstablishmentState::~EstablishmentState()
		{
			if(m_dhKey)
				delete m_dhKey;
		}

		EstablishmentState::Direction EstablishmentState::getDirection() const
		{
			return m_direction;
		}

		EstablishmentState::State EstablishmentState::getState() const
		{
			return m_state;
		}

		void EstablishmentState::setState(EstablishmentState::State state)
		{
			m_state = state;
		}

		Botan::InitializationVector EstablishmentState::getIV() const
		{
			return m_iv;
		}

		void EstablishmentState::setIV(ByteArrayConstItr iv_begin, ByteArrayConstItr iv_end)
		{
			ByteArray b(iv_begin, iv_end);
			m_iv = Botan::InitializationVector(b.data(), b.size());
		}

		const SessionKey& EstablishmentState::getSessionKey() const
		{
			return m_sessionKey;
		}

		void EstablishmentState::setSessionKey(SessionKey const &sk)
		{
			m_sessionKey = sk;
		}

		const SessionKey& EstablishmentState::getMacKey() const
		{
			return m_macKey;
		}

		void EstablishmentState::setMacKey(SessionKey const &mk)
		{
			m_macKey = mk;
		}

		const Endpoint& EstablishmentState::getTheirEndpoint() const
		{
			return m_theirEndpoint;
		}

		void EstablishmentState::setMyEndpoint(Endpoint const &ep)
		{
			m_myEndpoint = ep;
		}

		uint32_t EstablishmentState::getRelayTag() const
		{
			return m_relayTag;
		}

		void EstablishmentState::setRelayTag(const uint32_t rt)
		{
			m_relayTag = rt;
		}

		const RouterIdentity& EstablishmentState::getTheirIdentity() const
		{
			return *m_theirIdentity;
		}

		void EstablishmentState::setTheirIdentity(RouterIdentity const &ri)
		{
			m_theirIdentity = std::make_shared<RouterIdentity>(ri);
		}

		const RouterIdentity& EstablishmentState::getMyIdentity() const
		{
			return m_myIdentity;
		}

		ByteArray EstablishmentState::getMyDH() const
		{
			return ByteArray(m_dhKey->public_value());
		}

		void EstablishmentState::setTheirDH(ByteArrayConstItr dh_begin, ByteArrayConstItr dh_end)
		{
			m_theirDH = ByteArray(dh_begin, dh_end);
		}

		void EstablishmentState::setSignatureTimestamp(const uint32_t ts)
		{
			m_signatureTimestamp = ts;
		}

		void EstablishmentState::setSignature(ByteArrayConstItr sig_begin, ByteArrayConstItr sig_end)
		{
			m_signature = ByteArray(sig_begin, sig_end);
		}

		ByteArray EstablishmentState::calculateCreationSignature(const uint32_t signedOn)
		{
			Botan::AutoSeeded_RNG rng;

			Botan::Pipe sigPipe(
					new Botan::Hash_Filter("SHA-1"),
					new Botan::PK_Signer_Filter(
						new Botan::PK_Signer(m_dsaKey, "Raw"),
						rng
						));

			sigPipe.start_msg();

			sigPipe.write(m_theirDH.data(), m_theirDH.size());
			const ByteArray myDH(m_dhKey->public_value());
			sigPipe.write(myDH.data(), myDH.size());

			const ByteArray theirIP = m_theirEndpoint.getRawIP();
			unsigned short theirPort = m_theirEndpoint.getPort();
			sigPipe.write(theirIP.data(), theirIP.size());
			sigPipe.write(theirPort >> 8);
			sigPipe.write(theirPort);

			const ByteArray myIP = m_myEndpoint.getRawIP();
			unsigned short myPort =  m_myEndpoint.getPort();
			sigPipe.write(myIP.data(), myIP.size());
			sigPipe.write(myPort >> 8);
			sigPipe.write(myPort);

			sigPipe.write(m_relayTag >> 24);
			sigPipe.write(m_relayTag >> 16);
			sigPipe.write(m_relayTag >> 8);
			sigPipe.write(m_relayTag);

			sigPipe.write(signedOn >> 24);
			sigPipe.write(signedOn >> 16);
			sigPipe.write(signedOn >> 8);
			sigPipe.write(signedOn);

			sigPipe.end_msg();

			size_t size = sigPipe.remaining();
			ByteArray signature(size);
			sigPipe.read(signature.data(), size);

			unsigned char padSize = 16 - (size % 16);
			if(padSize < 16)
				signature.insert(signature.end(), padSize, padSize);

			Botan::SymmetricKey encKey(m_dhSecret.data(), 32);
			m_iv = Botan::InitializationVector(rng, 16);
			Botan::Pipe encPipe(get_cipher("AES-256/CBC/NoPadding", encKey, m_iv, Botan::ENCRYPTION));
			encPipe.process_msg(signature.data(), signature.size());

			size = encPipe.remaining();
			ByteArray encSignature(size);
			encPipe.read(encSignature.data(), size);

			return encSignature;
		}

		ByteArray EstablishmentState::calculateConfirmationSignature(const uint32_t signedOn) const
		{
			Botan::AutoSeeded_RNG rng;

			Botan::Pipe sigPipe(new Botan::Hash_Filter("SHA-1"), new Botan::PK_Signer_Filter(new Botan::PK_Signer(m_dsaKey, "Raw"), rng));
			sigPipe.start_msg();

			const ByteArray myDH(m_dhKey->public_value());
			sigPipe.write(myDH.data(), myDH.size());
			sigPipe.write(m_theirDH.data(), m_theirDH.size());

			const ByteArray myIP = m_myEndpoint.getRawIP();
			unsigned short myPort =  m_myEndpoint.getPort();
			sigPipe.write(myIP.data(), myIP.size());
			sigPipe.write(myPort >> 8);
			sigPipe.write(myPort);

			const ByteArray theirIP = m_theirEndpoint.getRawIP();
			unsigned short theirPort = m_theirEndpoint.getPort();
			sigPipe.write(theirIP.data(), theirIP.size());
			sigPipe.write(theirPort >> 8);
			sigPipe.write(theirPort);

			sigPipe.write(m_relayTag >> 24);
			sigPipe.write(m_relayTag >> 16);
			sigPipe.write(m_relayTag >> 8);
			sigPipe.write(m_relayTag);

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
			Botan::SymmetricKey key(m_dhSecret.data(), 32);
			Botan::Pipe cipherPipe(get_cipher("AES-256/CBC/NoPadding", key, m_iv, Botan::DECRYPTION));

			cipherPipe.process_msg(m_signature.data(), m_signature.size());

			size_t decryptedSize = cipherPipe.remaining() - 8;
			Botan::secure_vector<Botan::byte> decryptedSig(decryptedSize);
			cipherPipe.read(decryptedSig.data(), decryptedSize);

			const ByteArray dsaKeyBytes = m_theirIdentity->getSigningKey();
			Botan::DSA_PublicKey dsaKey(DH::getGroup(), Botan::BigInt(dsaKeyBytes.data(), dsaKeyBytes.size()));

			Botan::Pipe sigPipe(new Botan::Hash_Filter("SHA-1"), new Botan::PK_Verifier_Filter(new Botan::PK_Verifier(dsaKey, "Raw"), decryptedSig));
			sigPipe.start_msg();

			const ByteArray& myDH(m_dhKey->public_value());
			sigPipe.write(myDH.data(), myDH.size());
			sigPipe.write(m_theirDH.data(), m_theirDH.size());

			const ByteArray myIP = m_myEndpoint.getRawIP();
			unsigned short myPort =  m_myEndpoint.getPort();
			sigPipe.write(myIP.data(), myIP.size());
			sigPipe.write(myPort >> 8);
			sigPipe.write(myPort);

			const ByteArray theirIP = m_theirEndpoint.getRawIP();
			unsigned short theirPort = m_theirEndpoint.getPort();
			sigPipe.write(theirIP.data(), theirIP.size());
			sigPipe.write(theirPort >> 8);
			sigPipe.write(theirPort);

			sigPipe.write(m_relayTag >> 24);
			sigPipe.write(m_relayTag >> 16);
			sigPipe.write(m_relayTag >> 8);
			sigPipe.write(m_relayTag);

			sigPipe.write(m_signatureTimestamp >> 24);
			sigPipe.write(m_signatureTimestamp >> 16);
			sigPipe.write(m_signatureTimestamp >> 8);
			sigPipe.write(m_signatureTimestamp);

			sigPipe.end_msg();

			unsigned char verified;
			sigPipe.read(&verified, 1);

			return verified;
		}

		bool EstablishmentState::verifyConfirmationSignature() const
		{
			const ByteArray dsaKeyBytes = m_theirIdentity->getSigningKey();
			Botan::DSA_PublicKey dsaKey(DH::getGroup(), Botan::BigInt(dsaKeyBytes.data(), dsaKeyBytes.size()));

			Botan::secure_vector<Botan::byte> sig(m_signature.cbegin(), m_signature.cend());
			Botan::Pipe sigPipe(new Botan::Hash_Filter("SHA-1"), new Botan::PK_Verifier_Filter(new Botan::PK_Verifier(dsaKey, "Raw"), sig));
			sigPipe.start_msg();

			sigPipe.write(m_theirDH.data(), m_theirDH.size());
			const ByteArray& myDH(m_dhKey->public_value());
			sigPipe.write(myDH.data(), myDH.size());

			const ByteArray theirIP = m_theirEndpoint.getRawIP();
			unsigned short theirPort = m_theirEndpoint.getPort();
			sigPipe.write(theirIP.data(), theirIP.size());
			sigPipe.write(theirPort >> 8);
			sigPipe.write(theirPort);

			const ByteArray myIP = m_myEndpoint.getRawIP();
			unsigned short myPort =  m_myEndpoint.getPort();
			sigPipe.write(myIP.data(), myIP.size());
			sigPipe.write(myPort >> 8);
			sigPipe.write(myPort);

			sigPipe.write(m_relayTag >> 24);
			sigPipe.write(m_relayTag >> 16);
			sigPipe.write(m_relayTag >> 8);
			sigPipe.write(m_relayTag);

			sigPipe.write(m_signatureTimestamp >> 24);
			sigPipe.write(m_signatureTimestamp >> 16);
			sigPipe.write(m_signatureTimestamp >> 8);
			sigPipe.write(m_signatureTimestamp);

			sigPipe.end_msg();

			unsigned char verified;
			sigPipe.read(&verified, 1);

			return verified;
		}

		void EstablishmentState::calculateDHSecret()
		{
			Botan::DH_KA_Operation keyop(*m_dhKey);
			Botan::SymmetricKey secret = keyop.agree(m_theirDH.data(), m_theirDH.size());

			m_dhSecret.resize(secret.length());
			copy(secret.begin(), secret.end(), m_dhSecret.begin());
			if(m_dhSecret[0] & 0x80)
				m_dhSecret.insert(m_dhSecret.begin(), 0x00); // 2's comlpement

		}

		const ByteArray& EstablishmentState::getDHSecret() const
		{
			return m_dhSecret;
		}
	}
}
