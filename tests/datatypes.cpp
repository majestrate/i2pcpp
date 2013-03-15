#include "gtest/gtest.h"

#include "../datatypes/Date.h"
#include "../exceptions/FormattingError.h"

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
}
