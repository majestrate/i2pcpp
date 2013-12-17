/**
 * @file BuildResponseRecord.h
 * @brief Defines the i2pcpp::BuildResponseRecord type.
 */
#ifndef BUILDRESPONSERECORD_H
#define BUILDRESPONSERECORD_H

#include "SessionKey.h"
#include "BuildRecord.h"

namespace i2pcpp {
    /**
     * The tunnel response record type. Consists of a 32 bytes hash,
     *  followed by 495 bytes of random badding and finally the reply value.
     */
	class BuildResponseRecord : public BuildRecord {
		public:

            /**
             * This is a one byte value indicates success or failure responses 
             *  on tunnel build requests. 
             * SUCCESS(0x00) means that the hop agrees to participate.
             * Any higher value indicates that the hop rejects the build request.
             */
			enum class Reply {
				SUCCESS = 0,
				PROBABALISTIC_REJECT = 10,
				TRANSIENT_OVERLOAD = 20,
				BANDWIDTH = 30,
				CRITICAL = 50
			};
            
            /**
             * Creates a tunnel response record from a
             *  i2pcpp::BuildResponseRecord::Reply.
             */
			BuildResponseRecord(Reply r);

            /**
             * Constructs from an i2pcpp::BuildRecord.
             */
			BuildResponseRecord(const BuildRecord &r);

            /**
             * Checks whether the given data and corresponding hash are correct.
             * That is, recomputes the hash and compares it to the given hash.
             * @return true if the hash is correct, false otherwise
             */
			bool parse();

            /**
             * Creates the record by first generating 495 bytes of random padding followed
             *  by the reply byte. Then computes the SHA256 hash, which is placed before
             *  that. 
             */
			void compile();

            /**
             * @return i2pcpp::BuildResponseRecord::m_reply
             */
			Reply getReply() const;

		private:
			Reply m_reply;
	};

	typedef std::shared_ptr<BuildResponseRecord> BuildResponseRecordPtr;
}

#endif
