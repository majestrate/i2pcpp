#ifndef BUILDRESPONSERECORD_H
#define BUILDRESPONSERECORD_H

#include "SessionKey.h"
#include "BuildRecord.h"

namespace i2pcpp {
	class BuildResponseRecord : public BuildRecord {
		public:
			enum Reply {
				SUCCESS = 0,
				PROBABALISTIC_REJECT = 10,
				TRANSIENT_OVERLOAD = 20,
				BANDWIDTH = 30,
				CRITICAL = 50
			};

			BuildResponseRecord(const BuildRecord &r);

			bool validate() const;
			Reply getReply() const;
	};
}

#endif
