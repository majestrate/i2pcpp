#ifndef DHTSEARCHSTATE_H
#define DHTSEARCHSTATE_H

#include <memory>
#include <list>
#include <queue>
#include <set>

#include <boost/asio.hpp>

#include "../datatypes/RouterHash.h"

#include "Kademlia.h"

namespace i2pcpp {
	namespace DHT {
		struct SearchState {
			enum class CurrentState {
				CONNECTING,
				LOOKUP_SENT
			};

			Kademlia::key_type goal;
			RouterHash current;
			RouterHash next;
			std::list<RouterHash> excluded;
			std::queue<RouterHash> alternates;
			std::set<RouterHash> tried;
			CurrentState state = CurrentState::CONNECTING;
		};

		class PopAlternates {
			public:
				void operator()(SearchState &ss);
		};

		class PushAlternates {
			public:
				PushAlternates(RouterHash const &alt);

				void operator()(SearchState &ss);

			private:
				RouterHash m_alt;
		};

		class InsertTried {
			public:
				InsertTried(RouterHash const &rh);

				void operator()(SearchState &ss);

			private:
				RouterHash m_rh;
		};

		class ModifyState {
			private:
				enum class UpdateType {
					STATE,
					NEXT,
					NEW_CONNECTION
				};

			public:
				ModifyState(SearchState::CurrentState const state);
				ModifyState(RouterHash const &next);
				ModifyState(RouterHash const &current, RouterHash const &exclude);

				void operator()(SearchState &ss);

			private:
				UpdateType m_type;

				RouterHash m_current;
				RouterHash m_exclude;
				RouterHash m_next;
				SearchState::CurrentState m_state;
		};
	}
}

#endif
