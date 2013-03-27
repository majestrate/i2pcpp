#ifndef I2NPVARIABLETUNNELBUILD_H
#define I2NPVARIABLETUNNELBUILD_H

#include <list>

#include "../datatypes/BuildRequestRecord.h"

#include "Message.h"

namespace i2pcpp {
	namespace I2NP {
		class VariableTunnelBuild : public Message {
			public:
				VariableTunnelBuild();
				VariableTunnelBuild(std::list<BuildRequestRecord> const &buildRecords);

				Message::Type getType() const;

				const std::list<BuildRequestRecord>& getRecords() const { return m_buildRecords; }

			protected:
				ByteArray getBytes() const;
				bool parse(ByteArrayConstItr &begin, ByteArrayConstItr end);

			private:
				std::list<BuildRequestRecord> m_buildRecords;
		};
	}
}

#endif
