#include "BuildRecord.h"

#include <botan/auto_rng.h>
#include <botan/pipe.h>
#include <botan/pk_filts.h>
#include <botan/lookup.h>

#include "../exceptions/FormattingError.h"

namespace i2pcpp {
	BuildRecord::BuildRecord() :
		m_header({}),
		m_data({}) {}

	BuildRecord::BuildRecord(ByteArrayConstItr &begin, ByteArrayConstItr end)
	{
		if((end - begin) < 528)
			throw FormattingError();

		std::copy(begin, begin + 16, m_header.begin()), begin += 16;
		m_data.resize(512);
		std::copy(begin, begin + 512, m_data.begin()), begin += 512;
	}

	BuildRecord& BuildRecord::operator=(BuildRecord const &rec)
	{
		m_data.clear();
		m_data.resize(rec.m_data.size());

		std::copy(rec.m_header.cbegin(), rec.m_header.cend(), m_header.begin());
		std::copy(rec.m_data.cbegin(), rec.m_data.cend(), m_data.begin());

		return *this;
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
		Botan::Pipe hashPipe(new Botan::Hash_Filter("SHA-256"));
		hashPipe.start_msg();
		hashPipe.write(m_data.data(), m_data.size());
		hashPipe.end_msg();

		size_t size = hashPipe.remaining();
		ByteArray hash(size);
		hashPipe.read(hash.data(), size);

		Botan::AutoSeeded_RNG rng;
		Botan::DL_Group group("modp/ietf/2048");
		Botan::ElGamal_PublicKey elgKey(group, Botan::BigInt(encryptionKey.data(), encryptionKey.size()));
		Botan::Pipe encPipe(new Botan::PK_Encryptor_Filter(new Botan::PK_Encryptor_EME(elgKey, "Raw"), rng));

		encPipe.start_msg();
		encPipe.write(0xFF);
		encPipe.write(hash.data(), hash.size());
		encPipe.write(m_data.data(), m_data.size());
		encPipe.end_msg();

		size = encPipe.remaining();
		m_data.resize(size);
		encPipe.read(m_data.data(), size);
	}

	void BuildRecord::decrypt(std::shared_ptr<const Botan::ElGamal_PrivateKey> key)
	{
		Botan::DL_Group group("modp/ietf/2048");
		Botan::Pipe decPipe(new Botan::PK_Decryptor_Filter(new Botan::PK_Decryptor_EME(*key, "Raw")));

		decPipe.start_msg();
		decPipe.write(m_data.data(), m_data.size());
		decPipe.end_msg();

		size_t size = decPipe.remaining();
		if(size != 255)
			throw FormattingError();

		ByteArray record(size);
		decPipe.read(record.data(), size);

		auto dataItr = record.cbegin();

		dataItr++; // Non zero byte

		std::array<unsigned char, 32> givenHash;
		std::copy(dataItr, dataItr + 32, givenHash.begin()), dataItr += 32;

		ByteArray toHash(dataItr, record.cend());

		Botan::Pipe hashPipe(new Botan::Hash_Filter("SHA-256"));
		hashPipe.start_msg();
		hashPipe.write(toHash.data(), toHash.size());
		hashPipe.end_msg();

		std::array<unsigned char, 32> calcHash;
		hashPipe.read(calcHash.data(), 32);
		if(givenHash != calcHash)
			throw FormattingError();

		m_data = toHash;
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
		if(encryptedSize <= 16)
			throw FormattingError();

		cipherPipe.read(m_header.data(), 16);
		m_data.resize(encryptedSize - 16);
		cipherPipe.read(m_data.data(), encryptedSize - 16);
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
		if(decryptedSize <= 16)
			throw FormattingError();

		cipherPipe.read(m_header.data(), 16);
		m_data.resize(decryptedSize - 16);
		cipherPipe.read(m_data.data(), decryptedSize - 16);
	}

	void BuildRecord::setHeader(const std::array<unsigned char, 16> &header)
	{
		m_header = header;
	}

	const std::array<unsigned char, 16>& BuildRecord::getHeader() const
	{
		return m_header;
	}
}
