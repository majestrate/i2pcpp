#ifndef BASE64_H
#define BASE64_H

#include <string>

#include "../datatypes/ByteArray.h"

using namespace std;

namespace i2pcpp {

	class Base64 {
		public:
			static ByteArray decode(string const &s);
			static string encode(ByteArray const &b);

		private:
			static bool is_base64(unsigned char c);
			static const string b64chars;
	};
}

#endif
