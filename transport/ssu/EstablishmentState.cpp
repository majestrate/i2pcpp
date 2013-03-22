#include "EstablishmentState.h"

#include <botan/auto_rng.h>
#include <botan/pk_filts.h>

namespace i2pcpp {
	namespace SSU {
		const Botan::BigInt EstablishmentState::p = Botan::BigInt("0x9C05B2AA960D9B97B8931963C9CC9E8C3026E9B8ED92FAD0A69CC886D5BF8015FCADAE31A0AD18FAB3F01B00A358DE237655C4964AFAA2B337E96AD316B9FB1CC564B5AEC5B69A9FF6C3E4548707FEF8503D91DD8602E867E6D35D2235C1869CE2479C3B9D5401DE04E0727FB33D6511285D4CF29538D9E3B6051F5B22CC1C93");
		const Botan::BigInt EstablishmentState::q = Botan::BigInt("0xA5DFC28FEF4CA1E286744CD8EED9D29D684046B7");
		const Botan::BigInt EstablishmentState::g = Botan::BigInt("0xC1F4D27D40093B429E962D7223824E0BBC47E7C832A39236FC683AF84889581075FF9082ED32353D4374D7301CDA1D23C431F4698599DDA02451824FF369752593647CC3DDC197DE985E43D136CDCFC6BD5409CD2F450821142A5E6F8EB1C3AB5D0484B8129FCF17BCE4F7F33321C3CB3DBB14A905E7B2B3E93BE4708CBCC82");
		const Botan::DL_Group EstablishmentState::m_group = Botan::DL_Group(EstablishmentState::p, EstablishmentState::q, EstablishmentState::g);

		EstablishmentState::EstablishmentState(Botan::DSA_PrivateKey const &dsaKey, RouterIdentity const &myIdentity, Endpoint const &ep) :
			m_direction(EstablishmentState::INBOUND),
			m_dsaKey(dsaKey),
			m_myIdentity(myIdentity),
			m_sessionKey(myIdentity.getHash()),
			m_macKey(m_sessionKey),
			m_theirEndpoint(ep)
		{
			Botan::AutoSeeded_RNG rng;
			Botan::DL_Group dh_group("modp/ietf/2048");

			m_dhKey = new Botan::DH_PrivateKey(rng, dh_group);
		}

		EstablishmentState::EstablishmentState(Botan::DSA_PrivateKey const &dsaKey, RouterIdentity const &myIdentity, Endpoint const &ep, RouterIdentity const &theirIdentity) :
			EstablishmentState(dsaKey, myIdentity, ep)
		{
			m_direction = EstablishmentState::OUTBOUND;
			m_theirIdentity = theirIdentity;
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
			return m_theirIdentity;
		}

		void EstablishmentState::setTheirIdentity(RouterIdentity const &ri)
		{
			m_theirIdentity = ri;
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

			const ByteArray dsaKeyBytes = m_theirIdentity.getSigningKey();
			Botan::DSA_PublicKey dsaKey(m_group, Botan::BigInt(dsaKeyBytes.data(), dsaKeyBytes.size()));

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
			const ByteArray dsaKeyBytes = m_theirIdentity.getSigningKey();
			Botan::DSA_PublicKey dsaKey(m_group, Botan::BigInt(dsaKeyBytes.data(), dsaKeyBytes.size()));

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
	}
}
