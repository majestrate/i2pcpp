/**
 * @file Router.h
 * @brief Defines the i2pcpp::Router class.
 */
#ifndef ROUTER_H
#define ROUTER_H

#include <i2pcpp/datatypes/ByteArray.h>

#include <string>

namespace i2pcpp {
    class RouterInfo;
    class Transport;
    class Database;
    struct Callbacks;

    /**
     * Represents an I2P router.
     */
    class Router {
        public:
            /**
             * Constructs a router.
             * @param db a valid i2pcpp::Database object.
             * @param cb a struct containing the callbacks for various events.
             */
            Router(std::shared_ptr<Database> const &db, Callbacks const &cb);
            Router(const Router &) = delete;
            Router& operator=(Router &) = delete;
            ~Router();

            /**
             * Initializes various global states. Call this once and only once
             * per application, regardless of how many routers you start.
             */
            static void initialize();

            /**
             * Registers a transport with libi2p. The transport must already have
             * been initialized properly.
             */
            void addTransport(std::shared_ptr<Transport> t);

            /**
             * Starts the i2pcpp::Router.
             * That is, runs the boost::asio I/O service, registers the handlers,
             *  binds the signals, etc.
             */
            void start();

            /**
             * Stops the i2pcpp::Router.
             * That is, stops the control server and I/O service.
             */
            void stop();

        private:
            struct RouterImpl;
            std::unique_ptr<RouterImpl> m_impl;
    };
}

#endif
