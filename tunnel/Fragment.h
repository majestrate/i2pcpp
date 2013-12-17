#ifndef FRAGMENT_H
#define FRAGMENT_H

#include <list>
#include <memory>

#include "../datatypes/ByteArray.h"

#include "../i2np/Message.h"

namespace i2pcpp {
	class Fragment {
		public:
			virtual ~Fragment() = default;

			void setMsgId(uint32_t id);
			uint32_t getMsgId() const;

			void setPayload(ByteArrayConstItr &begin, ByteArrayConstItr end, uint16_t max);
			const ByteArray& getPayload() const;
			uint16_t size() const;

			virtual ByteArray compile() const = 0;

			static std::vector<std::unique_ptr<Fragment>> fragmentMessage(ByteArray const &data);
			static std::unique_ptr<Fragment> parse(ByteArrayConstItr &begin, ByteArrayConstItr end);

		protected:
			uint32_t m_msgId = 0;
			ByteArray m_payload;

		private:
			virtual uint8_t headerSize() const = 0;
	};

	typedef std::unique_ptr<Fragment> FragmentPtr;
};

#endif
