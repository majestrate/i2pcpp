/**
 * @file Mapping.cpp
 * @brief Implements Mapping.h.
 */
#include "Mapping.h"

#include "../exceptions/FormattingError.h"

namespace i2pcpp {

    Mapping::Mapping(ByteArrayConstItr &begin, ByteArrayConstItr end)
    {
        if(std::distance(begin, end) < 2)
            throw FormattingError();

        uint16_t size = parseUint16(begin);
        if(std::distance(begin, end) < size)
            throw FormattingError();

        end = begin + size;

        while(begin != end) {
            unsigned char keySize = *(begin++);
            if(std::distance(begin, end) < keySize)
                throw FormattingError();
            std::string key(begin, begin + keySize);
            begin += keySize;

            begin++; // Equal

            unsigned char valueSize = *(begin++);
            if(std::distance(begin, end) < valueSize)
                throw FormattingError();
            std::string value(begin, begin + valueSize);
            begin += valueSize;

            begin++; // Semi-colon

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
        std::string s;

        auto itr = m_map.find(name);
        if(itr != m_map.end())
            s = itr->second;

        return s;
    }

    ByteArray Mapping::serialize() const
    {
        ByteArray ret;

        for(auto& o: m_map) {
            ret.insert(ret.end(), o.first.size());
            ret.insert(ret.end(), o.first.begin(), o.first.end());
            ret.insert(ret.end(), '=');
            ret.insert(ret.end(), o.second.size());
            ret.insert(ret.end(), o.second.begin(), o.second.end());
            ret.insert(ret.end(), ';');
        }

        uint16_t size = ret.size();
        ret.insert(ret.begin(), size);
        ret.insert(ret.begin(), size >> 8);

        return ret;
    }

    std::map<std::string, std::string>::const_iterator Mapping::begin() const
    {
        return m_map.cbegin();
    }

    std::map<std::string, std::string>::const_iterator Mapping::end() const
    {
        return m_map.cend();
    }
}
