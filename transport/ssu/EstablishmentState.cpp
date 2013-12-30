/**
 * @file EstablishmentState.cpp
 * @brief Implements EstablishmentState.h
 */
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
            m_sessionKey(myIdentity.getHash()),
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
            m_sessionKey(theirIdentity.getHash()),
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
            Botan::Pipe hashPipe(new Botan::Hash_Filter("SHA-1"));

            hashPipe.start_msg();

            hashPipe.write(m_theirDH.data(), m_theirDH.size());
            const ByteArray myDH(m_dhKey->public_value());
            hashPipe.write(myDH.data(), myDH.size());

            const ByteArray theirIP = m_theirEndpoint.getRawIP();
            unsigned short theirPort = m_theirEndpoint.getPort();
            hashPipe.write(theirIP.data(), theirIP.size());
            hashPipe.write(theirPort >> 8);
            hashPipe.write(theirPort);

            const ByteArray myIP = m_myEndpoint.getRawIP();
            unsigned short myPort =  m_myEndpoint.getPort();
            hashPipe.write(myIP.data(), myIP.size());
            hashPipe.write(myPort >> 8);
            hashPipe.write(myPort);

            hashPipe.write(m_relayTag >> 24);
            hashPipe.write(m_relayTag >> 16);
            hashPipe.write(m_relayTag >> 8);
            hashPipe.write(m_relayTag);

            hashPipe.write(signedOn >> 24);
            hashPipe.write(signedOn >> 16);
            hashPipe.write(signedOn >> 8);
            hashPipe.write(signedOn);

            hashPipe.end_msg();

            ByteArray hash(20);
            hashPipe.read(hash.data(), 20);

            Botan::AutoSeeded_RNG rng;
            Botan::PK_Signer *pks = new Botan::PK_Signer(m_dsaKey, "Raw");
            ByteArray signature = pks->sign_message(hash, rng);

            unsigned char padSize = 16 - (signature.size() % 16);
            if(padSize < 16)
                signature.insert(signature.end(), padSize, padSize);

            Botan::SymmetricKey encKey(m_dhSecret.data(), 32);
            m_iv = Botan::InitializationVector(rng, 16);
            Botan::Pipe encPipe(get_cipher("AES-256/CBC/NoPadding", encKey, m_iv, Botan::ENCRYPTION));
            encPipe.process_msg(signature.data(), signature.size());

            size_t size = encPipe.remaining();
            ByteArray encSignature(size);
            encPipe.read(encSignature.data(), size);

            return encSignature;
        }

        ByteArray EstablishmentState::calculateConfirmationSignature(const uint32_t signedOn) const
        {
            Botan::Pipe hashPipe(new Botan::Hash_Filter("SHA-1"));

            hashPipe.start_msg();

            const ByteArray myDH(m_dhKey->public_value());
            hashPipe.write(myDH.data(), myDH.size());
            hashPipe.write(m_theirDH.data(), m_theirDH.size());

            const ByteArray myIP = m_myEndpoint.getRawIP();
            unsigned short myPort =  m_myEndpoint.getPort();
            hashPipe.write(myIP.data(), myIP.size());
            hashPipe.write(myPort >> 8);
            hashPipe.write(myPort);

            const ByteArray theirIP = m_theirEndpoint.getRawIP();
            unsigned short theirPort = m_theirEndpoint.getPort();
            hashPipe.write(theirIP.data(), theirIP.size());
            hashPipe.write(theirPort >> 8);
            hashPipe.write(theirPort);

            hashPipe.write(m_relayTag >> 24);
            hashPipe.write(m_relayTag >> 16);
            hashPipe.write(m_relayTag >> 8);
            hashPipe.write(m_relayTag);

            hashPipe.write(signedOn >> 24);
            hashPipe.write(signedOn >> 16);
            hashPipe.write(signedOn >> 8);
            hashPipe.write(signedOn);

            hashPipe.end_msg();

            ByteArray hash(20);
            hashPipe.read(hash.data(), 20);

            Botan::AutoSeeded_RNG rng;
            Botan::PK_Signer *pks = new Botan::PK_Signer(m_dsaKey, "Raw");
            ByteArray signature = pks->sign_message(hash, rng);

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

            Botan::Pipe hashPipe(new Botan::Hash_Filter("SHA-1"));
            hashPipe.start_msg();

            const ByteArray& myDH(m_dhKey->public_value());
            hashPipe.write(myDH.data(), myDH.size());
            hashPipe.write(m_theirDH.data(), m_theirDH.size());

            const ByteArray myIP = m_myEndpoint.getRawIP();
            unsigned short myPort =  m_myEndpoint.getPort();
            hashPipe.write(myIP.data(), myIP.size());
            hashPipe.write(myPort >> 8);
            hashPipe.write(myPort);

            const ByteArray theirIP = m_theirEndpoint.getRawIP();
            unsigned short theirPort = m_theirEndpoint.getPort();
            hashPipe.write(theirIP.data(), theirIP.size());
            hashPipe.write(theirPort >> 8);
            hashPipe.write(theirPort);

            hashPipe.write(m_relayTag >> 24);
            hashPipe.write(m_relayTag >> 16);
            hashPipe.write(m_relayTag >> 8);
            hashPipe.write(m_relayTag);

            hashPipe.write(m_signatureTimestamp >> 24);
            hashPipe.write(m_signatureTimestamp >> 16);
            hashPipe.write(m_signatureTimestamp >> 8);
            hashPipe.write(m_signatureTimestamp);

            hashPipe.end_msg();

            ByteArray hash(20);
            hashPipe.read(hash.data(), 20);

            Botan::AutoSeeded_RNG rng;
            Botan::PK_Verifier *pkv = new Botan::PK_Verifier(dsaKey, "Raw");

            return pkv->verify_message(hash, decryptedSig);
        }

        bool EstablishmentState::verifyConfirmationSignature() const
        {
            const ByteArray dsaKeyBytes = m_theirIdentity->getSigningKey();
            Botan::DSA_PublicKey dsaKey(DH::getGroup(), Botan::BigInt(dsaKeyBytes.data(), dsaKeyBytes.size()));

            Botan::Pipe hashPipe(new Botan::Hash_Filter("SHA-1"));
            hashPipe.start_msg();

            hashPipe.write(m_theirDH.data(), m_theirDH.size());
            const ByteArray& myDH(m_dhKey->public_value());
            hashPipe.write(myDH.data(), myDH.size());

            const ByteArray theirIP = m_theirEndpoint.getRawIP();
            unsigned short theirPort = m_theirEndpoint.getPort();
            hashPipe.write(theirIP.data(), theirIP.size());
            hashPipe.write(theirPort >> 8);
            hashPipe.write(theirPort);

            const ByteArray myIP = m_myEndpoint.getRawIP();
            unsigned short myPort =  m_myEndpoint.getPort();
            hashPipe.write(myIP.data(), myIP.size());
            hashPipe.write(myPort >> 8);
            hashPipe.write(myPort);

            hashPipe.write(m_relayTag >> 24);
            hashPipe.write(m_relayTag >> 16);
            hashPipe.write(m_relayTag >> 8);
            hashPipe.write(m_relayTag);

            hashPipe.write(m_signatureTimestamp >> 24);
            hashPipe.write(m_signatureTimestamp >> 16);
            hashPipe.write(m_signatureTimestamp >> 8);
            hashPipe.write(m_signatureTimestamp);

            hashPipe.end_msg();

            ByteArray hash(20);
            hashPipe.read(hash.data(), 20);

            Botan::PK_Verifier *pkv = new Botan::PK_Verifier(dsaKey, "Raw");

            return pkv->verify_message(hash, m_signature);
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
