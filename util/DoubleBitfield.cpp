#include "DoubleBitfield.h"

namespace i2pcpp {
	size_t DoubleBitfield::size() const
	{
		return m_field.size() / 2;
	}

	void DoubleBitfield::resize(const size_t newSize)
	{
		m_field.resize(newSize * 2);
	}

	void DoubleBitfield::markA(const uint8_t fragNum)
	{
		m_field[fragNum * 2] = 1;
	}

	void DoubleBitfield::markB(const uint8_t fragNum)
	{
		m_field[(fragNum * 2) + 1] = 1;
	}

	bool DoubleBitfield::allA() const
	{
		uint8_t i = 0;
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
		uint8_t i = 1;
		size_t size = m_field.size();

		while(i < size) {
			if(!m_field.test(i))
				return false;
			i += 2;
		}

		return true;
	}

	uint8_t DoubleBitfield::getNextA() const
	{
		uint8_t i = 0, size = m_field.size();
		while(i < size && m_field.test(i)) i += 2;

		return i / 2;
	}

	uint8_t DoubleBitfield::getNextB() const
	{
		uint8_t i = 1, size = m_field.size();
		while(i < size && m_field.test(i)) i += 2;

		return i / 2;
	}

	std::vector<bool> DoubleBitfield::getA() const
	{
		std::vector<bool> v;

		uint8_t i = 0, size = m_field.size();
		while(i < size) {
			v.push_back(m_field.test(i));
			i += 2;
		}

		return v;
	}

	std::vector<bool> DoubleBitfield::getB() const
	{
		std::vector<bool> v;

		uint8_t i = 1, size = m_field.size();
		while(i < size) {
			v.push_back(m_field.test(i));
			i += 2;
		}

		return v;
	}
}
