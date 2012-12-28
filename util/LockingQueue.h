#ifndef LOCKINGQUEUE_H
#define LOCKINGQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

namespace i2pcpp {
	template<class T>
		class LockingQueue {
			public:
				LockingQueue() {}

				void enqueue(T const &p)
				{
					std::lock_guard<std::mutex> lock(m_queueMutex);
					m_queue.push(p);
					m_condition.notify_all();
				}

				T pop()
				{
					std::lock_guard<std::mutex> lock(m_queueMutex);

					T ret;

					if(m_queue.size()) {
						ret = m_queue.front();
						m_queue.pop();
					}

					return ret;
				}

				void notify() { m_condition.notify_all(); }
				void finish() { m_finished = true; m_condition.notify_all(); }

				void wait()
				{
					int size;

					if(m_finished) return;

					m_queueMutex.lock();
					size = m_queue.size();
					m_queueMutex.unlock();

					if(!size) {
						std::unique_lock<std::mutex> lock(m_conditionMutex);
						m_condition.wait(lock);
					}
				}

			private:
				std::queue<T> m_queue;
				std::mutex m_queueMutex;

				std::condition_variable m_condition;
				std::mutex m_conditionMutex;

				bool m_finished;
		};
}

#endif
