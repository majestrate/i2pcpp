#include "JobRunner.h"

#include <iostream>

namespace i2pcpp {
	void JobRunner::loop()
	{
		while(m_keepRunning) {
			m_jobQueue.wait();

			JobPtr j = m_jobQueue.pop();

			if(!j)
				continue;

			j->run();
		}
	}
}
