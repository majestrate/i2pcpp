#include "TunnelMessage.h"

#include <stdexcept>

#include <botan/pipe.h>
#include <botan/lookup.h>

namespace i2pcpp {
	TunnelMessage::TunnelMessage(StaticByteArray<1024, true> const &data)
	{
		std::copy(data.cbegin(), data.cbegin() + 16, m_iv.begin());
		std::copy(data.cbegin() + 16, data.cbegin() + 16 + 1008, m_data.begin());
	}

	void TunnelMessage::encrypt(Botan::SymmetricKey const &ivKey, Botan::SymmetricKey const &layerKey)
	{
		Botan::Pipe ivCipherPipe(get_cipher("AES-256/ECB/NoPadding", ivKey, Botan::ENCRYPTION));

		ivCipherPipe.process_msg(m_iv.data(), m_iv.size());

		if(ivCipherPipe.remaining() != m_iv.size())
			throw std::runtime_error("Did not get 16 bytes back");

		Botan::secure_vector<Botan::byte> v(16);
		ivCipherPipe.read(v.data(), v.size());

		Botan::InitializationVector iv(v);

		Botan::Pipe dataCipherPipe(get_cipher("AES-256/CBC/NoPadding", layerKey, iv, Botan::ENCRYPTION));

		dataCipherPipe.process_msg(m_data.data(), m_data.size());

		if(dataCipherPipe.remaining() != m_data.size())
			throw std::runtime_error("Did not get 1008 bytes back");

		dataCipherPipe.read(m_data.data(), m_data.size());

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
