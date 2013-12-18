#ifndef I2NPVARIABLETUNNELBUILDREPLY_H
#define I2NPVARIABLETUNNELBUILDREPLY_H

#include <list>

#include "../datatypes/BuildRecord.h"

#include "Message.h"

namespace i2pcpp {
	namespace I2NP {
		class VariableTunnelBuildReply : public Message {
			public:
				VariableTunnelBuildReply(std::list<BuildRecordPtr> const &buildRecords);
				VariableTunnelBuildReply(uint32_t msgId, std::list<BuildRecordPtr> const &buildRecords);

				std::list<BuildRecordPtr> getRecords() const;

				static VariableTunnelBuildReply parse(ByteArrayConstItr &begin, ByteArrayConstItr end);

			protected:
				VariableTunnelBuildReply() = default;

				ByteArray compile() const;

			private:
				std::list<BuildRecordPtr> m_buildRecords;
		};
	}
}

#endif
