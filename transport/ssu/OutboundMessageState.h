#ifndef SSUOUTBOUNDMESSAGESTATE_H
#define SSUOUTBOUNDMESSAGESTATE_H

#include <vector>
#include <memory>

#include <boost/dynamic_bitset.hpp>

#include "../../datatypes/ByteArray.h"

#include "PacketBuilder.h"

namespace i2pcpp {
	namespace SSU {
		class OutboundMessageState {
			public:
				OutboundMessageState(ByteArray const &data);

				const PacketBuilder::FragmentPtr getNextFragment();
				const PacketBuilder::FragmentPtr getNextUnackdFragment() const;
				void markFragmentSent(const uint8_t fragNum);
				void markFragmentAckd(const uint8_t fragNum);
				bool allFragmentsSent() const;
				bool allFragmentsAckd() const;

				uint32_t getMsgId() const;

			private:
				void fragment();

				uint32_t m_msgId;
				ByteArray m_data;
				std::vector<PacketBuilder::FragmentPtr> m_fragments;

				/* Bit 1 = received (acknowledged)
				 * Bit 0 = sent (not yet acknowledged)
				 */
				boost::dynamic_bitset<> m_fragmentStates;
		};

		typedef std::shared_ptr<OutboundMessageState> OutboundMessageStatePtr;
	}
}

#endif
