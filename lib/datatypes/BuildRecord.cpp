/**
 * @file BuildRecord.cpp
 * @brief Implements BuildRecord.h, specifically i2pcpp::BuildRecord.
 */
#include <i2pcpp/datatypes/BuildRecord.h>

#include <botan/auto_rng.h>
#include <botan/pipe.h>
#include <botan/pk_filts.h>
#include <botan/lookup.h>

namespace i2pcpp {
    BuildRecord::BuildRecord(ByteArrayConstItr &begin, ByteArrayConstItr end)
    {
        if((end - begin) < 528)
            throw std::runtime_error("malformed BuildRecord");

        std::copy(begin, begin + 16, m_header.begin()), begin += 16;
        std::copy(begin, begin + 512, m_data.begin()), begin += 512;
    }

    ByteArray BuildRecord::serialize() const
    {
        ByteArray b(m_header.size() + m_data.size());
        std::copy(m_header.cbegin(), m_header.cend(), b.begin());
        std::copy(m_data.cbegin(), m_data.cend(), b.begin() + m_header.size());

        return b;
    }

    void BuildRecord::encrypt(ByteArray const &encryptionKey)
    {
        // First hash the data
        Botan::Pipe hashPipe(new Botan::Hash_Filter("SHA-256"));
        hashPipe.start_msg();
        hashPipe.write(m_data.data(), 222);
        hashPipe.end_msg();

        std::array<unsigned char, 32> hash;
        hashPipe.read(hash.data(), 32);

        // Create a vector containing the bytes to encrypt
        Botan::secure_vector<Botan::byte> toEncrypt = { 0xFF }; // 0xFF is the non-zero byte
        toEncrypt.insert(toEncrypt.end(), hash.cbegin(), hash.cend());
        toEncrypt.insert(toEncrypt.end(), m_data.cbegin(), m_data.cbegin() + 222);

        // Perform the encryption
        Botan::AutoSeeded_RNG rng;
        Botan::DL_Group group("modp/ietf/2048");
        Botan::ElGamal_PublicKey elgKey(group, Botan::BigInt(encryptionKey.data(), encryptionKey.size()));
        Botan::PK_Encryptor *pke = new Botan::PK_Encryptor_EME(elgKey, "Raw");
        m_data = toStaticByteArray<512>(pke->encrypt(toEncrypt, rng));
    }

    void BuildRecord::decrypt(std::shared_ptr<const Botan::ElGamal_PrivateKey> key)
    {
        // Decrypt
        Botan::DL_Group group("modp/ietf/2048");
        Botan::PK_Decryptor *pkd = new Botan::PK_Decryptor_EME(*key, "Raw");
        Botan::secure_vector<Botan::byte> decrypted = pkd->decrypt(m_data.data(), 512);

        // Parse
        auto dataItr = decrypted.cbegin();

        ++dataItr; // Non zero byte

        std::array<unsigned char, 32> givenHash;
        std::copy(dataItr, dataItr + 32, givenHash.begin());
        dataItr += 32;

        std::move(dataItr, dataItr + 222, m_data.begin());

        // Hash
        Botan::Pipe hashPipe(new Botan::Hash_Filter("SHA-256"));
        hashPipe.start_msg();
        hashPipe.write(m_data.cbegin(), 222);
        hashPipe.end_msg();

        std::array<unsigned char, 32> calcHash;
        hashPipe.read(calcHash.data(), 32);

        // Verify that the hashes are the same
        if(calcHash != givenHash)
            throw std::runtime_error("hash mismatch in BuildRecord");
    }

    void BuildRecord::encrypt(StaticByteArray<16> const &iv, SessionKey const &key)
    {
        Botan::InitializationVector biv(iv.data(), 16);
        Botan::SymmetricKey bkey(key.data(), key.size());
        Botan::Pipe cipherPipe(get_cipher("AES-256/CBC/NoPadding", bkey, biv, Botan::ENCRYPTION));

        cipherPipe.start_msg();
        cipherPipe.write(m_header.data(), m_header.size());
        cipherPipe.write(m_data.data(), m_data.size());
        cipherPipe.end_msg();

        size_t encryptedSize = cipherPipe.remaining();
        if(encryptedSize != (16 + 512))
            throw std::runtime_error("error AES encrypting BuildRecord");

        cipherPipe.read(m_header.data(), 16);
        cipherPipe.read(m_data.data(), 512);
    }

    void BuildRecord::decrypt(StaticByteArray<16> const &iv, SessionKey const &key)
    {
        Botan::InitializationVector biv(iv.data(), 16);
        Botan::SymmetricKey bkey(key.data(), key.size());
        Botan::Pipe cipherPipe(get_cipher("AES-256/CBC/NoPadding", bkey, biv, Botan::DECRYPTION));

        cipherPipe.start_msg();
        cipherPipe.write(m_header.data(), m_header.size());
        cipherPipe.write(m_data.data(), m_data.size());
        cipherPipe.end_msg();

        size_t decryptedSize = cipherPipe.remaining();
        if(decryptedSize != (16 + 512))
            throw std::runtime_error("error AES decrypting BuildRecord");

        cipherPipe.read(m_header.data(), 16);
        cipherPipe.read(m_data.data(), 512);
    }

    void BuildRecord::setHeader(StaticByteArray<16> const &header)
    {
        m_header = header;
    }

    const StaticByteArray<16>& BuildRecord::getHeader() const
    {
        return m_header;
    }
}
