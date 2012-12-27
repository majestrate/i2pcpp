#ifndef SSUINBOUNDMESSAGESTATE_H
#define SSUINBOUNDMESSAGESTATE_H

#include <memory>
#include <map>
#include <mutex>

#include "../datatypes/ByteArray.h"
#include "../datatypes/RouterHash.h"

using namespace std;

namespace i2pcpp {
	namespace SSU {
		class InboundMessageState {
			public:
				InboundMessageState(RouterHash const &routerHash, const unsigned long msgId) : m_msgid(msgId), m_routerHash(routerHash) {}

				void lock() { m_mutex.lock(); }
				void unlock() { m_mutex.unlock(); }

				void addFragment(const unsigned char fragNum, ByteArray const &data);

				unsigned long getMsgId() { return m_msgid; }
				unsigned char getNumFragments() { return m_fragments.size(); }

			private:
				unsigned long m_msgid;
				RouterHash m_routerHash;

				map<unsigned char, ByteArray> m_fragments;

				mutex m_mutex;
		};

		typedef shared_ptr<InboundMessageState> InboundMessageStatePtr;
	}
}

#endif
