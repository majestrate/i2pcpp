#include "RouterHash.h"

#include "../exceptions/FormattingError.h"
#include "../util/Base64.h"

namespace i2pcpp {
	RouterHash::RouterHash() :
		std::array<unsigned char, 32>() 
	{
		fill(0);
	}

	RouterHash::RouterHash(ByteArray const &b)
	{
		std::copy(b.cbegin(), b.cbegin() + 32, begin());
	}

	RouterHash::RouterHash(std::string const &s)
	{
		ByteArray b = Base64::decode(s);
		if(b.size() != 32) throw FormattingError();
		std::copy(b.cbegin(), b.cbegin() + 32, begin());
	}

	RouterHash::RouterHash(std::array<unsigned char, 32> const &k)
	{
		std::copy(k.cbegin(), k.cbegin() + 32, begin());
	}

	RouterHash & RouterHash::operator=(std::array<unsigned char, 32> & arr)
	{
		std::copy(arr.cbegin(), arr.cbegin() + 32, begin());
	}

	RouterHash::operator ByteArray() const
	{
		return ByteArray(cbegin(), cend());
	}

	RouterHash::operator std::string() const
	{
		return std::string(Base64::encode(*this));
	}

	std::ostream& operator<<(std::ostream &s, RouterHash const &rh)
	{
		s << Base64::encode(rh);
		return s;
	}
}
