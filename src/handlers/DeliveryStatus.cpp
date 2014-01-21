/**
 * @file DeliveryStatus.cpp
 * @brief Implements DeliveryStatus.h
 */
#include "DeliveryStatus.h"

#include <botan/pipe.h>

#include <i2pcpp/util/gzip.h>
#include "../i2np/DatabaseStore.h"

#include "../RouterContext.h"

namespace i2pcpp {
    namespace Handlers {
        DeliveryStatus::DeliveryStatus(RouterContext &ctx) :
            Message(ctx),
            m_log(I2P_LOG_CHANNEL("H[DStat]")) {}

        void DeliveryStatus::handleMessage(RouterHash const from, I2NP::MessagePtr const msg)
        {
            I2P_LOG_SCOPED_TAG(m_log, "RouterHash", from);
            I2P_LOG(m_log, debug) << "received DeliveryStatus message, replying with DatabaseStore message";

            // TODO Get this out of here
            Mapping am;
            am.setValue("caps", "BC");
            am.setValue("host", m_ctx.getDatabase().getConfigValue("ssu_external_ip"));
            am.setValue("key", Base64::encode(m_ctx.getIdentity()->getHash()));
            am.setValue("port", m_ctx.getDatabase().getConfigValue("ssu_external_port"));
            RouterAddress a(5, Date(0), "SSU", am);

            Mapping rm;
            rm.setValue("coreVersion", "0.9.9");
            rm.setValue("netId", "2");
            rm.setValue("router.version", "0.9.9");
            rm.setValue("stat_uptime", "90m");
            rm.setValue("caps", "OR");
            RouterInfo myInfo(*m_ctx.getIdentity(), Date(), rm);
            myInfo.addAddress(a);
            myInfo.sign(m_ctx.getSigningKey());

            Botan::Pipe gzPipe(new Gzip_Compression);
            gzPipe.start_msg();
            gzPipe.write(myInfo.serialize());
            gzPipe.end_msg();

            unsigned int size = gzPipe.remaining();
            ByteArray gzInfoBytes(size);
            gzPipe.read(gzInfoBytes.data(), size);

            auto mydsm = std::make_shared<I2NP::DatabaseStore>(myInfo.getIdentity().getHash(), I2NP::DatabaseStore::DataType::ROUTER_INFO, 0, gzInfoBytes);
            m_ctx.getOutMsgDisp().sendMessage(from, mydsm);
        }
    }
}
