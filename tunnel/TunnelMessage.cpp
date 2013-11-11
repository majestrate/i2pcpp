#include "TunnelMessage.h"

#include <stdexcept>
#include <cmath>

#include <botan/pipe.h>
#include <botan/lookup.h>
#include <botan/auto_rng.h>

namespace i2pcpp {
	TunnelMessage::TunnelMessage(StaticByteArray<1024, true> const &data)
	{
		std::copy(data.cbegin(), data.cbegin() + 16, m_iv.begin());
		std::copy(data.cbegin() + 16, data.cbegin() + 16 + 1008, m_data.begin());
	}

	TunnelMessage::TunnelMessage(std::list<ByteArrayPtr> const &fragments)
	{
		ByteArray data;

		auto r = fragments.crbegin();
		for(; r != fragments.crend(); ++r)
			data.insert(data.begin(), (*r)->cbegin(), (*r)->cend());

		data.insert(data.begin(), 0);
		data.insert(data.begin(), (1008 - data.size() - 4), 0xFF);

		Botan::Pipe hashPipe(new Botan::Hash_Filter("SHA-256"));
		hashPipe.start_msg();

		hashPipe.write(data);

		hashPipe.end_msg();

		ByteArray checksum(4);
		hashPipe.read(checksum.data(), 4);

		data.insert(data.begin(), checksum.cbegin(), checksum.cend());

		if(data.size() != 1008)
			throw std::runtime_error("tunnel message data not 1008 bytes");

		m_data = data;

		Botan::AutoSeeded_RNG rng;
		rng.randomize(m_iv.data(), m_iv.size());
	}

	std::list<ByteArrayPtr> TunnelMessage::fragment(I2NP::MessagePtr const &msg)
	{
		ByteArray data = msg->toBytes();

		uint8_t headerSize = 3;
		unsigned char flag = 0x00;
		if(data.size() > (1003 - headerSize)) {
			// Too big, can't do it all in one fragment
			headerSize += 4; // msgId
			flag |= (1 << 3); // set fragmented bit
		}

		std::list<ByteArrayPtr> fragments;
		fragments.push_back(std::make_shared<ByteArray>());
		ByteArrayPtr initial = fragments.back();

		initial->insert(initial->end(), flag);

		uint32_t msgId = msg->getMsgId();
		if(flag & (1 << 3)) { // if the fragment bit is set
			initial->insert(initial->end(), msgId >> 24);
			initial->insert(initial->end(), msgId >> 16);
			initial->insert(initial->end(), msgId >> 8);
			initial->insert(initial->end(), msgId);
		}

		// Extended options not implemented

		uint16_t size;
		if(flag & (1 << 3))
			size = (1003 - initial->size() - 2); // -2 to account for the size itself
		else
			size = data.size();

		initial->insert(initial->end(), size >> 8);
		initial->insert(initial->end(), size);

		if(flag & (1 << 3)) {
			uint16_t index = size;
			initial->insert(initial->end(), data.cbegin(), data.cbegin() + index);

			uint16_t numFragments = std::ceil((data.size() - index) / 996.0);
			if(numFragments > 63)
				throw std::runtime_error("message too big to fragment");

			unsigned char frag;
			// FOFs start at 1. The initial fragment is 0
			for(int i = 1; i <= numFragments; ++i) {
				fragments.push_back(std::make_shared<ByteArray>());
				ByteArrayPtr fof = fragments.back();;

				frag = (1 << 7);
				frag |= (i << 1);
				frag |= (i == numFragments);
				fof->insert(fof->end(), frag);

				fof->insert(fof->end(), msgId >> 24);
				fof->insert(fof->end(), msgId >> 16);
				fof->insert(fof->end(), msgId >> 8);
				fof->insert(fof->end(), msgId);

				if(i == numFragments) {
					uint8_t fofSize = (data.size() - index) % 996;
					fof->insert(fof->end(), fofSize >> 8);
					fof->insert(fof->end(), fofSize);
					fof->insert(fof->end(), data.cbegin() + index, data.cbegin() + index + fofSize);
				} else {
					fof->insert(fof->end(), 996 >> 8);
					fof->insert(fof->end(), (unsigned char)996);
					fof->insert(fof->end(), data.cbegin() + index, data.cbegin() + index + 996);
					index += 996;
				}
			}
		} else
			initial->insert(initial->end(), data.cbegin(), data.cend());

		return fragments;
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
