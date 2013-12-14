#include "TunnelMessage.h"

#include <stdexcept>
#include <cmath>

#include <botan/pipe.h>
#include <botan/lookup.h>
#include <botan/auto_rng.h>

#include "../util/make_unique.h"

namespace i2pcpp {
	TunnelMessage::TunnelMessage(StaticByteArray<1024, true> const &data)
	{
		std::copy(data.cbegin(), data.cbegin() + 16, m_iv.begin());
		std::copy(data.cbegin() + 16, data.cbegin() + 16 + 1008, m_data.begin());
	}

	TunnelMessage::TunnelMessage(std::list<ByteArrayPtr> const &fragments)
	{
		Botan::AutoSeeded_RNG rng;
		rng.randomize(m_iv.data(), m_iv.size());

		ByteArray data;
		auto r = fragments.crbegin();
		for(; r != fragments.crend(); ++r)
			data.insert(data.begin(), (*r)->cbegin(), (*r)->cend());

		Botan::Pipe hashPipe(new Botan::Hash_Filter("SHA-256"));
		hashPipe.start_msg();

		hashPipe.write(data);
		hashPipe.write(m_iv.data(), m_iv.size());

		hashPipe.end_msg();

		data.insert(data.begin(), 0);
		data.insert(data.begin(), (1008 - data.size() - 4), 0xFF);

		ByteArray checksum(4);
		hashPipe.read(checksum.data(), 4);

		data.insert(data.begin(), checksum.cbegin(), checksum.cend());

		if(data.size() != 1008)
			throw std::runtime_error("tunnel message data not 1008 bytes");

		m_data = data;
	}

	void TunnelMessage::encrypt(Botan::SymmetricKey const &ivKey, Botan::SymmetricKey const &layerKey)
	{
		// Using the IV key from the BRR, encrypt the IV contained in the tunnel message.
		Botan::Pipe ivCipherPipe(get_cipher("AES-256/ECB/NoPadding", ivKey, Botan::ENCRYPTION));

		ivCipherPipe.process_msg(m_iv.data(), m_iv.size());

		if(ivCipherPipe.remaining() != m_iv.size())
			throw std::runtime_error("Did not get 16 bytes back");

		Botan::secure_vector<Botan::byte> v(16);
		ivCipherPipe.read(v.data(), v.size());

		Botan::InitializationVector iv(v);

		/* We now have an encrypted IV that is used in conjunction with the layer key
		 * to encrypt the actual content of the message.
		 */
		Botan::Pipe dataCipherPipe(get_cipher("AES-256/CBC/NoPadding", layerKey, iv, Botan::ENCRYPTION));

		dataCipherPipe.process_msg(m_data.data(), m_data.size());

		if(dataCipherPipe.remaining() != m_data.size())
			throw std::runtime_error("Did not get 1008 bytes back");

		dataCipherPipe.read(m_data.data(), m_data.size());

		// Now encrypt our current IV with the IV key again and overwrite the current IV.
		Botan::Pipe ivCipherPipe2(get_cipher("AES-256/ECB/NoPadding", ivKey, Botan::ENCRYPTION));

		ivCipherPipe2.process_msg(v);

		if(ivCipherPipe2.remaining() != m_iv.size())
			throw std::runtime_error("Did not get 16 bytes back");

		ivCipherPipe2.read(m_iv.data(), m_iv.size());
	}

	ByteArray TunnelMessage::compile() const
	{
		ByteArray b(m_iv.size() + m_data.size());

		std::copy(m_iv.cbegin(), m_iv.cend(), b.begin());
		std::copy(m_data.cbegin(), m_data.cend(), b.begin() + m_iv.size());

		return b;
	}
}
