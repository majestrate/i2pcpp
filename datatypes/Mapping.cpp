#include "Mapping.h"

namespace i2pcpp {
	Mapping::Mapping(ByteArray::const_iterator &mapItr)
	{
		unsigned short size = *(mapItr++) << 8 | *(mapItr++);
		auto end = mapItr + size;

		while(mapItr != end) {
			unsigned char keySize = *(mapItr++);
			string key(mapItr, mapItr + keySize);
			mapItr += keySize;

			unsigned char valueSize = *(mapItr++);
			string value(mapItr, mapItr + valueSize);
			mapItr += valueSize;

			m_map[key] = value;
		}
	}

	void Mapping::setValue(string const &name, string const &value)
	{
		m_map[name] = value;
	}

	void Mapping::deleteValue(string const &name)
	{
		m_map.erase(name);
	}

	string Mapping::getValue(string const &name) const
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

		unsigned short size = ret.size();
		ret.insert(ret.begin(), size);
		ret.insert(ret.begin(), size << 8);

		return ret;
	}
}
