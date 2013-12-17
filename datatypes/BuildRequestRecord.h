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
            /**
             * Constructs from a pointer to a i2pcpp::TunnelHop.
             * @param hop pointer to the tunnel hop
             */
			BuildRequestRecord(TunnelHopPtr const &hop);
			BuildRequestRecord(BuildRecord const &r);

			void parse();
			void compile();

            /**
             * Returns a reference to the associated i2pcpp::TunnelHop.
             */
			TunnelHop& getHop();

		private:
			TunnelHop m_hop;
			std::bitset<8> m_flags;
	};

	typedef std::shared_ptr<BuildRequestRecord> BuildRequestRecordPtr;
}

#endif
