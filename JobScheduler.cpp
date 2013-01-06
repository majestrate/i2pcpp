#include "JobScheduler.h"

#include <iostream>

namespace i2pcpp {
	void JobScheduler::callback(const boost::system::error_code& e, std::shared_ptr<boost::asio::deadline_timer> const &timer, JobPtr const &job)
	{
		m_jobQueue.enqueue(job);

		timer->expires_at(timer->expires_at() + m_durations[job->getId()]);
		timer->async_wait(boost::bind(&JobScheduler::callback, this, boost::asio::placeholders::error, timer, job));
	}

	void JobScheduler::registerJob(JobPtr const &job, boost::asio::deadline_timer::duration_type const &duration)
	{
		std::shared_ptr<boost::asio::deadline_timer> timer(new boost::asio::deadline_timer(m_ios, duration));

		m_durations[job->getId()] = duration;

		timer->async_wait(boost::bind(&JobScheduler::callback, this, boost::asio::placeholders::error, timer, job));
	}

	void JobScheduler::loop()
	{
		m_ios.run();
	}
}
