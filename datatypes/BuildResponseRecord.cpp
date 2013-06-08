#include "BuildResponseRecord.h"

#include <stdexcept>

#include <botan/auto_rng.h>
#include <botan/pipe.h>
#include <botan/lookup.h>

#include "../exceptions/FormattingError.h"

namespace i2pcpp {
	BuildResponseRecord::BuildResponseRecord(const BuildRecord &r) :
		BuildRecord(r) {}

	BuildResponseRecord::BuildResponseRecord(Reply r) :
		m_reply(r) {}

	void BuildResponseRecord::parse()
	{
		Botan::Pipe hashPipe(new Botan::Hash_Filter("SHA-256"));
		hashPipe.start_msg();
		hashPipe.write(m_data.data() + 32, m_data.size() - 32);
		hashPipe.end_msg();

		size_t size = hashPipe.remaining();
		ByteArray calcHash(size);
		hashPipe.read(calcHash.data(), size);

		ByteArray givenHash(m_data.cbegin(), m_data.cbegin() + 32);

		if(givenHash != calcHash)
			throw FormattingError();

		m_reply = (Reply)m_data.back();
	}

	void BuildResponseRecord::compile()
	{
		Botan::AutoSeeded_RNG rng;

		m_data.resize(528);
		rng.randomize(m_data.data() + 32, 528 - 32);
		m_data[527] = m_reply;

		Botan::Pipe hashPipe(new Botan::Hash_Filter("SHA-256"));
		hashPipe.start_msg();
		hashPipe.write(m_data.data() + 32, m_data.size() - 32);
		hashPipe.end_msg();

		size_t size = hashPipe.remaining();
		if(size != 32)
			throw std::runtime_error("hash is not 32 bytes");

		hashPipe.read(m_data.data(), size);
	}

	BuildResponseRecord::Reply BuildResponseRecord::getReply() const
	{
		return m_reply;
	}
}
