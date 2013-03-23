#ifndef SSUINBOUNDMESSAGESTATE_H
#define SSUINBOUNDMESSAGESTATE_H

#include <memory>
#include <map>
#include <list>

#include <boost/dynamic_bitset.hpp>

#include "../../datatypes/RouterHash.h"

namespace i2pcpp {
	namespace SSU {
		typedef boost::dynamic_bitset<> AckBitfield;
		typedef std::list<std::pair<uint32_t, AckBitfield>> AckList;

		class InboundMessageState {
			public:
				InboundMessageState(RouterHash const &rh, const uint32_t msgId);

				void addFragment(const unsigned char fragNum, ByteArray const &data, bool isLast);
				ByteArray assemble() const;
				bool allFragmentsReceived() const;

				RouterHash getRouterHash() const;
				uint32_t getMsgId() const;
				unsigned char getNumFragments() const;
				const AckBitfield& getAckStates() const;

			private:
				RouterHash m_routerHash;

				uint32_t m_msgId;
				bool m_gotLast = false;
				unsigned char m_lastFragment;
				uint32_t m_byteTotal = 0;

				std::map<unsigned char, ByteArray> m_fragments;
				AckBitfield m_fragmentAckStates;
		};

		typedef std::shared_ptr<InboundMessageState> InboundMessageStatePtr;
	}
}

#endif
