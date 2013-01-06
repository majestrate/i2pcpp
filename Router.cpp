#include "Router.h"

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include "ssu/UDPTransport.h"
#include "handlers/DatabaseStore.h"

#include "util/Base64.h"

namespace i2pcpp {
	void Router::start()
	{
		m_inMsgDispatcher.registerHandler(I2NP::Message::Type::DB_STORE, MessageHandlerPtr(new Handlers::DatabaseStore(m_ctx)));

		m_jobRunnerPool.push_back(JobRunnerPtr(new JobRunner(m_jobQueue)));
		for(auto jr: m_jobRunnerPool)
			jr->start();

		/*JobPtr testJob(new Jobs::TestJob());
		m_jobScheduler.registerJob(testJob, boost::posix_time::time_duration(0, 0, 1));*/

		m_jobScheduler.start();

		m_transport = TransportPtr(new SSU::UDPTransport(m_ctx));
		m_outMsgDispatcher.registerTransport(m_transport);
		std::shared_ptr<SSU::UDPTransport> u = std::dynamic_pointer_cast<SSU::UDPTransport>(m_transport);
		u->begin(Endpoint("127.0.0.1", 27333));
		m_transport->connect(Base64::decode("zhPja0k1cboGnHbhqO50hNPTVHIRE8b4GMwi7Htey~E="));
	}

	void Router::stop()
	{
		std::shared_ptr<SSU::UDPTransport> u = std::dynamic_pointer_cast<SSU::UDPTransport>(m_transport);
		u->shutdown();

		m_jobScheduler.stop();

		for(auto jr: m_jobRunnerPool)
			jr->stop();
	}
}
