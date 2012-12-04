#include "I2PHMAC.h"

#include "xor_buf.h"

#include <iostream>
using namespace std;

namespace i2pcpp {
	void I2PHMAC::add_data(const byte input[], size_t length)
	{
		hash->update(input, length);
	}

	void I2PHMAC::final_result(byte mac[])
	{
		/* Because I2P doesn't follow the HMAC RFC,
		 * we need to make a change right here:
		 */
		byte tmp[32] = {0};
		hash->final(tmp);
		hash->update(o_key);
		hash->update(tmp, 32);
		hash->final(mac);
		hash->update(i_key);
	}

	void I2PHMAC::key_schedule(const byte key[], size_t length)
	{
		hash->clear();
		fill(i_key.begin(), i_key.end(), 0x36);
		fill(o_key.begin(), o_key.end(), 0x5C);

		if(length > hash->hash_block_size())
		{
			secure_vector<byte> hmac_key = hash->process(key, length);
			xor_buf(i_key, hmac_key, hmac_key.size());
			xor_buf(o_key, hmac_key, hmac_key.size());
		}
		else
		{
			xor_buf(i_key, key, length);
			xor_buf(o_key, key, length);
		}

		hash->update(i_key);
	}

	void I2PHMAC::clear()
	{
		hash->clear();
		zeroise(i_key);
		zeroise(o_key);
	}

	string I2PHMAC::name() const
	{
		return "I2PHMAC(" + hash->name() + ")";
	}

	MessageAuthenticationCode* I2PHMAC::clone() const
	{
		return new I2PHMAC(hash->clone());
	}

	I2PHMAC::I2PHMAC(HashFunction* hash_in) : hash(hash_in)
	{
		if(hash->hash_block_size() == 0)
			throw Invalid_Argument("HMAC cannot be used with " + hash->name());

		if(hash->name() != "MD5")
			throw Invalid_Argument("This HMAC is specially adapted for use with MD5 only.");

		i_key.resize(hash->hash_block_size());
		o_key.resize(hash->hash_block_size());
	}
}
