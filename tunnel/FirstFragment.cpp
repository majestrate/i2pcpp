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
			output.insert(output.end(), m_tunnelId >> 24);
			output.insert(output.end(), m_tunnelId >> 16);
			output.insert(output.end(), m_tunnelId >> 8);
			output.insert(output.end(), m_tunnelId);

			output.insert(output.end(), m_toHash.cbegin(), m_toHash.cend());
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

	void FirstFragment::setFragmented(bool f)
	{
		m_fragmented = f;
	}

	bool FirstFragment::isFragmented() const
	{
		return m_fragmented;
	}

	uint32_t FirstFragment::getTunnelId() const
	{
		return m_tunnelId;
	}

	const RouterHash& FirstFragment::getToHash() const
	{
		return m_toHash;
	}

	FirstFragment::DeliveryMode FirstFragment::getDeliveryMode() const
	{
		return m_mode;
	}

	FirstFragment FirstFragment::parse(ByteArrayConstItr &begin, ByteArrayConstItr end)
	{
		FirstFragment ff;

		unsigned char flag = *(begin++);
		ff.m_mode = (DeliveryMode)((flag >> 5) & 0x03);
		ff.m_fragmented = flag & (1 << 3);

		switch(ff.m_mode) {
			case DeliveryMode::TUNNEL:
				ff.m_tunnelId = (begin[0] << 24) | (begin[1] << 16) | (begin[2] << 8) | (begin[3]);
				begin += 4;
				std::copy(begin, begin + 32, ff.m_toHash.begin());
				begin += 32;

				break;

			case DeliveryMode::ROUTER:
				std::copy(begin, begin + 32, ff.m_toHash.begin());
				begin += 32;

				break;

			default:
				throw std::runtime_error("unhandled first fragment delivery mode");
		};

		if(ff.m_fragmented) {
			ff.m_msgId = (begin[0] << 24) | (begin[1] << 16) | (begin[2] << 8) | (begin[3]);
			begin += 4;
		}

		uint16_t size = (begin[0] << 8) | (begin[1]);
		begin += 2;
		if((end - begin) < size)
			throw std::runtime_error("malformed first fragment");

		ff.m_payload = ByteArray(begin, begin + size);
		begin += size;

		return ff;
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
}
