#ifndef DOUBLEBITFIELD_H
#define DOUBLEBITFIELD_H

#include <boost/dynamic_bitset.hpp>

namespace i2pcpp {
	class DoubleBitfield {
		public:
			size_t size() const;
			void resize(const size_t newSize);

			void markA(const unsigned char fragNum);
			void markB(const unsigned char fragNum);

			bool allA() const;
			bool allB() const;

			unsigned char getNextA() const;
			unsigned char getNextB() const;

			std::vector<bool> getA() const;
			std::vector<bool> getB() const;

		private:
			boost::dynamic_bitset<> m_field;
	};
}

#endif
