#include "JobRunner.h"

#include "JobQueue.h"

namespace i2pcpp {
	void JobRunner::loop()
	{
		try {
		while(m_keepRunning) {
			JobPtr j = m_jobQueue.wait_and_pop();

			if(!j)
				continue;

			j->run();
		}
		} catch(LockingQueueFinished) {}
	}
}
