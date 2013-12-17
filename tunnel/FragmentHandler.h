#ifndef FRAGMENTHANDLER_H
#define FRAGMENTHANDLER_H

#include <unordered_map>
#include <list>

#include "FragmentState.h"

namespace i2pcpp {
	class FragmentHandler {
		public:
			void receiveFragments(std::list<FragmentPtr> const &f);

		private:
			std::unordered_map<uint32_t, FragmentState> m_states;
	};
}

#endif
