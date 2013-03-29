#include "BuildResponseRecord.h"

#include <botan/pipe.h>
#include <botan/pk_filts.h>
#include <botan/lookup.h>

namespace i2pcpp {
	BuildResponseRecord::BuildResponseRecord(ByteArrayConstItr &begin, ByteArrayConstItr end)
	{
		m_data.resize(528);
		std::copy(begin, begin + 528, m_data.begin()), begin += 528;
	}

	void BuildResponseRecord::decrypt(SessionKey const &iv, SessionKey const &key)
	{
		Botan::InitializationVector biv(iv.data(), 16);
		Botan::SymmetricKey bkey(key.data(), key.size());
		Botan::Pipe cipherPipe(get_cipher("AES-256/CBC/NoPadding", bkey, biv, Botan::DECRYPTION));

		cipherPipe.process_msg(m_data.data(), m_data.size());

		size_t decryptedSize = cipherPipe.remaining();
		ByteArray decryptedBytes(decryptedSize);

		cipherPipe.read(decryptedBytes.data(), decryptedSize);
		m_data = decryptedBytes;
}

	BuildResponseRecord::Reply BuildResponseRecord::getReply() const
	{
		return (BuildResponseRecord::Reply)m_data.back();
	}
}
