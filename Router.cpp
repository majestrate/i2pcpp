#include "Router.h"

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include "ssu/UDPTransport.h"
#include "handlers/DatabaseStore.h"

/* TEMPORARY */
#include "util/Base64.h"
#include <botan/pipe.h>
#include "i2np/DatabaseLookup.h"

namespace i2pcpp {
	void Router::start()
	{
		m_inMsgDispatcher.registerHandler(I2NP::Message::Type::DB_STORE, MessageHandlerPtr(new Handlers::DatabaseStore(m_ctx)));

		m_jobRunnerPool.push_back(JobRunnerPtr(new JobRunner(m_jobQueue)));
		for(auto& jr: m_jobRunnerPool)
			jr->start();

		/*JobPtr testJob(new Jobs::TestJob());
		m_jobScheduler.registerJob(testJob, boost::posix_time::time_duration(0, 0, 1));*/

		//m_jobScheduler.start();

		std::cerr << "My router hash: " << m_ctx.getMyRouterIdentity().getHashEncoded() << "\n";

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

		//m_jobScheduler.stop();

		for(auto jr: m_jobRunnerPool)
			jr->stop();
	}

	void Router::databaseLookup(std::string const &to, std::string const &query)
	{
		ByteArray queryBytes = Base64::decode(query);
		std::array<unsigned char, 32> key;
		std::copy(queryBytes.cbegin(), queryBytes.cbegin() + 32, key.begin());

		RouterHash toHash = Base64::decode(to);
		I2NP::MessagePtr dbl(new I2NP::DatabaseLookup(key, m_ctx.getMyRouterHash(), 0));

		m_ctx.getOutMsgDispatcher().sendMessage(toHash, dbl);
	}
}
