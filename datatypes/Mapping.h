#ifndef MAPPING_H
#define MAPPING_H

#include <map>
#include <string>

#include "Datatype.h"

namespace i2pcpp {
    class Mapping : public Datatype {
        public:
            Mapping();
            Mapping(ByteArrayConstItr &begin, ByteArrayConstItr end);

            void setValue(std::string const &name, std::string const &value);
            void deleteValue(std::string const &name);
            std::string getValue(std::string const &name) const;
            ByteArray serialize() const;
            std::map<std::string, std::string>::const_iterator begin() const;
            std::map<std::string, std::string>::const_iterator end() const;

        private:
            std::map<std::string, std::string> m_map;
    };
}

#endif
