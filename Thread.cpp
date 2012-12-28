#include "Thread.h"

namespace i2pcpp {
	void Thread::start()
	{
		m_keepRunning = true;
		m_thread = std::thread(&Thread::loop, this);

		startHook();
	}

	void Thread::stop()
	{
		m_keepRunning = false;

		stopHook();

		m_thread.join();
	}
}
