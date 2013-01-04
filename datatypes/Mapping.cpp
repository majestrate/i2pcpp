#include "Mapping.h"

#include <iostream>

namespace i2pcpp {
	Mapping::Mapping(ByteArray::const_iterator &mapItr)
	{
		uint16_t size = (*(mapItr++) << 8) | *(mapItr++);
		auto end = mapItr + size;

		while(mapItr != end) {
			unsigned char keySize = *(mapItr++);
			std::string key(mapItr, mapItr + keySize);
			mapItr += keySize;

			mapItr++; // Equal

			unsigned char valueSize = *(mapItr++);
			std::string value(mapItr, mapItr + valueSize);
			mapItr += valueSize;

			mapItr++; // Semi-colon

			m_map[key] = value;
		}

	}

	void Mapping::setValue(std::string const &name, std::string const &value)
	{
		m_map[name] = value;
	}

	void Mapping::deleteValue(std::string const &name)
	{
		m_map.erase(name);
	}

	std::string Mapping::getValue(std::string const &name) const
	{
		return m_map.find(name)->second;
	}

	ByteArray Mapping::getBytes() const
	{
		ByteArray ret;

		for(auto o: m_map) {
			ret.insert(ret.end(), o.first.size());
			ret.insert(ret.end(), o.first.begin(), o.first.end());
			ret.insert(ret.end(), '=');
			ret.insert(ret.end(), o.second.size());
			ret.insert(ret.end(), o.second.begin(), o.second.end());
			ret.insert(ret.end(), ';');
		}

		uint16_t size = ret.size();
		ret.insert(ret.begin(), size);
		ret.insert(ret.begin(), size << 8);

		return ret;
	}
}
