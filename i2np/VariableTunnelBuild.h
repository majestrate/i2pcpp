#ifndef I2NPVARIABLETUNNELBUILD_H
#define I2NPVARIABLETUNNELBUILD_H

#include <list>

#include "../datatypes/BuildRecord.h"

#include "Message.h"

namespace i2pcpp {
	namespace I2NP {
		class VariableTunnelBuild : public Message {
			public:
				VariableTunnelBuild();
				VariableTunnelBuild(std::list<BuildRecordPtr> const &buildRecords);
				VariableTunnelBuild(uint32_t msgId, std::list<BuildRecordPtr> const &buildRecords);

				Message::Type getType() const;

				std::list<BuildRecordPtr> getRecords() const;

			protected:
				ByteArray getBytes() const;
				bool parse(ByteArrayConstItr &begin, ByteArrayConstItr end);

			private:
				std::list<BuildRecordPtr> m_buildRecords;
		};
	}
}

#endif
