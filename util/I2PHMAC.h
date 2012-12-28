#ifndef I2PHMAC_H
#define I2PHMAC_H

#include <string>

#include <botan/mac.h>
#include <botan/hash.h>

namespace i2pcpp {
	class I2PHMAC : public Botan::MessageAuthenticationCode {
		public:
			void clear();
			std::string name() const;
			Botan::MessageAuthenticationCode* clone() const;
			size_t output_length() const { return hash->output_length(); }
			Botan::Key_Length_Specification key_spec() const
			{
				return Botan::Key_Length_Specification(0, 512);
			}

			I2PHMAC(Botan::HashFunction* hash);

			I2PHMAC(const I2PHMAC&) = delete;
			I2PHMAC& operator=(const I2PHMAC&) = delete;

			~I2PHMAC() { delete hash; }

		private:
			void add_data(const Botan::byte[], size_t);
			void final_result(Botan::byte[]);
			void key_schedule(const Botan::byte[], size_t);

			Botan::HashFunction* hash;
			Botan::secure_vector<Botan::byte> i_key, o_key;
	};
}

#endif
