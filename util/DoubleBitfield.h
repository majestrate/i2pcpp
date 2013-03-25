#ifndef DOUBLEBITFIELD_H
#define DOUBLEBITFIELD_H

#include <boost/dynamic_bitset.hpp>

namespace i2pcpp {
	class DoubleBitfield {
		public:
			size_t size() const;
			void resize(const size_t newSize);

			void markA(const uint8_t fragNum);
			void markB(const uint8_t fragNum);

			bool allA() const;
			bool allB() const;

			uint8_t getNextA() const;
			uint8_t getNextB() const;

			std::vector<bool> getA() const;
			std::vector<bool> getB() const;

		private:
			boost::dynamic_bitset<> m_field;
	};
}

#endif
