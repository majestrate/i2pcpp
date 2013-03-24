#include "DoubleBitfield.h"

namespace i2pcpp {
	size_t DoubleBitfield::size() const
	{
		return m_field.size();
	}

	void DoubleBitfield::resize(const size_t newSize)
	{
		m_field.resize(newSize * 2);
	}

	void DoubleBitfield::markA(const unsigned char fragNum)
	{
		m_field[fragNum * 2] = 1;
	}

	void DoubleBitfield::markB(const unsigned char fragNum)
	{
		m_field[(fragNum * 2) + 1] = 1;
	}

	bool DoubleBitfield::allA() const
	{
		unsigned char i = 0;
		size_t size = m_field.size();

		while(i < size) {
			if(!m_field.test(i))
				return false;
			i += 2;
		}

		return true;
	}

	bool DoubleBitfield::allB() const
	{
		unsigned char i = 1;
		size_t size = m_field.size();

		while(i < size) {
			if(!m_field.test(i))
				return false;
			i += 2;
		}

		return true;
	}

	unsigned char DoubleBitfield::getNextA() const
	{
		unsigned char i = 0, size = m_field.size();
		while(i < size && m_field.test(i)) i += 2;

		return i / 2;
	}

	unsigned char DoubleBitfield::getNextB() const
	{
		unsigned char i = 1, size = m_field.size();
		while(i < size && m_field.test(i)) i += 2;

		return i / 2;
	}

	std::vector<bool> DoubleBitfield::getA() const
	{
		std::vector<bool> v;

		unsigned char i = 0, size = m_field.size();
		while(i < size) {
			v.push_back(m_field.test(i));
			i += 2;
		}
	}

	std::vector<bool> DoubleBitfield::getB() const
	{
		std::vector<bool> v;

		unsigned char i = 1, size = m_field.size();
		while(i < size) {
			v.push_back(m_field.test(i));
			i += 2;
		}
	}
}
