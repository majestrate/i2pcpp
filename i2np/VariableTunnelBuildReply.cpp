#include "VariableTunnelBuildReply.h"

namespace i2pcpp {
	namespace I2NP {
		Message::Type VariableTunnelBuildReply::getType() const
		{
			return Message::Type::VARIABLE_TUNNEL_BUILD_REPLY;
		}
	}
}
