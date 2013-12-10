#include "FirstFragment.h"

namespace i2pcpp {
	ByteArray FirstFragment::compile() const
	{
		ByteArray output;

		unsigned char flag = 0x00;

		flag |= (unsigned char)m_mode << 5;
		flag |= (unsigned char)m_fragmented << 3;
		output.insert(output.end(), flag);

		if(m_mode == DeliveryMode::TUNNEL) {
			// TODO tunnel ID and router hash
		}

		if(m_fragmented) {
			output.insert(output.end(), m_msgId >> 24);
			output.insert(output.end(), m_msgId >> 16);
			output.insert(output.end(), m_msgId >> 8);
			output.insert(output.end(), m_msgId);
		}

		uint16_t size = (uint16_t)m_payload.size();
		output.insert(output.end(), size >> 8);
		output.insert(output.end(), size);

		output.insert(output.end(), m_payload.cbegin(), m_payload.cend());

		return output;
	}

	bool FirstFragment::mustFragment(uint16_t desiredSize, uint16_t max) const
	{
		return (headerSize() + desiredSize > max);
	}

	uint8_t FirstFragment::headerSize() const
	{
		switch(m_mode) {
			case DeliveryMode::LOCAL:
				if(!m_fragmented)
					return 3;
				else
					return 7;

			case DeliveryMode::TUNNEL:
				if(!m_fragmented)
					return 39;
				else
					return 43;

			default:
				throw std::logic_error("Unimplemented FirstFragment delivery mode");
		}
	}

	void FirstFragment::setFragmented(bool f)
	{
		m_fragmented = f;
	}
}
