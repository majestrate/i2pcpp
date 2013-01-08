#ifndef SSUINBOUNDMESSAGESTATE_H
#define SSUINBOUNDMESSAGESTATE_H

#include <memory>
#include <map>
#include <list>

#include <boost/dynamic_bitset.hpp>

#include "../datatypes/ByteArray.h"
#include "../datatypes/RouterHash.h"

namespace i2pcpp {
	namespace SSU {
		typedef boost::dynamic_bitset<> AckBitfield;
		typedef std::list<std::pair<uint32_t, AckBitfield>> AckList;

		class InboundMessageState {
			public:
				InboundMessageState(RouterHash const &routerHash, const uint32_t msgId) : m_msgId(msgId), m_routerHash(routerHash), m_gotLast(false), m_byteTotal(0) {}

				void addFragment(const unsigned char fragNum, ByteArray const &data, bool isLast);
				ByteArray assemble() const;

				uint32_t getMsgId() const { return m_msgId; }
				const RouterHash& getRouterHash() const { return m_routerHash; }
				unsigned char getNumFragments() const { return m_fragments.size(); }

				const AckBitfield& getAckStates() const { return m_fragmentAckStates; }
				bool allFragmentsReceived() const;

			private:
				uint32_t m_msgId;
				RouterHash m_routerHash;
				bool m_gotLast;
				unsigned char m_lastFragment;
				uint32_t m_byteTotal;

				std::map<unsigned char, ByteArray> m_fragments;
				AckBitfield m_fragmentAckStates;
		};

		typedef std::shared_ptr<InboundMessageState> InboundMessageStatePtr;
	}
}

#endif
