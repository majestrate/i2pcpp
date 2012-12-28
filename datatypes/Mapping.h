#ifndef MAPPING_H
#define MAPPING_H

#include <unordered_map>
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

		private:
			std::unordered_map<std::string, std::string> m_map;
	};
}

#endif
