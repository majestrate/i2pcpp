#ifndef FRAGMENTSTATE_H
#define FRAGMENTSTATE_H

#include <list>

#include "Fragment.h"

namespace i2pcpp {
	class FragmentState {
		public:
		private:
			std::list<FragmentPtr> m_fragments;
	};
}

#endif
