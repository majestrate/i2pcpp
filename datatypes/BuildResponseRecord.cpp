#include "BuildResponseRecord.h"

#include <botan/auto_rng.h>
#include <botan/pipe.h>
#include <botan/lookup.h>

namespace i2pcpp {
	BuildResponseRecord::BuildResponseRecord(const BuildRecord &r) :
		BuildRecord(r) {}

	bool BuildResponseRecord::validate() const
	{
		ByteArray toHash(m_data.cbegin() + 32, m_data.cend());

		Botan::Pipe hashPipe(new Botan::Hash_Filter("SHA-256"));
		hashPipe.start_msg();
		hashPipe.write(toHash.data(), toHash.size());
		hashPipe.end_msg();

		size_t size = hashPipe.remaining();
		ByteArray calcHash(size);
		hashPipe.read(calcHash.data(), size);

		ByteArray givenHash(m_data.cbegin(), m_data.cbegin() + 32);

		return (givenHash == calcHash);
	}

	BuildResponseRecord::Reply BuildResponseRecord::getReply() const
	{
		return (BuildResponseRecord::Reply)m_data.back();
	}
}
