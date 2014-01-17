#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include "datatypes/RouterInfo.h"
#include "datatypes/StaticByteArray.h"
#include "datatypes/Mapping.h"
#include "datatypes/Date.h"
#include "datatypes/Endpoint.h"
#include "datatypes/Endpoint.h"
#include "exceptions/FormattingError.h"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(ByteArrayTests)

BOOST_AUTO_TEST_CASE(ToHex)
{

    i2pcpp::ByteArray ba(256, 0);
    for(std::size_t c = 0; c < 256; ++c)
        ba[c] = c;
    std::stringstream ss;
    ss << ba;
    BOOST_CHECK_EQUAL(
        ss.str(),
        "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122"
        "232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445"
        "464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768"
        "696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a8b"
        "8c8d8e8f909192939495969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadae"
        "afb0b1b2b3b4b5b6b7b8b9babbbcbdbebfc0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1"
        "d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4"
        "f5f6f7f8f9fafbfcfdfeff"
    );
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(StaticByteArrayTests)

BOOST_AUTO_TEST_CASE(EmptyToByteArray)
{
    i2pcpp::StaticByteArray<0> sba;
    i2pcpp::ByteArray ba = i2pcpp::toByteArray(sba);
    BOOST_CHECK(ba.empty());
}

BOOST_AUTO_TEST_CASE(ToByteArray)
{
    i2pcpp::StaticByteArray<8> sba{{1, 2, 3, 4, 5, 6, 7, 8}};
    i2pcpp::ByteArray ba = i2pcpp::toByteArray(sba);
    BOOST_CHECK_EQUAL_COLLECTIONS(ba.begin(), ba.end(), sba.begin(), sba.end());
}

BOOST_AUTO_TEST_CASE(FromByteArray)
{
    i2pcpp::ByteArray ba{1, 2, 3, 4, 5, 6, 7, 8};
    i2pcpp::StaticByteArray<8> sba = i2pcpp::toStaticByteArray<8>(ba);
    BOOST_CHECK_EQUAL_COLLECTIONS(sba.begin(), sba.end(), ba.begin(), ba.end());
}

BOOST_AUTO_TEST_CASE(Base64Stream)
{
    i2pcpp::StaticByteArray<8> sba{{1, 2, 3, 4, 5, 6, 7, 8}};
    std::stringstream ss;
    ss << sba;
    BOOST_CHECK_EQUAL(ss.str(), "AQIDBAUGBwg=");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(MappingTests)

struct MappingFixture {
    i2pcpp::ByteArray ba = {
        0x00, 0x0e,
        0x01, 'A', '=', 0x01, 'B', ';',
        0x02, 'A', 'B', '=', 0x02, 'C', 'D', ';'
    };
    i2pcpp::Mapping mp;
    MappingFixture()
    {
        i2pcpp::ByteArrayConstItr it = ba.begin();
        mp = i2pcpp::Mapping(it, ba.end());
    }
};

BOOST_FIXTURE_TEST_CASE(Serialize, MappingFixture)
{
    i2pcpp::ByteArray res = mp.serialize();
    BOOST_CHECK_EQUAL_COLLECTIONS(
        res.begin(), res.end(), ba.begin(), ba.end()
    );
}

BOOST_FIXTURE_TEST_CASE(GetValue, MappingFixture)
{
    BOOST_CHECK_EQUAL(mp.getValue("A"), "B");
    BOOST_CHECK_EQUAL(mp.getValue("AB"), "CD");
}

BOOST_FIXTURE_TEST_CASE(SetValue, MappingFixture)
{
    mp.setValue("A", "X");
    BOOST_CHECK_EQUAL(mp.getValue("A"), "X");
}

BOOST_AUTO_TEST_CASE(EmptyByteArrayThrows)
{
    const i2pcpp::ByteArray ba;
    auto it = ba.begin();
    BOOST_CHECK_THROW(
        i2pcpp::Mapping m(it, ba.end()), i2pcpp::FormattingError
    );
}

BOOST_AUTO_TEST_CASE(IncorrectSizeThrows)
{
    const i2pcpp::ByteArray ba = {
        0x00, 0x0f,
        0x01, 'A', '=', 0x01, 'B', ';',
        0x02, 'A', 'B', '=', 0x02, 'C', 'D', ';'
    };
    auto it = ba.begin();
    BOOST_CHECK_THROW(
        i2pcpp::Mapping m(it, ba.end()), i2pcpp::FormattingError
    );
}

BOOST_AUTO_TEST_CASE(IncorrectLenThrows)
{
    const i2pcpp::ByteArray ba = {
        0x00, 0x0f,
        0x02, 'A', '=', 0x01, 'B', ';',
        0x02, 'A', 'B', '=', 0x02, 'C', 'D', ';'
    };
    auto it = ba.begin();
    BOOST_CHECK_THROW(
        i2pcpp::Mapping m(it, ba.end()), i2pcpp::FormattingError
    );
}

BOOST_FIXTURE_TEST_CASE(DeletedGivesEmpty, MappingFixture)
{
    mp.deleteValue("A");
    mp.deleteValue("AB");
    BOOST_CHECK(mp.getValue("A").empty());
    BOOST_CHECK(mp.getValue("AB").empty());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(DateTests)

BOOST_AUTO_TEST_CASE(Serialize)
{
    i2pcpp::ByteArray b{0, 0, 1, 67, 68, 53, 254, 195};
    i2pcpp::Date d(1388418825923ul);
    i2pcpp::ByteArray res = d.serialize();
    BOOST_CHECK_EQUAL_COLLECTIONS(res.begin(), res.end(), b.begin(), b.end());
}

BOOST_AUTO_TEST_CASE(ConstructFromBytesAndSerialize)
{
    const i2pcpp::ByteArray b{0, 0, 1, 67, 68, 53, 254, 195};
    auto it = b.begin();
    i2pcpp::Date d(it, b.end());
    i2pcpp::ByteArray res = d.serialize();
    BOOST_CHECK_EQUAL_COLLECTIONS(res.begin(), res.end(), b.begin(), b.end());
}

BOOST_AUTO_TEST_CASE(EmptyThrows)
{
    const i2pcpp::ByteArray b;
    auto it = b.begin();
    BOOST_CHECK_THROW(i2pcpp::Date d(it, b.end()), i2pcpp::FormattingError);
}

BOOST_AUTO_TEST_CASE(InvalidSizeThrows)
{
    const i2pcpp::ByteArray b{0, 0, 1, 67, 68, 53, 254};
    auto it = b.begin();
    BOOST_CHECK_THROW(i2pcpp::Date d(it, b.end()), i2pcpp::FormattingError);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(EndpointTests)

BOOST_AUTO_TEST_CASE(GetIp)
{
    i2pcpp::Endpoint ep("127.0.0.1", 1337);
    BOOST_CHECK_EQUAL(ep.getIP(), "127.0.0.1");
}

BOOST_AUTO_TEST_CASE(GetPort)
{
    i2pcpp::Endpoint ep("127.0.0.1", 1337);
    BOOST_CHECK_EQUAL(ep.getPort(), 1337);
}

BOOST_AUTO_TEST_CASE(Equal)
{
    i2pcpp::Endpoint ep1("127.0.0.1", 1337);
    i2pcpp::Endpoint ep2("127.0.0.1", 1337);
    i2pcpp::Endpoint ep3("127.0.0.2", 1337);
    i2pcpp::Endpoint ep4("127.0.0.2", 1338);
    BOOST_CHECK(ep1 == ep2);
    BOOST_CHECK(!(ep1 == ep3));
    BOOST_CHECK(!(ep1 == ep4));
    BOOST_CHECK(!(ep3 == ep4));
}

BOOST_AUTO_TEST_CASE(ToString)
{
    i2pcpp::Endpoint ep("127.0.0.1", 1337);
    std::string s = ep;
    BOOST_CHECK_EQUAL(s, "127.0.0.1:1337");
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(CertificateTests)

BOOST_AUTO_TEST_CASE(Serialize)
{
    const i2pcpp::ByteArray ba = {
        0x01, 0x00, 0x10, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x01, 0x00, 0x10, 0x00, 0x01,
        0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,
        0x0E, 0x0F
    };
    auto it = ba.begin();
    i2pcpp::Certificate cert(it, ba.end());
    const i2pcpp::ByteArray res = cert.serialize();
    BOOST_CHECK_EQUAL_COLLECTIONS(
        ba.begin(), ba.begin() + 19,
        res.begin(), res.end()
    );
}

BOOST_AUTO_TEST_CASE(GetLength)
{
    const i2pcpp::ByteArray ba = {
        0x01, 0x00, 0x10, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x01, 0x00, 0x10, 0x00, 0x01,
        0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,
        0x0E, 0x0F
    };
    auto it = ba.begin();
    i2pcpp::Certificate cert(it, ba.end());
    BOOST_CHECK_EQUAL(cert.getLength(), 16);
}

BOOST_AUTO_TEST_CASE(EmptyThrows)
{
    const i2pcpp::ByteArray ba;
    auto it = ba.begin();
    BOOST_CHECK_THROW(i2pcpp::Certificate c(it, ba.end()), i2pcpp::FormattingError);
}

BOOST_AUTO_TEST_CASE(IncorrectThrows)
{
    const i2pcpp::ByteArray ba = { 0x01, 0x02, 0x03, 0x04, 0x05 };
    auto it = ba.begin();
    BOOST_CHECK_THROW(i2pcpp::Certificate c(it, ba.end()), i2pcpp::FormattingError);
}

BOOST_AUTO_TEST_SUITE_END()


#include "SampleRI.inc"

BOOST_AUTO_TEST_SUITE(RouterInfoTests)

BOOST_AUTO_TEST_CASE(Serialize)
{
    auto it = sample_routerInfo.begin();
    i2pcpp::RouterInfo ri(it, sample_routerInfo.end());
    i2pcpp::ByteArray result = ri.serialize();
    BOOST_CHECK_EQUAL_COLLECTIONS(
        result.begin(), result.end(),
        sample_routerInfo.begin(), sample_routerInfo.end()
    );
}

BOOST_AUTO_TEST_CASE(Verify)
{
    auto it = sample_routerInfo.begin();
    i2pcpp::RouterInfo ri(it, sample_routerInfo.end());
    BOOST_CHECK_EQUAL(ri.verifySignature(), true);
}

BOOST_AUTO_TEST_CASE(GetOptions)
{
    auto it = sample_routerInfo.begin();
    i2pcpp::RouterInfo ri(it, sample_routerInfo.end());
    i2pcpp::Mapping mp = ri.getOptions();
    BOOST_CHECK_EQUAL(mp.getValue("netdb.knownLeaseSets"), "37");
    BOOST_CHECK_EQUAL(mp.getValue("caps"), "OfR");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(RouterIdentityTests)

BOOST_AUTO_TEST_CASE(GetHash)
{
    auto it = sample_routerInfo.begin();
    i2pcpp::RouterInfo ri(it, sample_routerInfo.end());
    i2pcpp::StaticByteArray<32> sba = {
        214, 154, 116, 173, 5, 123, 132, 175, 215, 176, 184, 155, 242, 143, 0,
        29, 126, 248, 145, 97, 230, 70, 48, 236, 152, 58, 133, 237, 168, 160,
        100, 61
    };
    BOOST_CHECK_EQUAL(ri.getIdentity().getHash(), sba);
}

BOOST_AUTO_TEST_CASE(EmptyThrows)
{
    const i2pcpp::ByteArray ba;
    auto it = ba.begin();
    BOOST_CHECK_THROW(i2pcpp::RouterIdentity id(it, ba.end()), i2pcpp::FormattingError);
}

BOOST_AUTO_TEST_CASE(IncorrectThrows)
{
    const i2pcpp::ByteArray ba = { 0x01, 0x02, 0x03, 0x04, 0x05 };
    auto it = ba.begin();
    BOOST_CHECK_THROW(i2pcpp::RouterIdentity id(it, ba.end()), i2pcpp::FormattingError);
}

BOOST_AUTO_TEST_SUITE_END()
