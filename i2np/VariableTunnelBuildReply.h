#ifndef I2NPVARIABLETUNNELBUILDREPLY_H
#define I2NPVARIABLETUNNELBUILDREPLY_H

#include "VariableTunnelBuild.h"

namespace i2pcpp {
	namespace I2NP {
		class VariableTunnelBuildReply : public VariableTunnelBuild {
			public:
				VariableTunnelBuildReply();
				VariableTunnelBuildReply(std::list<BuildRecordPtr> const &buildRecords);
				VariableTunnelBuildReply(uint32_t msgId, std::list<BuildRecordPtr> const &buildRecords);

				Message::Type getType() const;
		};
	}
}

#endif
