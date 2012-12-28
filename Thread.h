#ifndef THREAD_H
#define THREAD_H

#include <atomic>
#include <thread>

namespace i2pcpp {
	class Thread {
		public:
			void start();
			void stop();

		protected:
			~Thread() { } // TODO Need to figure out what to do here

			std::atomic<bool> m_keepRunning;

		private:
			virtual void startHook() {}
			virtual void loop() = 0;
			virtual void stopHook() {}

			std::thread m_thread;
	};
}

#endif
