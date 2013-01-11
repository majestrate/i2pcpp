#include "MessageReceiver.h"

#include <boost/bind.hpp>

#include "UDPTransport.h"

namespace i2pcpp {
	namespace SSU {
		MessageReceiver::MessageReceiver(UDPTransport &transport) :
			m_transport(transport) {}

		void MessageReceiver::messageReceived(InboundMessageStatePtr const &ims)
		{
			std::cerr << "MessageReceiver[" << ims->getMsgId() << "]: Received IMS with " << (int)ims->getNumFragments() << " fragments\n";

			const ByteArray&& data = ims->assemble();

			if(data.size())
				m_transport.m_ios.post(boost::bind(boost::ref(m_transport.m_receivedSignal), ims->getRouterHash(), data));
		}

		void MessageReceiver::addMessage(InboundMessageStatePtr const &ims)
		{
			m_transport.m_ios.post(boost::bind(&MessageReceiver::messageReceived, this, ims));
		}
	}
}
