#include "VariableTunnelBuildReply.h"

namespace i2pcpp {
	namespace I2NP {
		VariableTunnelBuildReply::VariableTunnelBuildReply() :
			VariableTunnelBuild() {}

		VariableTunnelBuildReply::VariableTunnelBuildReply(std::list<BuildRecordPtr> const &buildRecords) :
			VariableTunnelBuild(buildRecords) {}

		VariableTunnelBuildReply::VariableTunnelBuildReply(uint32_t msgId, std::list<BuildRecordPtr> const &buildRecords) :
			VariableTunnelBuild(msgId, buildRecords) {}

		Message::Type VariableTunnelBuildReply::getType() const
		{
			return Message::Type::VARIABLE_TUNNEL_BUILD_REPLY;
		}
	}
}
