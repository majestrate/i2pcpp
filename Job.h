#ifndef JOB_H
#define JOB_H

/* DEY TOOK R JERBZ! */

#include <atomic>
#include <memory>

namespace i2pcpp {
	class Job {
		public:
			Job() : m_id(m_idSource++) {}

			unsigned long getId() { return m_id; }
			virtual void run() = 0;

		protected:
			std::atomic<unsigned long> m_id;

		private:
			static std::atomic<unsigned long> m_idSource;
	};

	typedef std::shared_ptr<Job> JobPtr;
}

#endif
