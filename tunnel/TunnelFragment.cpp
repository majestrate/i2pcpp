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

	void TunnelFragment::setPayload(ByteArrayConstItr &begin, ByteArrayConstItr end, uint16_t n)
	{
		size_t totalSize = headerSize() + std::distance(begin, end);

		if(totalSize < n)
			n = std::distance(begin, end);

		m_payload.clear();
		m_payload.resize(n);
		std::copy(begin, begin += n, m_payload.begin());
	}

	std::vector<TunnelFragmentPtr> TunnelFragment::fragmentMessage(I2NP::MessagePtr const &msg)
	{
		std::vector<TunnelFragmentPtr> fragments;

		ByteArray data = msg->toBytes();
		ByteArrayConstItr pos = data.cbegin();

		std::unique_ptr<FirstFragment> first = std::make_unique<FirstFragment>();
		first->setPayload(pos, data.cend(), 1003);
		if(pos != data.cend()) {
			first->setFragmented(true);

			uint32_t msgId = 0;
			Botan::AutoSeeded_RNG rng;
			rng.randomize((unsigned char *)&msgId, sizeof(msgId));
			first->setMsgId(msgId);
			fragments.push_back(std::move(first));

			uint8_t fragNum = 1;
			while(pos != data.cend()) {
				std::unique_ptr<FollowOnFragment> followup = std::make_unique<FollowOnFragment>(msgId, fragNum++);
				followup->setPayload(pos, data.cend(), 1003);

				if(pos == data.cend())
					followup->setLast(true);

				fragments.push_back(std::move(followup));
			}
		} else {
			fragments.push_back(std::move(first));
		}

		return fragments;
	}

	TunnelFragmentPtr TunnelFragment::parse(ByteArrayConstItr &begin, ByteArrayConstItr end)
	{
		return TunnelFragmentPtr();
	}
}
