#include "JobRunner.h"

#include "JobQueue.h"

#include <iostream>

namespace i2pcpp {
	void JobRunner::loop()
	{
		try {
			while(m_keepRunning) {
				JobPtr j = m_jobQueue.wait_and_pop();

				j->run();
			}
		} catch(LockingQueueFinished) {}
	}
}
