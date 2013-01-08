#include "AcknowledgementScheduler.h"

#include <boost/bind.hpp>

#include "Packet.h"

namespace i2pcpp {
	namespace SSU {
		void AcknowledgementScheduler::loop()
		{
			AcknowledgementTimerPtr timer(new boost::asio::deadline_timer(m_ios, boost::posix_time::time_duration(0, 0, 1)));
			timer->async_wait(boost::bind(&AcknowledgementScheduler::flushAckCallback, this, boost::asio::placeholders::error, timer));

			m_ios.run();
		}

		void AcknowledgementScheduler::flushAckCallback(const boost::system::error_code& e, AcknowledgementTimerPtr &timer)
		{
			try {
				for(auto itr = m_ackList.begin(); itr != m_ackList.end(); ++itr) {
					PeerStatePtr ps = itr->first;
					InboundMessageStatePtr ims = itr->second;

					if(ims->allFragmentsAckd())
						m_ackList.erase(itr++);

					//

					std::lock_guard<std::mutex> lock(ps->getMutex());
				}

				timer->expires_at(timer->expires_at() + boost::posix_time::time_duration(0, 0, 1));
				timer->async_wait(boost::bind(&AcknowledgementScheduler::flushAckCallback, this, boost::asio::placeholders::error, timer));
			} catch(LockingQueueFinished) {}
		}

		void AcknowledgementScheduler::inboundCallback(const boost::system::error_code& e, InboundMessageStatePtr ims)
		{
		}

		void AcknowledgementScheduler::outboundCallback(const boost::system::error_code& e, OutboundMessageStatePtr oms)
		{
		}

		void AcknowledgementScheduler::addAck(PeerStatePtr const &ps, InboundMessageStatePtr const &ims)
		{
			m_ackList.push_back(std::make_pair(ps, ims));
		}

		AcknowledgementTimerPtr AcknowledgementScheduler::createInboundTimer(InboundMessageStatePtr ims)
		{
			AcknowledgementTimerPtr timer(new boost::asio::deadline_timer(m_ios, boost::posix_time::time_duration(0, 0, 10)));

			timer->async_wait(boost::bind(&AcknowledgementScheduler::inboundCallback, this, boost::asio::placeholders::error, ims));

			return timer;
		}

		AcknowledgementTimerPtr AcknowledgementScheduler::createOutboundTimer(OutboundMessageStatePtr oms)
		{
			AcknowledgementTimerPtr timer(new boost::asio::deadline_timer(m_ios, boost::posix_time::time_duration(0, 0, 10)));

			timer->async_wait(boost::bind(&AcknowledgementScheduler::outboundCallback, this, boost::asio::placeholders::error, oms));

			return timer;
		}
	}
}
