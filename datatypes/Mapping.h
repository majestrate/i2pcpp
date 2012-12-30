#ifndef MAPPING_H
#define MAPPING_H

#include <map>
#include <string>

#include "Datatype.h"
#include "ByteArray.h"

namespace i2pcpp {
	class Mapping : public Datatype {
		public:
			Mapping() {}
			Mapping(ByteArray::const_iterator &mapItr);

			void setValue(std::string const &name, std::string const &value);
			void deleteValue(std::string const &name);
			std::string getValue(std::string const &name) const;
			ByteArray getBytes() const;
			std::map<std::string, std::string>::const_iterator begin() const { return m_map.cbegin(); }
			std::map<std::string, std::string>::const_iterator end() const { return m_map.cend(); }

		private:
			std::map<std::string, std::string> m_map;
	};
}

#endif
