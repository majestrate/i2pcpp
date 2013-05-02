#include "BuildRequestRecord.h"

#include <botan/auto_rng.h>
#include <botan/pipe.h>
#include <botan/pk_filts.h>
#include <botan/lookup.h>

namespace i2pcpp {
	BuildRequestRecord::BuildRequestRecord(uint32_t tunnelId, RouterHash const &localIdentity, uint32_t nextTunnelId, RouterHash const &nextIdentity, SessionKey const &tunnelLayerKey, SessionKey const &tunnelIVKey, SessionKey const &replyKey, SessionKey const &replyIV, HopType type, uint32_t requestTime, uint32_t nextMsgId) :
		m_tunnelId(tunnelId),
		m_localIdentity(localIdentity),
		m_nextTunnelId(nextTunnelId),
		m_nextIdentity(nextIdentity),
		m_tunnelLayerKey(tunnelLayerKey),
		m_tunnelIVKey(tunnelIVKey),
		m_replyKey(replyKey),
		m_replyIV(replyIV),
		m_flags(0),
		m_requestTime(requestTime),
		m_nextMsgId(nextMsgId)
	{
		m_flags |= type;
	}

	BuildRequestRecord::BuildRequestRecord(ByteArrayConstItr &begin, ByteArrayConstItr end)
	{
		std::copy(begin, begin + 16, m_header.begin()), begin += 16;
		m_bytes.resize(512);
		std::copy(begin, begin + 512, m_bytes.begin()), begin += 512;
	}

	ByteArray BuildRequestRecord::serialize() const
	{
		return m_bytes;
	}

	void BuildRequestRecord::encrypt(ByteArray const &encryptionKey)
	{
		ByteArray b;

		b.insert(b.end(), m_tunnelId >> 24);
		b.insert(b.end(), m_tunnelId >> 16);
		b.insert(b.end(), m_tunnelId >> 8);
		b.insert(b.end(), m_tunnelId);

		b.insert(b.end(), m_localIdentity.cbegin(), m_localIdentity.cend());

		b.insert(b.end(), m_nextTunnelId >> 24);
		b.insert(b.end(), m_nextTunnelId >> 16);
		b.insert(b.end(), m_nextTunnelId >> 8);
		b.insert(b.end(), m_nextTunnelId);

		b.insert(b.end(), m_nextIdentity.cbegin(), m_nextIdentity.cend());

		b.insert(b.end(), m_tunnelLayerKey.cbegin(), m_tunnelLayerKey.cend());
		b.insert(b.end(), m_tunnelIVKey.cbegin(), m_tunnelIVKey.cend());
		b.insert(b.end(), m_replyKey.cbegin(), m_replyKey.cend());
		b.insert(b.end(), m_replyIV.cbegin(), m_replyIV.cbegin() + 16);

		b.insert(b.end(), m_flags);

		b.insert(b.end(), m_requestTime >> 24);
		b.insert(b.end(), m_requestTime >> 16);
		b.insert(b.end(), m_requestTime >> 8);
		b.insert(b.end(), m_requestTime);

		b.insert(b.end(), m_nextMsgId >> 24);
		b.insert(b.end(), m_nextMsgId >> 16);
		b.insert(b.end(), m_nextMsgId >> 8);
		b.insert(b.end(), m_nextMsgId);

		b.insert(b.end(), 29, 0x00); // TODO Random padding

		Botan::Pipe hashPipe(new Botan::Hash_Filter("SHA-256")); // memory leak?
		hashPipe.start_msg();
		hashPipe.write(b.data(), b.size());
		hashPipe.end_msg();

		size_t size = hashPipe.remaining();
		ByteArray hash(size);
		hashPipe.read(hash.data(), size);

		b.insert(b.begin(), hash.cbegin(), hash.cend());
		b.insert(b.begin(), 0xFF); // TODO Pad?

		Botan::AutoSeeded_RNG rng;
		Botan::DL_Group group("modp/ietf/2048");
		Botan::ElGamal_PublicKey elgKey(group, Botan::BigInt(encryptionKey.data(), encryptionKey.size()));
		// memory leak?
		Botan::Pipe encPipe(new Botan::PK_Encryptor_Filter(new Botan::PK_Encryptor_EME(elgKey, "Raw"), rng));

		encPipe.start_msg();
		encPipe.write(b.data(), b.size());
		encPipe.end_msg();

		size = encPipe.remaining();
		m_bytes.resize(size + 16);
		encPipe.read(m_bytes.data() + 16, size);
		std::copy(m_localIdentity.cbegin(), m_localIdentity.cbegin() + 16, m_bytes.begin());
	}

	void BuildRequestRecord::decrypt(Botan::ElGamal_PrivateKey const *key)
	{
		Botan::DL_Group group("modp/ietf/2048");
		// memory leak ?
		Botan::Pipe decPipe(new Botan::PK_Decryptor_Filter(new Botan::PK_Decryptor_EME(*key, "Raw")));

		decPipe.start_msg();
		decPipe.write(m_bytes.data(), m_bytes.size());
		decPipe.end_msg();

		size_t size = decPipe.remaining();
		m_bytes.resize(size);
		decPipe.read(m_bytes.data(), size);

		auto dataItr = m_bytes.cbegin();

		dataItr++; // Non zero byte
		ByteArray hash(dataItr, dataItr + 32);
		dataItr += 32;

		m_tunnelId = (*(dataItr++) << 24) | (*(dataItr++) << 16) | (*(dataItr++) << 8) | *(dataItr++);

		copy(dataItr, dataItr + 32, m_localIdentity.begin()), dataItr += 32;

		m_nextTunnelId = (*(dataItr++) << 24) | (*(dataItr++) << 16) | (*(dataItr++) << 8) | *(dataItr++);

		copy(dataItr, dataItr + 32, m_nextIdentity.begin()), dataItr += 32;

		copy(dataItr, dataItr + 32, m_tunnelLayerKey.begin()), dataItr += 32;
		copy(dataItr, dataItr + 32, m_tunnelIVKey.begin()), dataItr += 32;
		copy(dataItr, dataItr + 32, m_replyKey.begin()), dataItr += 32;
		copy(dataItr, dataItr + 16, m_replyIV.begin()), dataItr += 16;

		m_flags = *(dataItr)++;

		m_requestTime = (*(dataItr++) << 24) | (*(dataItr++) << 16) | (*(dataItr++) << 8) | *(dataItr++);
		m_nextMsgId = (*(dataItr++) << 24) | (*(dataItr++) << 16) | (*(dataItr++) << 8) | *(dataItr++);
	}

	void BuildRequestRecord::decrypt(SessionKey const &iv, SessionKey const &key)
	{
		Botan::InitializationVector biv(iv.data(), 16);
		Botan::SymmetricKey bkey(key.data(), key.size());
		Botan::Pipe cipherPipe(get_cipher("AES-256/CBC/NoPadding", bkey, biv, Botan::DECRYPTION));

		ByteArray record = serialize();
		cipherPipe.process_msg(record.data(), record.size());

		size_t decryptedSize = cipherPipe.remaining();
		ByteArray decryptedBytes(decryptedSize);

		cipherPipe.read(decryptedBytes.data(), decryptedSize);
		m_bytes = decryptedBytes;
	}

	const std::array<unsigned char, 16>& BuildRequestRecord::getHeader() const
	{
		return m_header;
	}

	const RouterHash& BuildRequestRecord::getLocalIdentity() const
	{
		return m_localIdentity;
	}

	const SessionKey& BuildRequestRecord::getReplyIV() const
	{
		return m_replyIV;
	}

	const SessionKey& BuildRequestRecord::getReplyKey() const
	{
		return m_replyKey;
	}

	uint32_t BuildRequestRecord::getTunnelId() const
	{
		return m_tunnelId;
	}

	BuildRequestRecord::operator BuildResponseRecord() const
	{
		ByteArray bytes = m_bytes;
		bytes.insert(bytes.begin(), m_header.cbegin(), m_header.cend());

		auto begin = bytes.cbegin();

		return BuildResponseRecord(begin, bytes.cend());
	}
}
