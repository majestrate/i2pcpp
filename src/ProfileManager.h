/**
 * @file ProfileManager.h
 * @brief Defines the i2pcpp::ProfileManager
 */
#ifndef PROFILEMANAGER_H
#define PROFILEMANAGER_H

#include <i2pcpp/datatypes/RouterInfo.h>

namespace i2pcpp {
    class RouterContext;

    /**
     * Manages peer profiles. Allows random peer selection.
     */
    class ProfileManager {
        public:
            /**
             * Constructs from a reference to the i2pcpp::RouterContext.
             */
            ProfileManager(RouterContext &ctx);
            ProfileManager(const ProfileManager &) = delete;
            ProfileManager& operator=(ProfileManager &) = delete;

            /**
             * Randomly selects a peer and returns its RI.
             * @return the i2pcpp::RouterInfo structure of the peer
             */
            const RouterInfo getPeer();

        private:
            RouterContext& m_ctx; ///< Reference to the router context
    };
}

#endif
