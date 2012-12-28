#ifndef SSUINBOUNDMESSAGESTATE_H
#define SSUINBOUNDMESSAGESTATE_H

#include <memory>
#include <map>
#include <mutex>

#include "../datatypes/ByteArray.h"
#include "../datatypes/RouterHash.h"

namespace i2pcpp {
	namespace SSU {
		class InboundMessageState {
			public:
				InboundMessageState(RouterHash const &routerHash, const unsigned long msgId) : m_msgid(msgId), m_routerHash(routerHash), m_gotLast(false), m_byteTotal(0) {}

				bool addFragment(const unsigned char fragNum, ByteArray const &data, bool isLast);
				bool isComplete() const;

				std::mutex& getMutex() { return m_mutex; }
				unsigned long getMsgId() { return m_msgid; }
				unsigned char getNumFragments() { return m_fragments.size(); }
				void assemble(ByteArray &dst) const;

			private:
				unsigned long m_msgid;
				RouterHash m_routerHash;
				bool m_gotLast;
				unsigned char m_lastFragment;
				unsigned long m_byteTotal;

				std::map<unsigned char, ByteArray> m_fragments;

				std::mutex m_mutex;
		};

		typedef std::shared_ptr<InboundMessageState> InboundMessageStatePtr;
	}
}

#endif
