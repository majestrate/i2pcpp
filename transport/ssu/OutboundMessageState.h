#ifndef SSUOUTBOUNDMESSAGESTATE_H
#define SSUOUTBOUNDMESSAGESTATE_H

#include <vector>
#include <forward_list>
#include <memory>

#include <boost/dynamic_bitset.hpp>

#include "../../datatypes/ByteArray.h"

namespace i2pcpp {
	namespace SSU {
		class OutboundMessageState {
			public:
				struct Fragment {
					uint32_t msgId;
					bool isLast;
					uint8_t fragNum;
					ByteArray data;
				};
				typedef std::shared_ptr<Fragment> FragmentPtr;

				OutboundMessageState(ByteArray const &data);

				const FragmentPtr getNextFragment();
				const FragmentPtr getFragment(const unsigned char fragNum) const;
				void markFragmentSent(unsigned char fragNum);
				void markFragmentReceived(unsigned char fragNum);
				bool allFragmentsSent() const;
				bool allFragmentsReceived() const;

			private:
				void fragment();

				uint32_t m_msgId;
				ByteArray m_data;
				std::vector<FragmentPtr> m_fragments;

				/* Bit 1 = data received (acknowledged)
				 * Bit 0 = data sent (not yet acknowledged)
				 */
				boost::dynamic_bitset<> m_fragmentStates;
		};

		typedef std::shared_ptr<OutboundMessageState> OutboundMessageStatePtr;
	}
}

#endif
