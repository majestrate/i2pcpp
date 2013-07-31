#ifndef BUILDREQUESTRECORD_H
#define BUILDREQUESTRECORD_H

#include <memory>

#include <botan/elgamal.h>

#include "Datatype.h"
#include "ByteArray.h"
#include "BuildRecord.h"

#include "../tunnel/TunnelHop.h"

namespace i2pcpp {
	class BuildRequestRecord : public BuildRecord {
		public:
			BuildRequestRecord(TunnelHopPtr const &hop);
			BuildRequestRecord(BuildRecord const &r);

			void parse();
			void compile();

			TunnelHop& getHop();

		private:
			TunnelHop m_hop;
			std::bitset<8> m_flags;
			uint32_t m_requestTime;
			uint32_t m_nextMsgId;
	};

	typedef std::shared_ptr<BuildRequestRecord> BuildRequestRecordPtr;
}

#endif
