#ifndef JOBRUNNER_H
#define JOBRUNNER_H

#include <memory>
#include <vector>

#include "Thread.h"
#include "JobQueue.h"

namespace i2pcpp {
	class JobRunner : public Thread {
		public:
			JobRunner(JobQueue &jq) : m_jobQueue(jq) {}

		private:
			void loop();
			void stopHook() { m_jobQueue.finish(); }

			JobQueue& m_jobQueue;
	};

	typedef std::shared_ptr<JobRunner> JobRunnerPtr;
	typedef std::vector<JobRunnerPtr> JobRunnerPool;
}

#endif
