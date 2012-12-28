#ifndef JOBRUNNER_H
#define JOBRUNNER_H

#include "Thread.h"

namespace i2pcpp {
	class JobRunner : public Thread {
		public:
			void loop();
	};
}

#endif
