#ifndef TUNNELMESSAGE_H
#define TUNNELMESSAGE_H

#include <array>

namespace i2pcpp {
	class TunnelMessage {
		public:
			TunnelMessage(std::array<unsigned char, 1024> const &data);

		private:
			std::array<unsigned char, 16> m_iv;
			std::array<unsigned char, 1008> m_data;
	};
}

#endif
