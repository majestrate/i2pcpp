#include "Message.h"

#include <stdexcept>
#include <cmath>

#include <botan/pipe.h>
#include <botan/lookup.h>
#include <botan/auto_rng.h>

#include "../util/make_unique.h"

namespace i2pcpp {
	Message::Message(StaticByteArray<1024, true> const &data)
	{
		auto pos = data.cbegin();

		std::copy(pos, pos + 16, m_iv.begin()), pos += 16;
		std::copy(pos, pos + 1008, m_encrypted.begin());
	}

	Message::Message(std::list<FragmentPtr> &fragments) :
		m_fragments(std::move(fragments))
	{
		Botan::AutoSeeded_RNG rng;
		rng.randomize(m_iv.data(), m_iv.size());

		for(auto& f: m_fragments)
			m_payloadSize += f->size();

		if(m_payloadSize > 1003)
			throw std::runtime_error("total size of all fragments is too large for a tunnel message");

		calculateChecksum();
	}

	std::list<FragmentPtr> Message::parse() const
	{
		if(!verifyChecksum())
			throw std::runtime_error("invalid checksum in tunnel message");

		ByteArray data = m_encrypted;
		auto pos = data.cbegin() + 4;
		auto end = data.cend();

		pos = std::find(pos, end, 0x00);
		++pos; // 0x00 at the end

		std::list<FragmentPtr> fragments;
		while(pos != end)
			fragments.push_back(Fragment::parse(pos, end));

		return fragments;
	}

	StaticByteArray<1024, true> Message::getEncryptedData() const
	{
		StaticByteArray<1024, true> payload;
		std::copy(m_iv.cbegin(), m_iv.cend(), payload.begin());
		std::copy(m_encrypted.cbegin(), m_encrypted.cend(), payload.begin() + 16);

		return payload;
	}

	void Message::encrypt(Botan::SymmetricKey const &ivKey, Botan::SymmetricKey const &layerKey)
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

		dataCipherPipe.process_msg(m_encrypted.data(), m_encrypted.size());

		if(dataCipherPipe.remaining() != m_encrypted.size())
			throw std::runtime_error("Did not get 1008 bytes back");

		dataCipherPipe.read(m_encrypted.data(), m_encrypted.size());

		// Now encrypt our current IV with the IV key again and overwrite the current IV.
		Botan::Pipe ivCipherPipe2(get_cipher("AES-256/ECB/NoPadding", ivKey, Botan::ENCRYPTION));

		ivCipherPipe2.process_msg(v);

		if(ivCipherPipe2.remaining() != m_iv.size())
			throw std::runtime_error("Did not get 16 bytes back");

		ivCipherPipe2.read(m_iv.data(), m_iv.size());
	}

	void Message::compile()
	{
		m_encrypted[0] = m_checksum >> 24;
		m_encrypted[1] = m_checksum >> 16;
		m_encrypted[2] = m_checksum >> 8;
		m_encrypted[3] = m_checksum;

		// Pad the message
		Botan::AutoSeeded_RNG rng;
		auto padStart = m_encrypted.begin() + 4;
		auto padEnd = padStart + (1008 - 4 - 1 - m_payloadSize);
		rng.randomize((StaticByteArray<1008, true>::value_type *)padStart, padEnd - padStart);
		std::replace_if(padStart, padEnd, [](const ByteArray::value_type &x) { return x == 0x00; }, 0xff);
		*padEnd = 0x00; // last byte must be zero
		auto pos = padEnd + 1;

		// Copy the fragments
		for(auto& f: m_fragments) {
			ByteArray compiledFragment = f->compile();
			std::copy(compiledFragment.cbegin(), compiledFragment.cend(), pos);
			pos += compiledFragment.size();
		}
	}

	void Message::calculateChecksum()
	{
		Botan::Pipe hashPipe(new Botan::Hash_Filter("SHA-256"));
		hashPipe.start_msg();

		for(auto& f: m_fragments)
			hashPipe.write(f->compile());

		hashPipe.write(m_iv.data(), m_iv.size());

		hashPipe.end_msg();

		std::array<unsigned char, 4> checksum;
		hashPipe.read(checksum.data(), 4);
		m_checksum = (checksum[0] << 24) | (checksum[1] << 16) | (checksum[2] << 8) | (checksum[3]);
	}

	bool Message::verifyChecksum() const
	{
		auto pos = m_encrypted.cbegin();
		auto end = m_encrypted.cend();

		std::array<unsigned char, 4> givenChecksum;
		std::copy(pos, pos + 4, givenChecksum.begin());
		pos += 4; // checksum

		pos = std::find(pos, end, 0x00);
		++pos; // 0x00 at the end

		Botan::Pipe hashPipe(new Botan::Hash_Filter("SHA-256"));
		hashPipe.start_msg();

		hashPipe.write(m_encrypted.data() + (pos - m_encrypted.cbegin()), (end - pos));
		hashPipe.write(m_iv.data(), m_iv.size());

		hashPipe.end_msg();

		std::array<unsigned char, 4> calculatedChecksum;
		hashPipe.read(calculatedChecksum.data(), 4);

		return calculatedChecksum == givenChecksum;
	}
}
