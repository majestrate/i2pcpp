#ifndef MAPPING_H
#define MAPPING_H

#include <unordered_map>
#include <string>

#include "Datatype.h"
#include "ByteArray.h"

using namespace std;

namespace i2pcpp {
	class Mapping : public Datatype {
		public:
			Mapping() {}
			Mapping(ByteArray::const_iterator &mapItr);

			void setValue(string const &name, string const &value);
			void deleteValue(string const &name);
			string getValue(string const &name) const;
			ByteArray getBytes() const;

		private:
			unordered_map<string, string> m_map;
	};
}

#endif
