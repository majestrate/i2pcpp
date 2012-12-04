#ifndef I2PHMAC_H
#define I2PHMAC_H

#include <string>

#include <botan/mac.h>
#include <botan/hash.h>

using namespace std;
using namespace Botan;

namespace i2pcpp {
	class I2PHMAC : public MessageAuthenticationCode {
		public:
			void clear();
			string name() const;
			MessageAuthenticationCode* clone() const;
			size_t output_length() const { return hash->output_length(); }
			Key_Length_Specification key_spec() const
			{
				return Key_Length_Specification(0, 512);
			}

			I2PHMAC(HashFunction* hash);

			I2PHMAC(const I2PHMAC&) = delete;
			I2PHMAC& operator=(const I2PHMAC&) = delete;

			~I2PHMAC() { delete hash; }

		private:
			void add_data(const byte[], size_t);
			void final_result(byte[]);
			void key_schedule(const byte[], size_t);

			HashFunction* hash;
			secure_vector<byte> i_key, o_key;
	};
}

#endif
