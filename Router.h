/**
 * @file Router.h
 * @brief Defines the i2pcpp::Router class.
 */
#ifndef ROUTER_H
#define ROUTER_H

#include <boost/asio.hpp>

#include <string>
#include <thread>

#include "datatypes/ByteArray.h"
#include "control/Server.h"

#include "Log.h"
#include "RouterContext.h"

namespace i2pcpp {

    /**
     * Represents an I2P router. 
     */
    class Router {
        public:
            /**
             * Constructs a router from a database file.
             * @param dbFile the name of the datatabase file
             */
            Router(std::string const &dbFile);
            Router(const Router &) = delete;
            Router& operator=(Router &) = delete;
            ~Router();

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

            /**
             * @return the i2pcpp::RouterInfo as an i2pcpp::ByteArray.
             * @todo move this to somewhere else
             */
            ByteArray getRouterInfo();

            /**
             * Imports a known i2pcpp::RouterInfo into the database of this
             *  i2pcpp::Router. 
             */
            void importRouter(RouterInfo const &router);

            /**
             * Imports a std::vector of known i2pcpp::RouterInfo into the
             *  database of this i2pcpp::Router. 
             */
            void importRouter(std::vector<RouterInfo> const &routers);

            /**
             * Deletes all known routers given by their i2pcpp::RouterInfo
             *  from the database.
             */
            void deleteAllRouters();

            /**
             * Changes a configuration value.
             * @param key the key of the value
             * @param value the new value 
             */
            void setConfigValue(const std::string& key, const std::string& value);

            /**
             * @return the configuration value at a given \a key
             * @param key the key of the value
             */
            std::string getConfigValue(const std::string& key);

        private:
            boost::asio::io_service m_ios;
            boost::asio::io_service::work m_work;
            std::thread m_serviceThread;

            std::unique_ptr<Control::Server> m_controlServer = nullptr;

            RouterContext m_ctx;

            i2p_logger_mt m_log; ///< Logging object
    };
}

#endif
