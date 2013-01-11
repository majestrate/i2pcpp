#include "MessageReceiver.h"

#include <boost/bind.hpp>

#include "../InboundMessageDispatcher.h"
#include "../i2np/Message.h"

#include "UDPTransport.h"

namespace i2pcpp {
	namespace SSU {
		MessageReceiver::MessageReceiver(UDPTransport &transport) :
			boost::asio::io_service::service::service(transport.m_ios),
			m_ctx(transport.m_ctx) {}

		void MessageReceiver::messageReceived(InboundMessageStatePtr const &ims)
		{
			InboundMessageDispatcher &imd = m_ctx.getInMsgDispatcher();

			std::cerr << "MessageReceiver[" << ims->getMsgId() << "]: Received IMS with " << (int)ims->getNumFragments() << " fragments\n";

			const ByteArray&& data = ims->assemble();

			if(data.size()) {
				I2NP::MessagePtr m = I2NP::Message::fromBytes(data);

				if(m) {
					std::cerr << "MessageReceiver[" << ims->getMsgId() << "]: This looks like a message of type: " << (int)m->getType() << "\n";

					imd.receiveMessage(ims->getRouterHash(), m);
				}
			}
		}

		void MessageReceiver::addMessage(InboundMessageStatePtr const &ims)
		{
			get_io_service().post(boost::bind(&MessageReceiver::messageReceived, this, ims));
		}
	}
}
