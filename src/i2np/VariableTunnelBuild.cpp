/**
 * @file VariableTunnelBuild.cpp
 * @brief Implements VariableTunnelBuild.h
 */
#include "VariableTunnelBuild.h"

namespace i2pcpp {
    namespace I2NP {
        VariableTunnelBuild::VariableTunnelBuild(std::list<BuildRecordPtr> const &buildRecords) :
            m_buildRecords(buildRecords) {}

        VariableTunnelBuild::VariableTunnelBuild(uint32_t msgId, std::list<BuildRecordPtr> const &buildRecords) :
            Message(msgId),
            m_buildRecords(buildRecords) {}

        std::list<BuildRecordPtr> VariableTunnelBuild::getRecords() const
        {
            return m_buildRecords;
        }

        ByteArray VariableTunnelBuild::compile() const
        {
            ByteArray b;

            b.insert(b.end(), m_buildRecords.size());
            for(auto& r: m_buildRecords) {
                const ByteArray recordBytes = r->serialize();
                b.insert(b.end(), recordBytes.cbegin(), recordBytes.cend());
            }

            return b;
        }

        VariableTunnelBuild VariableTunnelBuild::parse(ByteArrayConstItr &begin, ByteArrayConstItr end)
        {
            VariableTunnelBuild vtb;

            unsigned char size = *begin++;

            for(int i = 0; i < size; i++)
                vtb.m_buildRecords.emplace_back(std::make_shared<BuildRecord>(begin, end));

            return vtb;
        }
    }
}
