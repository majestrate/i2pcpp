/**
 * @file Mapping.h
 * @brief Defines the i2pcpp::Mapping datatype.
 */
#ifndef MAPPING_H
#define MAPPING_H

#include "Datatype.h"

#include <map>
#include <string>

namespace i2pcpp {
    /**
     * A set of (unique) key/value pairs. Implemented on top of
     *  std::map<std::string, std::string>.
     */
    class Mapping : public Datatype {
        public:
            Mapping() = default;

            /**
             * Constructs an i2pcpp::Mapping from iterators to the begin and end of
             *  an i2pcpp::ByteArray. The format must be:
             * size (2B) | len (1B) | key (len) | "=" | len (1B) | value (len) | ";" | ...
             */
            Mapping(ByteArrayConstItr &begin, ByteArrayConstItr end);

            /**
             * Sets the element at \a name to \a value.
             */
            void setValue(std::string const &name, std::string const &value);

            /**
             * Deletes the value at \a name.
             */
            void deleteValue(std::string const &name);

            /**
             * @return the value at \a name
             */
            std::string getValue(std::string const &name) const;

            /**
             * Serializes in the following format:
             * size (2B) | len (1B) | key (len) | "=" | len (1B) | value (len) | ";" | ...
             */
            ByteArray serialize() const;

            /**
             * @return an iterator to the begin of the underlying std::map.
             */
            std::map<std::string, std::string>::const_iterator begin() const;

            /**
             * @return an iterator to the end of the underlying std::map.
             */
            std::map<std::string, std::string>::const_iterator end() const;

        private:
            /// the underlying std::map<std::string, std::string> for storing the pairs
            std::map<std::string, std::string> m_map;
    };
}

#endif
