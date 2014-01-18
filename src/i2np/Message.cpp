/**
 * @file Message.cpp
 * @brief Implements Message.h
 */
#include "Message.h"

#include <botan/pipe.h>
#include <botan/lookup.h>
#include <botan/auto_rng.h>

#include "DeliveryStatus.h"
#include "DatabaseStore.h"
#include "DatabaseSearchReply.h"
#include "DatabaseLookup.h"
#include "VariableTunnelBuild.h"
#include "VariableTunnelBuildReply.h"
#include "TunnelData.h"
#include "TunnelGateway.h"
#include "Garlic.h"

namespace i2pcpp {
    namespace I2NP {
        ByteArray Message::toBytes(bool standardHeader) const
        {
            ByteArray b(compile());

            if(standardHeader) {
                Botan::Pipe hashPipe(new Botan::Hash_Filter("SHA-256"));
                hashPipe.start_msg();

                hashPipe.write(b);

                hashPipe.end_msg();

                unsigned char checksum;
                hashPipe.read(&checksum, 1);
                uint16_t size = b.size();

                b.insert(b.begin(), checksum);
                b.insert(b.begin(), size);
                b.insert(b.begin(), size >> 8);

                ByteArray d(Date().serialize());
                b.insert(b.begin(), d.cbegin(), d.cend());

                b.insert(b.begin(), m_msgId);
                b.insert(b.begin(), m_msgId >> 8);
                b.insert(b.begin(), m_msgId >> 16);
                b.insert(b.begin(), m_msgId >> 24);
            } else {
                // m_expiration?
                uint32_t expiration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() + 60;

                b.insert(b.begin(), expiration);
                b.insert(b.begin(), expiration >> 8);
                b.insert(b.begin(), expiration >> 16);
                b.insert(b.begin(), expiration >> 24);
            }

            b.insert(b.begin(), (unsigned char)getType());

            return b;
        }

        uint32_t Message::getMsgId() const
        {
            return m_msgId;
        }

        Message::Type Message::getType() const
        {
            auto& ti = typeid(*this);

            if(ti == typeid(DatabaseLookup))
                return Type::DB_LOOKUP;

            if(ti == typeid(DeliveryStatus))
                return Type::DELIVERY_STATUS;

            if(ti == typeid(DatabaseStore))
                return Type::DB_STORE;

            if(ti == typeid(DatabaseSearchReply))
                return Type::DB_SEARCH_REPLY;

            if(ti == typeid(VariableTunnelBuild))
                return Type::VARIABLE_TUNNEL_BUILD;

            if(ti == typeid(VariableTunnelBuildReply))
                return Type::VARIABLE_TUNNEL_BUILD_REPLY;

            if(ti == typeid(TunnelData))
                return Type::TUNNEL_DATA;

            if(ti == typeid(TunnelGateway))
                return Type::TUNNEL_GATEWAY;

            if(ti == typeid(Garlic))
                return Type::GARLIC;

            throw std::runtime_error("couldn't identify myself");
        }

        MessagePtr Message::fromBytes(uint32_t msgId, ByteArray const &data, bool standardHeader)
        {
            MessagePtr m;

            auto dataItr = data.cbegin();
            auto end = data.cend();

            Type mtype = (Type)*(dataItr++);
            Date longExpiration;
            uint32_t expiration;

            if(standardHeader) {
                msgId = parseUint32(dataItr);

                longExpiration = Date(dataItr, end);

                uint16_t size = parseUint16(dataItr);

                uint8_t checksum = *dataItr++; // TODO verify this

                if(end - dataItr != size)
                    throw std::runtime_error("error parsing I2NP message");
            } else
                expiration = parseUint32(dataItr);

            switch(mtype)
            {
                case Type::DELIVERY_STATUS:
                    m = std::make_shared<DeliveryStatus>(DeliveryStatus::parse(dataItr, end));
                    break;

                case Type::DB_LOOKUP:
                    m = std::make_shared<DatabaseLookup>(DatabaseLookup::parse(dataItr, end));
                    break;

                case Type::DB_STORE:
                    m = std::make_shared<DatabaseStore>(DatabaseStore::parse(dataItr, end));
                    break;

                case Type::DB_SEARCH_REPLY:
                    m = std::make_shared<DatabaseSearchReply>(DatabaseSearchReply::parse(dataItr, end));
                    break;

                case Type::VARIABLE_TUNNEL_BUILD:
                    m = std::make_shared<VariableTunnelBuild>(VariableTunnelBuild::parse(dataItr, end));
                    break;

                case Type::VARIABLE_TUNNEL_BUILD_REPLY:
                    m = std::make_shared<VariableTunnelBuildReply>(VariableTunnelBuildReply::parse(dataItr, end));
                    break;

                case Type::TUNNEL_DATA:
                    m = std::make_shared<TunnelData>(TunnelData::parse(dataItr, end));
                    break;

                case Type::TUNNEL_GATEWAY:
                    m = std::make_shared<TunnelGateway>(TunnelGateway::parse(dataItr, end));
                    break;

                case Type::GARLIC:
                    m = std::make_shared<Garlic>(Garlic::parse(dataItr, end));
                    break;

                default:
                    return MessagePtr();
            }

            m->m_msgId = msgId;
            m->m_longExpiration = longExpiration;
            m->m_expiration = expiration;

            return m;
        }

        Message::Message()
        {
            Botan::AutoSeeded_RNG rng;

            rng.randomize((unsigned char *)&m_msgId, sizeof(m_msgId));
        }

        Message::Message(uint32_t msgId) :
            m_msgId(msgId) {}
    }
}
