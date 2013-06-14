#ifndef DHTDISTRIBUTEDHASHTABLE_H
#define DHTDISTRIBUTEDHASHTABLE_H

#include <boost/signals2.hpp>
#include <memory>

namespace i2pcpp {
	namespace DHT {
		template<typename Key, typename Value>
			class DistributedHashTable {
				public:
					typedef boost::signals2::signal<void(const Key, const Value)> SuccessSignal;
					typedef boost::signals2::signal<void(const Key)> FailureSignal;
					typedef std::shared_ptr<SuccessSignal> SuccessSignalPtr;
					typedef std::shared_ptr<FailureSignal> FailureSignalPtr;

					DistributedHashTable() {}
					DistributedHashTable(const DistributedHashTable &) = delete;
					DistributedHashTable& operator=(DistributedHashTable &) = delete;
					virtual ~DistributedHashTable() {}

					virtual void insert(Key const &k, Value const &v) = 0;
					virtual void erase(Key const &k) = 0;
					virtual void find(Key const &k) = 0;

					boost::signals2::connection registerSuccessHandler(typename SuccessSignal::slot_type const &sh)
					{
						return m_successSignal.connect(sh);
					}

					boost::signals2::connection registerFailureHandler(typename FailureSignal::slot_type const &fh)
					{
						return m_failureSignal.connect(fh);
					}

				protected:
					SuccessSignal m_successSignal;
					FailureSignal m_failureSignal;
			};
	}
}

#endif
