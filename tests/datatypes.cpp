#include "gtest/gtest.h"

#include "../exceptions/FormattingError.h"

#include "../datatypes/Date.h"
#include "../datatypes/Certificate.h"
#include "../datatypes/Mapping.h"
#include "../datatypes/RouterIdentity.h"
#include "../datatypes/RouterAddress.h"

#include "routerInfo.cpp"

TEST(Datatypes, Date) {
	using namespace i2pcpp;

	Date d1(3553611897179156816);
	ByteArray d1_serialized = d1.serialize();
	ByteArray expected_data = { 0x31, 0x50, 0xF7, 0x31, 0x50, 0xF7, 0x31, 0x50 };
	EXPECT_EQ(d1_serialized, expected_data);

	auto d1_itr = d1_serialized.cbegin();
	Date d2(d1_itr, d1_serialized.cend());
	ByteArray d2_serialized = d2.serialize();
	EXPECT_EQ(d2_serialized, expected_data);

	ByteArray bad_data = { 0x00, 0x01, 0x02 };
	auto bad_data_itr = bad_data.cbegin();
	ASSERT_THROW(Date(bad_data_itr, bad_data.cend()), FormattingError);

	// Check to see that it increments the iterator properly.
	ByteArray good_data = { 0x31, 0x50, 0xF7, 0x31, 0x50, 0xF7, 0x31, 0x50, 0x31, 0x50, 0xF7, 0x31, 0x50, 0xF7, 0x31, 0x50 };
	auto good_data_itr = good_data.cbegin();
	Date d3(good_data_itr, good_data.cend());
	Date d4(good_data_itr, good_data.cend());
	EXPECT_EQ(d3.serialize(), expected_data);
	EXPECT_EQ(d4.serialize(), expected_data);
}

TEST(Datatypes, Certificate) {
	using namespace i2pcpp;

	Certificate c1;
	ByteArray c1_serialized = c1.serialize();
	ByteArray expected_data = { 0x00, 0x00, 0x00 };
	ASSERT_EQ(c1_serialized, expected_data);

	ByteArray good_data = { 0x01, 0x00, 0x10, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x01, 0x00, 0x10, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
	auto good_data_itr = good_data.cbegin();
	Certificate c2(good_data_itr, good_data.cend());
	ASSERT_EQ(c2.getLength(), 0x10);

	Certificate c3(good_data_itr, good_data.cend());
	ASSERT_EQ(c3.serialize(), ByteArray(good_data.cbegin(), good_data.cbegin() + 0x13));

	ByteArray bad_data1 = { 0x05, 0x00, 0x00 };
	auto bad_data1_itr = bad_data1.cbegin();
	ASSERT_THROW(Certificate(bad_data1_itr, bad_data1.cend()), FormattingError);

	ByteArray bad_data2 = { 0x01, 0xDE, 0xAD, 0xBE, 0xEF };
	auto bad_data2_itr = bad_data2.cbegin();
	ASSERT_THROW(Certificate(bad_data2_itr, bad_data2.cend()), FormattingError);
}

TEST(Datatypes, Mapping) {
	using namespace i2pcpp;

	Mapping m1;
	m1.setValue("hello", "world");
	m1.setValue("foo", "bar");

	ByteArray m1_serialized = m1.serialize();
	ByteArray expected_data = { 0x00, 0x18, 0x03, 'f', 'o', 'o', '=', 0x03, 'b', 'a', 'r', ';', 0x05, 'h', 'e', 'l', 'l', 'o', '=', 0x05, 'w', 'o', 'r', 'l', 'd', ';' };

	ASSERT_EQ(m1_serialized, expected_data);

	auto m1_itr = m1_serialized.cbegin();
	Mapping m2(m1_itr, m1_serialized.cend());
	ASSERT_EQ(m2.getValue("hello"), std::string("world"));
	ASSERT_EQ(m2.getValue("foo"), std::string("bar"));

	m2.deleteValue("foo");
	ASSERT_EQ(m2.getValue("foo"), std::string());

	ByteArray bad = expected_data;
	bad[0] = 0xFF;
	bad[1] = 0xFF;
	auto bad_itr = bad.cbegin();
	ASSERT_THROW(Mapping(bad_itr, bad.cend()), FormattingError);

	bad = expected_data;
	bad[2] = 0x04;
	bad_itr = bad.cbegin();
	ASSERT_THROW(Mapping(bad_itr, bad.cend()), FormattingError);

	bad = expected_data;
	bad[7] = 0x04;
	bad_itr = bad.cbegin();
	ASSERT_THROW(Mapping(bad_itr, bad.cend()), FormattingError);
}

TEST(Datatypes, RouterIdentity) {
	using namespace i2pcpp;

	auto sample = sample_routerInfo.cbegin();
	RouterIdentity r1(sample, sample_routerInfo.cend());
	ASSERT_EQ(r1.getHashEncoded(), "1pp0rQV7hK~XsLib8o8AHX74kWHmRjDsmDqF7aigZD0=");

	ASSERT_EQ(r1.serialize(), ByteArray(sample_routerInfo.cbegin(), sample_routerInfo.cbegin() + 256 + 128 + 3));

	sample = sample_routerInfo.cbegin();
	ASSERT_THROW(RouterIdentity(sample, sample + 32), FormattingError);

	sample = sample_routerInfo.cbegin();
	ASSERT_THROW(RouterIdentity(sample, sample + 384), FormattingError);
}

TEST(Datatypes, RouterAddress) {
	using namespace i2pcpp;

	auto sample = sample_routerInfo.cbegin() + 256 + 128 + 3 + 8 + 1;
	RouterAddress r1(sample, sample_routerInfo.cend());

	sample = sample_routerInfo.cbegin() + 256 + 128 + 3 + 8 + 1;
	ASSERT_EQ(r1.serialize(), ByteArray(sample, sample + 48));
}
