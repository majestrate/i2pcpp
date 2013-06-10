#ifndef SSUINBOUNDMESSAGESTATE_H
#define SSUINBOUNDMESSAGESTATE_H

#include <memory>
#include <map>
#include <list>

#include "../../datatypes/RouterHash.h"

namespace i2pcpp {
	namespace SSU {
		class InboundMessageState {
			public:
				InboundMessageState(RouterHash const &rh, const uint32_t msgId);

				void addFragment(const uint8_t fragNum, ByteArray const &data, bool isLast);
				ByteArray assemble() const;

				RouterHash getRouterHash() const;
				uint32_t getMsgId() const;

				bool allFragmentsReceived() const;
				std::vector<bool> getFragmentsReceived() const;

			private:
				RouterHash m_routerHash;

				uint32_t m_msgId;
				bool m_gotLast = false;
				uint8_t m_lastFragment;
				uint32_t m_byteTotal = 0;

				std::vector<ByteArrayPtr> m_fragments;
		};

		typedef std::shared_ptr<InboundMessageState> InboundMessageStatePtr;
	}
}

#endif
