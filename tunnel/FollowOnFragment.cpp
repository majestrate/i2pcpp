#include "FollowOnFragment.h"

namespace i2pcpp {
	FollowOnFragment::FollowOnFragment(uint32_t msgId, uint8_t n) :
		m_fragNum(n)
	{
		m_msgId = msgId;

		if(m_fragNum > 63)
			throw std::runtime_error("too many fragments!");
	}

	void FollowOnFragment::setLast(bool isLast)
	{
		m_isLast = isLast;
	}

	ByteArray FollowOnFragment::compile() const
	{
		ByteArray output;

		unsigned char flag = 0x80;

		flag |= m_fragNum << 1;
		flag |= (unsigned char)m_isLast;
		output.insert(output.end(), flag);

		output.insert(output.end(), m_msgId << 24);
		output.insert(output.end(), m_msgId << 16);
		output.insert(output.end(), m_msgId << 8);
		output.insert(output.end(), m_msgId);

		uint16_t size = (uint16_t)m_payload.size();
		output.insert(output.end(), size << 8);
		output.insert(output.end(), size);

		output.insert(output.end(), m_payload.cbegin(), m_payload.cend());

		return output;
	}

	uint8_t FollowOnFragment::headerSize() const
	{
		return 7;
	}
}
