#include "BuildRequestRecord.h"

#include <botan/auto_rng.h>
#include <botan/pipe.h>
#include <botan/pk_filts.h>
#include <botan/elgamal.h>
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
		b.insert(b.end(), m_replyIV.cbegin(), m_replyIV.cend());

		b.insert(b.end(), m_flags);

		b.insert(b.end(), m_requestTime >> 24);
		b.insert(b.end(), m_requestTime >> 16);
		b.insert(b.end(), m_requestTime >> 8);
		b.insert(b.end(), m_requestTime);

		b.insert(b.end(), m_nextMsgId >> 24);
		b.insert(b.end(), m_nextMsgId >> 16);
		b.insert(b.end(), m_nextMsgId >> 8);
		b.insert(b.end(), m_nextMsgId);

		Botan::Pipe hashPipe(new Botan::Hash_Filter("SHA-256"));
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
		Botan::Pipe encPipe(new Botan::PK_Encryptor_Filter(new Botan::PK_Encryptor_EME(elgKey, "Raw"), rng));

		encPipe.start_msg();
		encPipe.write(b.data(), b.size());
		encPipe.end_msg();

		size = encPipe.remaining();
		m_bytes.resize(16 + size);
		encPipe.read(m_bytes.data() + 16, size);

		copy(m_localIdentity.cbegin(), m_localIdentity.cbegin() + 16, m_bytes.begin());
	}
}
