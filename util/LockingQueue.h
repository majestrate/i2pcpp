#ifndef LOCKINGQUEUE_H
#define LOCKINGQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

using namespace std;

namespace i2pcpp {
	template<class T>
		class LockingQueue {
			public:
				LockingQueue() {}

				void enqueue(T const &p)
				{
					m_queueMutex.lock();
					m_queue.push(p);
					m_queueMutex.unlock();
					m_condition.notify_all();
				}

				T pop()
				{
					T ret;

					m_queueMutex.lock();

					if(m_queue.size()) {
						ret = m_queue.front();
						m_queue.pop();
					}

					m_queueMutex.unlock();

					return ret;
				}

				void notify() { m_condition.notify_all(); }
				void wait()
				{
					int size;

					m_queueMutex.lock();
					size = m_queue.size();
					m_queueMutex.unlock();

					if(!size) {
						unique_lock<mutex> lock(m_conditionMutex);
						m_condition.wait(lock);
					}
				}

			private:
				queue<T> m_queue;
				mutex m_queueMutex;

				condition_variable m_condition;
				mutex m_conditionMutex;
		};
}

#endif
