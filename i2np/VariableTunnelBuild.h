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
				VariableTunnelBuild(std::list<BuildRecord> const &buildRecords);

				Message::Type getType() const;

				const std::list<BuildRecord>& getRecords() const;

			protected:
				ByteArray getBytes() const;
				bool parse(ByteArrayConstItr &begin, ByteArrayConstItr end);

			private:
				std::list<BuildRecord> m_buildRecords;
		};
	}
}

#endif
