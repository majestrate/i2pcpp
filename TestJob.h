#ifndef TESTJOB_H
#define TESTJOB_H

#include <iostream>

#include "Job.h"

namespace i2pcpp {
	class TestJob : public Job {
		public:
			void run() { std::cerr << "TestJob ran (job #" << m_id << ")\n"; }
	};
}

#endif
