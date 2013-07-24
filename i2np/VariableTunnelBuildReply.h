#ifndef I2NPVARIABLETUNNELBUILDREPLY_H
#define I2NPVARIABLETUNNELBUILDREPLY_H

#include "VariableTunnelBuild.h"

namespace i2pcpp {
	namespace I2NP {
		class VariableTunnelBuildReply : public VariableTunnelBuild {
			Message::Type getType() const;
		};
	}
}

#endif
