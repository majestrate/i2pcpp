#ifndef JOBQUEUE_H
#define JOBQUEUE_H

#include "util/LockingQueue.h"

#include "Job.h"

namespace i2pcpp {
	typedef LockingQueue<JobPtr> JobQueue;
}

#endif
