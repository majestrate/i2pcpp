/**
 * @file BuildRequestRecord.h
 * @brief Defines the i2pcpp::BuildRequestRecord type.
 */
#ifndef BUILDREQUESTRECORD_H
#define BUILDREQUESTRECORD_H

#include <memory>

#include <botan/elgamal.h>

#include "Datatype.h"
#include "ByteArray.h"
#include "BuildRecord.h"

#include "../tunnel/TunnelHop.h"

namespace i2pcpp {
    /**
     * Holds the tunnel build record request record.
     */
	class BuildRequestRecord : public BuildRecord {
		public:
			BuildRequestRecord() = default;

            /**
             * Constructs from an i2pcpp::BuilRecord.
             */
			BuildRequestRecord(BuildRecord const &r);

			TunnelHop parse();
			void compile(TunnelHop const &hop);

            /**
             * Returns a reference to the associated i2pcpp::TunnelHop.
             */
			TunnelHop& getHop();

		private:
			std::bitset<8> m_flags;
	};

	typedef std::shared_ptr<BuildRequestRecord> BuildRequestRecordPtr;
}

#endif
