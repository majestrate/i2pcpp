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
			BuildRequestRecord() = default;
			BuildRequestRecord(BuildRecord const &r);

			TunnelHop parse();
			void compile(TunnelHop const &hop);

		private:
			std::bitset<8> m_flags;
	};

	typedef std::shared_ptr<BuildRequestRecord> BuildRequestRecordPtr;
}

#endif
