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
    class Database;

    /**
     * Represents an I2P router.
     */
    class Router {
        public:
            /**
             * Constructs a router from a database file.
             * @param dbFile the name of the datatabase file
             */
            Router(std::shared_ptr<Database> const &db);
            Router(const Router &) = delete;
            Router& operator=(Router &) = delete;
            ~Router();

            /**
             * Initializes various global states. Call this once and only once
             * per application, regardless of how many routers you start.
             */
            static void initialize();

            /**
             * Starts the i2pcpp::Router.
             * That is, runs the boost::asio I/O service, registers the handlers,
             *  binds the signals and creates an i2pcpp::UDPTransport object.
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
