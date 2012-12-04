#include "Packet.h"

#include <botan/auto_rng.h>
#include <botan/pipe.h>
#include <botan/lookup.h>
#include <botan/md5.h>

#include "../util/I2PHMAC.h"
#include "../util/Base64.h"

using namespace Botan;

namespace i2pcpp {
	namespace SSU {
		Packet::Packet(Endpoint const &endpoint, const unsigned char *data, size_t length) : m_endpoint(endpoint)
		{
			m_data.resize(length);
			copy(data, data + length, m_data.begin());
		}

		void Packet::decrypt(SessionKey const &sk)
		{
			const unsigned char *packetIV = m_data.data() + 16;
			const unsigned char *packet = m_data.data() + 32;
			const unsigned int packetSize = m_data.size() - 32;

			InitializationVector iv(packetIV, 16);
			SymmetricKey key(sk.data(), sk.size());
			Pipe cipherPipe(get_cipher("AES-256/CBC/NoPadding", key, iv, DECRYPTION));

			cipherPipe.process_msg(packet, packetSize);

			size_t decryptedSize = cipherPipe.remaining();
			ByteArray plaintext(decryptedSize);

			cipherPipe.read(plaintext.data(), decryptedSize);
			m_data = plaintext;
		}

		bool Packet::verify(SessionKey const &mk)
		{
			unsigned int packetSize = m_data.size() - 32;

			SymmetricKey key(mk.data(), mk.size());
			Pipe hmacPipe(new MAC_Filter(new I2PHMAC(new MD5()), key));

			hmacPipe.start_msg();
			hmacPipe.write(m_data.data() + 32, packetSize);
			hmacPipe.write(m_data.data() + 16, 16);
			hmacPipe.write((packetSize >> 8) ^ (PROTOCOL_VERSION >> 8));
			hmacPipe.write(packetSize ^ (PROTOCOL_VERSION));
			hmacPipe.end_msg();

			ByteArray calculatedMAC(16);
			hmacPipe.read(calculatedMAC.data(), 16);

			return calculatedMAC == ByteArray(m_data.begin(), m_data.begin() + 16);
		}

		void Packet::encrypt(SessionKey const &sk, SessionKey const &mk)
		{
			AutoSeeded_RNG rng;
			InitializationVector iv(rng, 16);

			SymmetricKey sessionKey(sk.data(), sk.size());
			SymmetricKey macKey(mk.data(), mk.size());
			Pipe cipherPipe(get_cipher("AES-256/CBC/OneAndZeros", sessionKey, iv, ENCRYPTION));
			Pipe hmacPipe(new MAC_Filter(new I2PHMAC(new MD5()), macKey));

			cipherPipe.process_msg(m_data.data(), m_data.size());

			size_t encryptedSize = cipherPipe.remaining();
			m_data.resize(encryptedSize + 32);

			cipherPipe.read(m_data.data() + 32, encryptedSize);
			if(cipherPipe.remaining())
				throw runtime_error("Bytes still remaining in the cipherPipe!?");

			copy(iv.begin(), iv.end(), m_data.begin() + 16);

			hmacPipe.start_msg();
			hmacPipe.write(m_data.data() + 32, encryptedSize);
			hmacPipe.write(iv.bits_of());
			hmacPipe.write((encryptedSize >> 8) ^ (PROTOCOL_VERSION >> 8));
			hmacPipe.write(encryptedSize ^ (PROTOCOL_VERSION));
			hmacPipe.end_msg();

			hmacPipe.read(m_data.data(), 16);
			if(hmacPipe.remaining())
				throw runtime_error("Bytes still remaining in the hmacPipe!?");
		}
	}
}
