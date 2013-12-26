#include "BuildResponseRecord.h"

#include <stdexcept>

#include <botan/auto_rng.h>
#include <botan/pipe.h>
#include <botan/lookup.h>

#include "../exceptions/FormattingError.h"

namespace i2pcpp {
    BuildResponseRecord::BuildResponseRecord(const BuildRecord &r) :
        BuildRecord(r) {}

    BuildResponseRecord::BuildResponseRecord(Reply r) :
        m_reply(r) {}

    void BuildResponseRecord::parse()
    {
        Botan::Pipe hashPipe(new Botan::Hash_Filter("SHA-256"));
        hashPipe.start_msg();
        hashPipe.write(m_data.data() + 16, 496);
        hashPipe.end_msg();

        std::array<unsigned char, 32> calcHash;
        hashPipe.read(calcHash.data(), 32);

        std::array<unsigned char, 32> givenHash;
        std::copy(m_header.cbegin(), m_header.cend(), givenHash.begin());
        std::copy(m_data.cbegin(), m_data.cbegin() + 16, givenHash.begin() + m_header.size());

        if(givenHash != calcHash)
            throw std::runtime_error("hash verification failed in BuildResponseRecord");

        m_reply = (Reply)m_data[527];
    }

    void BuildResponseRecord::compile()
    {
        Botan::AutoSeeded_RNG rng;

        rng.randomize(m_data.data() + 16, 495);
        m_data[527] = (unsigned char)m_reply;

        Botan::Pipe hashPipe(new Botan::Hash_Filter("SHA-256"));
        hashPipe.start_msg();
        hashPipe.write(m_data.data() + 16, 496);
        hashPipe.end_msg();

        hashPipe.read(m_header.data(), 16);
        hashPipe.read(m_data.data(), 16);
    }

    BuildResponseRecord::Reply BuildResponseRecord::getReply() const
    {
        return m_reply;
    }
}
