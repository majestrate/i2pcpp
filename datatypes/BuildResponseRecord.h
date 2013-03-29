#ifndef BUILDRESPONSERECORD_H
#define BUILDRESPONSERECORD_H

#include "SessionKey.h"

namespace i2pcpp {
	class BuildResponseRecord {
		public:
			enum Reply {
				SUCCESS = 0,
				PROBABALISTIC_REJECT = 10,
				TRANSIENT_OVERLOAD = 20,
				BANDWIDTH = 30,
				CRITICAL = 50
			};

			BuildResponseRecord(ByteArrayConstItr &begin, ByteArrayConstItr end);

			void decrypt(SessionKey const &iv, SessionKey const &key);
			Reply getReply() const;

		private:
			ByteArray m_data;
	};
}

#endif
