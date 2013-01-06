#ifndef JOBSCHEDULER_H
#define JOBSCHEDULER_H

#include <map>
#include <memory>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "Thread.h"
#include "Job.h"
#include "JobQueue.h"

namespace i2pcpp {
	class JobScheduler : public Thread {
		public:
			JobScheduler(JobQueue &jq) : m_jobQueue(jq) {}

			void callback(const boost::system::error_code& e, std::shared_ptr<boost::asio::deadline_timer> const &timer, JobPtr const &job);
			void registerJob(JobPtr const &job, boost::asio::deadline_timer::duration_type const &duration);

		private:
			void loop();
			void stopHook() { m_ios.stop(); m_jobQueue.finish(); }

			JobQueue& m_jobQueue;
			boost::asio::io_service m_ios;
			std::map<unsigned long, boost::asio::deadline_timer::duration_type> m_durations;
	};
}

#endif
