/**
 * @file RouterInfo.cpp
 * @brief Implements RouterInfo.h.
 */
#include "RouterInfo.h"

#include <botan/pipe.h>
#include <botan/pubkey.h>
#include <botan/pk_filts.h>
#include <botan/auto_rng.h>

#include "../util/I2PDH.h"
#include "../exceptions/FormattingError.h"

namespace i2pcpp {
    RouterInfo::RouterInfo(RouterIdentity const &identity, Date const &published, Mapping const &options, ByteArray const &signature) :
        m_identity(identity),
        m_published(published),
        m_options(options),
        m_signature(signature) {}

    RouterInfo::RouterInfo(ByteArrayConstItr &begin, ByteArrayConstItr end) :
        m_signature(40),
        m_identity(begin, end)
    {
        m_published = Date(begin, end);

        unsigned char size = *(begin++);
        for(int i = 0; i < size; i++)
            m_addresses.push_back(RouterAddress(begin, end));

        begin++; // unused peer_size

        m_options = Mapping(begin, end);

        if((end - begin) < 40) throw FormattingError();
        copy(end - 40, end, m_signature.begin());
    }

    ByteArray RouterInfo::serialize() const
    {
        ByteArray signedBytes = getSignedBytes();
        signedBytes.insert(signedBytes.end(), m_signature.begin(), m_signature.end());

        return signedBytes;
    }

    void RouterInfo::addAddress(RouterAddress const &address)
    {
        m_addresses.push_back(address);
    }

    bool RouterInfo::verifySignature() const
    {
        Botan::Pipe hashPipe(new Botan::Hash_Filter("SHA-1"));
        hashPipe.start_msg();
        hashPipe.write(getSignedBytes());
        hashPipe.end_msg();

        ByteArray hash(20);
        hashPipe.read(hash.data(), 20);

        const ByteArray& dsaKeyBytes = m_identity.getSigningKey();
        Botan::DSA_PublicKey dsaKey(DH::getGroup(), Botan::BigInt(dsaKeyBytes.data(), dsaKeyBytes.size()));
        Botan::PK_Verifier *pkv = new Botan::PK_Verifier(dsaKey, "Raw");

        return pkv->verify_message(hash, m_signature);
    }

    void RouterInfo::sign(std::shared_ptr<const Botan::DSA_PrivateKey> signingKey)
    {
        Botan::Pipe hashPipe(new Botan::Hash_Filter("SHA-1"));
        hashPipe.start_msg();
        hashPipe.write(getSignedBytes());
        hashPipe.end_msg();

        ByteArray hash(20);
        hashPipe.read(hash.data(), 20);

        Botan::AutoSeeded_RNG rng;
        Botan::PK_Signer *pks = new Botan::PK_Signer(*signingKey, "Raw");
        m_signature = pks->sign_message(hash, rng);
    }

    const RouterAddress& RouterInfo::getAddress(const int index) const
    {
        return m_addresses[index];
    }

    const RouterIdentity& RouterInfo::getIdentity() const
    {
        return m_identity;
    }

    const Date& RouterInfo::getPublished() const
    {
        return m_published;
    }

    const Mapping& RouterInfo::getOptions() const
    {
        return m_options;
    }

    const ByteArray& RouterInfo::getSignature() const
    {
        return m_signature;
    }

    const std::vector<RouterAddress>::const_iterator RouterInfo::begin() const
    {
        return m_addresses.cbegin();
    }

    const std::vector<RouterAddress>::const_iterator RouterInfo::end() const
    {
        return m_addresses.cend();
    }

    ByteArray RouterInfo::getSignedBytes() const
    {
        ByteArray b;

        ByteArray idBytes = m_identity.serialize();
        ByteArray pubBytes = m_published.serialize();
        ByteArray optBytes = m_options.serialize();

        b.insert(b.end(), idBytes.cbegin(), idBytes.cend());

        b.insert(b.end(), pubBytes.cbegin(), pubBytes.cend());

        b.insert(b.end(), m_addresses.size());
        for(auto& a: m_addresses) {
            const ByteArray& addr = a.serialize();
            b.insert(b.end(), addr.cbegin(), addr.cend());
        }

        b.insert(b.end(), 0x00); // Unused peer_size

        b.insert(b.end(), optBytes.cbegin(), optBytes.cend());

        return b;
    }

}
