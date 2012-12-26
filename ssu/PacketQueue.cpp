#include "PacketQueue.h"

namespace i2pcpp {
	namespace SSU {
		void PacketQueue::enqueue(PacketPtr const &p)
		{
			m_packetQueueMutex.lock();
			m_packetQueue.push(p);
			m_packetQueueMutex.unlock();
			m_condition.notify_all();
		}

		PacketPtr PacketQueue::pop()
		{
			m_packetQueueMutex.lock();

			if(!m_packetQueue.size())
				return PacketPtr();

			PacketPtr p = m_packetQueue.front();
			m_packetQueue.pop();

			m_packetQueueMutex.unlock();

			return p;
		}

		void PacketQueue::wait()
		{
			int size;

			m_packetQueueMutex.lock();
			size = m_packetQueue.size();
			m_packetQueueMutex.unlock();

			if(!size) {
				unique_lock<mutex> lock(m_conditionMutex);
				m_condition.wait(lock);
			}
		}
	}
}
