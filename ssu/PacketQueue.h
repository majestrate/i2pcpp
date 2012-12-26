#ifndef PACKETQUEUE_H
#define PACKETQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

#include "Packet.h"

using namespace std;

namespace i2pcpp {
	namespace SSU {
		class PacketQueue {
			public:
				PacketQueue() {}
				void enqueue(PacketPtr const &p);
				PacketPtr pop();
				void notify() { m_condition.notify_all(); }
				void wait();

			private:
				queue<PacketPtr> m_packetQueue;
				mutex m_packetQueueMutex;

				condition_variable m_condition;
				mutex m_conditionMutex;
		};
	}
}

#endif
