#ifndef SSUOUTBOUNDMESSAGESTATE_H
#define SSUOUTBOUNDMESSAGESTATE_H

#include <vector>
#include <forward_list>
#include <memory>

#include "../../datatypes/ByteArray.h"

#include "../../util/DoubleBitfield.h"

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
				void markFragmentSent(const unsigned char fragNum);
				void markFragmentAckd(const unsigned char fragNum);
				bool allFragmentsSent() const;
				bool allFragmentsAckd() const;

			private:
				void fragment();

				uint32_t m_msgId;
				ByteArray m_data;
				std::vector<FragmentPtr> m_fragments;

				DoubleBitfield m_states;
		};

		typedef std::shared_ptr<OutboundMessageState> OutboundMessageStatePtr;
	}
}

#endif
