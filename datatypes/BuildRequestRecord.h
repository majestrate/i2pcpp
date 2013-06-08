#ifndef BUILDREQUESTRECORD_H
#define BUILDREQUESTRECORD_H

#include <botan/elgamal.h>

#include "Datatype.h"
#include "ByteArray.h"
#include "BuildRecord.h"

#include "../tunnel/TunnelHop.h"

namespace i2pcpp {
	class BuildRequestRecord : public BuildRecord {
		public:
			enum Flags {
				ALLOW_FROM_ALL = (1 << 7),
				ALLOW_TO_ALL = (1 << 6),
			};

			BuildRequestRecord(const BuildRecord &r);

			void parse();
			void compile();

			TunnelHop& getHop();

		private:
			TunnelHop m_hop;
			Flags m_flags;
			uint32_t m_requestTime;
			uint32_t m_nextMsgId;
	};
}

#endif
