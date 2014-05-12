#include <lib/i2p/dht/SearchState.h>
#include <random>
#include <boost/test/unit_test.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>

using namespace i2pcpp;

BOOST_AUTO_TEST_SUITE(SearchStateTests)

BOOST_AUTO_TEST_CASE(IsAlternateEmpty)
{
    DHT::SearchState ss(DHT::Kademlia::makeKey(RouterHash()), RouterHash());
    BOOST_CHECK(!ss.isAlternate(RouterHash()));
}


BOOST_AUTO_TEST_CASE(IsAlternateOne)
{
    DHT::SearchState ss(DHT::Kademlia::makeKey(RouterHash()), RouterHash());
    RouterHash rh;
    rh.fill(0x01);
    ss.addAlternate(rh);
    BOOST_CHECK(ss.isAlternate(rh));
}

BOOST_AUTO_TEST_CASE(IsTriedEmpty)
{
    DHT::SearchState ss(DHT::Kademlia::makeKey(RouterHash()), RouterHash());
    BOOST_CHECK(ss.isTried(RouterHash()));
}


BOOST_AUTO_TEST_CASE(CountAlternatesEmpty)
{
    DHT::SearchState ss(DHT::Kademlia::makeKey(RouterHash()), RouterHash());
    BOOST_CHECK_EQUAL(ss.countAlternates(), 0);
}


BOOST_AUTO_TEST_CASE(CountAlternatesRand)
{
    std::default_random_engine rng;
    for(int i = 0; i < 50; ++i) {
        DHT::SearchState ss(DHT::Kademlia::makeKey(RouterHash()), RouterHash());
        std::uniform_int_distribution<unsigned> dist(1, 250);
        const unsigned c = dist(rng);
        for(int j = 1; j <= c; ++j) {
            RouterHash rh;
            rh.fill(j);
            ss.addAlternate(rh);
        }
        BOOST_CHECK_EQUAL(ss.countAlternates(), c);
    }
}

BOOST_AUTO_TEST_CASE(PopOne)
{
    DHT::SearchState ss(DHT::Kademlia::makeKey(RouterHash()), RouterHash());
    RouterHash rh;
    rh.fill(0x01);
    ss.addAlternate(rh);
    ss.popAlternate();
    BOOST_CHECK(ss.isTried(rh));
    BOOST_CHECK(!ss.isAlternate(rh));
}


BOOST_AUTO_TEST_CASE(CountAlternatesAfterPopRand)
{
    std::default_random_engine rng;
    for(int i = 0; i < 50; ++i) {
        DHT::SearchState ss(DHT::Kademlia::makeKey(RouterHash()), RouterHash());
        std::uniform_int_distribution<unsigned> dist(1, 250);
        const unsigned c = dist(rng);
        for(int j = 1; j <= c; ++j) {
            RouterHash rh;
            rh.fill(j);
            ss.addAlternate(rh);
        }

        std::uniform_int_distribution<unsigned> dist_pop(0, c);
        const unsigned c_pop = dist_pop(rng);
        for(int j = 0; j < c_pop; ++j)
            ss.popAlternate();

        BOOST_CHECK_EQUAL(ss.countAlternates(), c - c_pop);
    }
}

BOOST_AUTO_TEST_CASE(CountAlternatesAfterPushPopRand)
{
    std::default_random_engine rng;
    for(int i = 0; i < 50; ++i) {
        DHT::SearchState ss(DHT::Kademlia::makeKey(RouterHash()), RouterHash());
        std::uniform_int_distribution<unsigned> dist(1, 200);
        for(int j = 0; j < dist(rng); ++j) {
            RouterHash rh;
            rh.fill(j);
            ss.addAlternate(rh);
            ss.popAlternate();
        }

        BOOST_CHECK_EQUAL(ss.countAlternates(), 0);
    }
    DHT::SearchState ss(DHT::Kademlia::makeKey(RouterHash()), RouterHash());
}

BOOST_AUTO_TEST_CASE(GetNextOne)
{
    DHT::SearchState ss(DHT::Kademlia::makeKey(RouterHash()), RouterHash());
    RouterHash rh;
    rh.fill(0x01);
    ss.addAlternate(rh);
    BOOST_CHECK(ss.getNext() == rh);
}

namespace bmi = boost::multi_index;
typedef boost::multi_index_container<
    DHT::SearchState,
    bmi::indexed_by<
        bmi::hashed_unique<
            BOOST_MULTI_INDEX_MEMBER(DHT::SearchState, DHT::Kademlia::key_type, goal)
        >,
        bmi::hashed_non_unique<
            BOOST_MULTI_INDEX_MEMBER(DHT::SearchState, RouterHash, current)
        >
    >
> SearchStateContainer;

BOOST_AUTO_TEST_CASE(StateContainerCountKey)
{
    SearchStateContainer ssc;
    const auto k = DHT::Kademlia::makeKey(RouterHash());
    DHT::SearchState ss(k, RouterHash());
    ssc.insert(ss);
    BOOST_CHECK_EQUAL(ssc.get<0>().count(k), 1);
}

BOOST_AUTO_TEST_CASE(StateContainerCountHash)
{
    SearchStateContainer ssc;
    RouterHash rh;
    rh.fill(0x00);
    DHT::SearchState ss(DHT::Kademlia::makeKey(RouterHash()), rh);
    ssc.insert(ss);
    BOOST_CHECK_EQUAL(ssc.get<1>().count(rh), 1);
}

BOOST_AUTO_TEST_SUITE_END()
