#include "TunnelFragment.h"

#include <botan/auto_rng.h>

#include "../util/make_unique.h"

#include "FirstFragment.h"
#include "FollowOnFragment.h"

namespace i2pcpp {
	void TunnelFragment::setMsgId(uint32_t id)
	{
		m_msgId = id;
	}

	ByteArrayConstItr TunnelFragment::setPayload(ByteArrayConstItr begin, ByteArrayConstItr end, uint16_t max)
	{
		uint8_t headerLen = headerSize();
		auto payloadLen = std::distance(begin, end);

		if(headerLen + payloadLen > max) {
			payloadLen = max - headerLen;
			end = begin + payloadLen;
		}

		m_payload.clear();
		m_payload.resize(payloadLen);
		std::copy(begin, end, m_payload.begin());

		return end;
	}

	std::vector<TunnelFragmentPtr> TunnelFragment::fragmentMessage(ByteArray const &data)
	{
		constexpr uint16_t maxSize = 1003;

		std::vector<TunnelFragmentPtr> fragments;

		std::unique_ptr<FirstFragment> first = std::make_unique<FirstFragment>();
		if(first->mustFragment(data.size(), maxSize)) {
			first->setFragmented(true);

			uint32_t msgId = 0;
			Botan::AutoSeeded_RNG rng;
			rng.randomize((unsigned char *)&msgId, sizeof(msgId));
			first->setMsgId(msgId);

			auto pos = first->setPayload(data.cbegin(), data.cend(), maxSize);
			fragments.push_back(std::move(first));

			uint8_t fragNum = 1;
			while(pos != data.cend()) {
				std::unique_ptr<FollowOnFragment> followup = std::make_unique<FollowOnFragment>(msgId, fragNum++);
				pos = followup->setPayload(pos, data.cend(), maxSize);

				if(pos == data.cend())
					followup->setLast(true);

				fragments.push_back(std::move(followup));
			}
		} else {
			first->setPayload(data.cbegin(), data.cend(), maxSize);
			fragments.push_back(std::move(first));
		}

		return fragments;
	}

	TunnelFragmentPtr TunnelFragment::parse(ByteArrayConstItr &begin, ByteArrayConstItr end)
	{
		return TunnelFragmentPtr();
	}
}
