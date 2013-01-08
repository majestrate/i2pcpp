#ifndef LOCKINGQUEUE_H
#define LOCKINGQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdexcept>

namespace i2pcpp {
	class LockingQueueFinished : public std::exception {};

	template<class T>
		class LockingQueue {
			public:
				LockingQueue() : m_finished(false) {}

				void enqueue(T const &p)
				{
					if(m_finished) return;

					std::unique_lock<std::mutex> lock(m_queueMutex);
					m_queue.push(p);
					lock.unlock();
					m_condition.notify_all();
				}

				T wait_and_pop() throw(LockingQueueFinished)
				{
					std::unique_lock<std::mutex> lock(m_queueMutex);

					m_condition.wait(lock, [this](){ return m_finished || !m_queue.empty(); });
					if(m_finished) throw LockingQueueFinished();
					T ret = m_queue.front();
					m_queue.pop();

					return ret;
				}

				bool isEmpty() const
				{
					if(m_finished) return true;

					std::lock_guard<std::mutex> lock(m_queueMutex);
					return m_queue.empty();
				}

				void finish()
				{
					m_finished = true;
					m_condition.notify_all();
				}

			private:
				std::queue<T> m_queue;
				mutable std::mutex m_queueMutex;
				std::condition_variable m_condition;
				std::atomic<bool> m_finished;
		};
}

#endif
