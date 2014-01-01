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

BOOST_AUTO_TEST_SUITE(StaticByteArrayTests)

BOOST_AUTO_TEST_CASE(EmptyToByteArray)
{
    i2pcpp::StaticByteArray<0> sba;
    i2pcpp::ByteArray ba = i2pcpp::toByteArray(sba);
    BOOST_CHECK(ba.empty());
}

BOOST_AUTO_TEST_CASE(ToByteArray)
{
    i2pcpp::StaticByteArray<8> sba{1, 2, 3, 4, 5, 6, 7, 8};
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
    i2pcpp::StaticByteArray<8> sba{1, 2, 3, 4, 5, 6, 7, 8};
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

BOOST_AUTO_TEST_CASE(InvalidSizeThrows)
{
    const i2pcpp::ByteArray b{195, 254, 53, 68, 67, 1, 0};
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

struct RouterInfoFixture {
#include "SampleRI.inc"
    i2pcpp::RouterInfo ri;
    i2pcpp::ByteArrayConstItr it;

    RouterInfoFixture()
     : it(sample_routerInfo.begin()), ri(it, sample_routerInfo.end())
    {
    }
};

BOOST_AUTO_TEST_SUITE(RouterInfoTests)

BOOST_FIXTURE_TEST_CASE(Serialize, RouterInfoFixture)
{
    i2pcpp::ByteArray result = ri.serialize();
    BOOST_CHECK_EQUAL_COLLECTIONS(
        result.begin(), result.end(),
        sample_routerInfo.begin(), sample_routerInfo.end()
    );
}

BOOST_FIXTURE_TEST_CASE(Verify, RouterInfoFixture)
{
    BOOST_CHECK_EQUAL(ri.verifySignature(), true);
}

BOOST_FIXTURE_TEST_CASE(GetOptions, RouterInfoFixture)
{
    i2pcpp::Mapping mp = ri.getOptions();
    BOOST_CHECK_EQUAL(mp.getValue("netdb.knownLeaseSets"), "37");
    BOOST_CHECK_EQUAL(mp.getValue("caps"), "OfR");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(RouterIdentityTests)

BOOST_FIXTURE_TEST_CASE(GetHash, RouterInfoFixture)
{
    i2pcpp::StaticByteArray<32> sba = {
        214, 154, 116, 173, 5, 123, 132, 175, 215, 176, 184, 155, 242, 143, 0,
        29, 126, 248, 145, 97, 230, 70, 48, 236, 152, 58, 133, 237, 168, 160,
        100, 61 
    };
    BOOST_CHECK_EQUAL(ri.getIdentity().getHash(), sba);
}

BOOST_AUTO_TEST_SUITE_END()
