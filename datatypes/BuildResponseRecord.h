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

			BuildResponseRecord(Reply r);
			BuildResponseRecord(const BuildRecord &r);

			void parse();
			void compile();

			Reply getReply() const;

		private:
			Reply m_reply;
	};
}

#endif
