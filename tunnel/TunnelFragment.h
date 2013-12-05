#ifndef TUNNELFRAGMENT_H
#define TUNNELFRAGMENT_H

#include <list>
#include <memory>

#include "../datatypes/ByteArray.h"

#include "../i2np/Message.h"

namespace i2pcpp {
	class TunnelFragment {
		public:
			virtual ~TunnelFragment() = default;

			void setMsgId(uint32_t id);
			void setPayload(ByteArrayConstItr &begin, ByteArrayConstItr end, uint16_t n);

			virtual ByteArray compile() const = 0;

			static std::vector<std::unique_ptr<TunnelFragment>> fragmentMessage(I2NP::MessagePtr const &msg);
			static std::unique_ptr<TunnelFragment> parse(ByteArrayConstItr &begin, ByteArrayConstItr end);

		protected:
			uint32_t m_msgId;
			ByteArray m_payload;

		private:
			virtual uint8_t headerSize() const = 0;
	};

	typedef std::unique_ptr<TunnelFragment> TunnelFragmentPtr;
};

#endif
