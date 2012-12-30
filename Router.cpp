#include "Router.h"

#include "ssu/UDPTransport.h"

#include "handlers/DatabaseStore.h"

namespace i2pcpp {
	void Router::start()
	{
		m_inMsgDispatcher.registerHandler(I2NP::Message::Type::DB_STORE, MessageHandlerPtr(new Handlers::DatabaseStore(m_ctx)));
		m_transport.begin(Endpoint("127.0.0.1", 27333)); // TODO Pull from DB

		m_jobRunnerPool.push_back(JobRunnerPtr(new JobRunner(m_jobQueue)));

		for(auto jr: m_jobRunnerPool)
			jr->start();
	}

	void Router::stop()
	{
		m_transport.shutdown();

		for(auto jr: m_jobRunnerPool)
			jr->stop();
	}
}
