/**
 * @file main.cpp
 * @brief Contains the starting point, main.
 */
#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <condition_variable>

#include <boost/filesystem.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

#include <i2pcpp/datatypes/RouterInfo.h>

/*#include "Router.h"
#include "Version.h"*/

static volatile bool quit = false;
static std::condition_variable cv;

static void sighandler(int sig)
{
    quit = true;
    cv.notify_all();
}

int main(int argc, char **argv)
{
    using namespace i2pcpp;
    using namespace std;

    /*try {
        string dbFile;

        Log::initialize();
        i2p_logger_mt lg(boost::log::keywords::channel = "M");
        Botan::LibraryInitializer init("thread_safe=true");

        if(signal(SIGINT, &sighandler) == SIG_ERR) {
            cerr << "error setting up signal handler" << endl;

            return EXIT_FAILURE;
        }

        namespace po = boost::program_options;

        po::options_description general("General options");
        general.add_options()
            ("help,h", "Print help")
            ("version,v", "Print application version")
            ("log,l", po::value<string>()->implicit_value("i2p.log"), "Log to file instead of clog");

        po::options_description db("Database manipulation");
        db.add_options()
            ("db", po::value<string>(&dbFile)->default_value("i2p.db"), "Database file to operate on")
            ("init", "Initialize a fresh copy of the database")
            ("import", po::value<string>(), "Import a single routerInfo file")
            ("export", po::value<string>(), "Export your routerInfo file")
            ("importdir", po::value<string>(), "Import all files in the given directory recursively")
            ("wipe", "Delete all stored routers and profiles");

        po::options_description config("Configuration manipulation");
        config.add_options()
            ("get", po::value<string>(), "Retrieve a configuration setting")
            ("set", po::value<vector<string>>()->multitoken(), "Set a configuration setting (key value)");

        po::options_description all_opts;
        all_opts.add(general).add(db).add(config);

        po::variables_map vm;

        po::store(po::command_line_parser(argc, argv).options(all_opts).run(), vm);
        po::notify(vm);

        if(vm.count("help")) {
            cout << general << endl;
            cout << db << endl;
            cout << config << endl;

            return EXIT_SUCCESS;
        }

        if(vm.count("version")) {
            cout << CLIENT_NAME + ' ' + CLIENT_BUILD << ' ' << CLIENT_DATE << endl;

            return EXIT_SUCCESS;
        }

        if(vm.count("init")) {
            Database::createDb(dbFile);

            I2P_LOG(lg, info) << "database created successfully";

            return EXIT_SUCCESS;
        }

        Router r(dbFile);

        if(vm.count("log")) {
            // TODO Log rotation, etc
            Log::logToFile(vm["log"].as<string>());
        }

        if(vm.count("export")) {
            string file = vm["export"].as<string>();
            ofstream f(file, ios::binary);

            if(f.is_open()) {
                ByteArray info = r.getRouterInfo();
                f.write((char *)info.data(), info.size());
                f.close();

                return EXIT_SUCCESS;
            } else {
                I2P_LOG(lg, fatal) << "error: could not open " << file << " for writing";

                return EXIT_FAILURE;
            }
        }

        if(vm.count("import")) {
            string file = vm["import"].as<string>();
            ifstream f(file, ios::binary);

            if(f.is_open()) {
                ByteArray ribytes = ByteArray((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
                f.close();
                auto begin = ribytes.cbegin();
                RouterInfo ri = RouterInfo(begin, ribytes.cend());
                r.importRouter(ri);
                I2P_LOG(lg, info) << "successfully imported RouterInfo file with hash " << ri.getIdentity().getHash();

                return EXIT_SUCCESS;
            } else {
                I2P_LOG(lg, fatal) << "error: could not open " << file << " for writing";

                return EXIT_FAILURE;
            }
        }

        if(vm.count("importdir")) {
            namespace fs = boost::filesystem;

            string dir = vm["importdir"].as<string>();
            if(!fs::exists(dir)) {
                I2P_LOG(lg, fatal) << "error: directory " << dir << " does not exist";

                return EXIT_FAILURE;
            }

            vector<RouterInfo> routers;
            fs::recursive_directory_iterator itr(dir), end;
            while(itr != end) {
                if(is_regular_file(*itr)) {
                    ifstream f(itr->path().string(), ios::binary);
                    if(!f.is_open()) continue;

                    ByteArray info = ByteArray((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
                    f.close();
                    I2P_LOG(lg, debug) << "importing " << itr->path().string();
                    auto begin = info.cbegin();
                    auto ri = RouterInfo(begin, info.cend());

                    if(ri.verifySignature())
                        routers.push_back(ri);
                    else
                        I2P_LOG(lg, error) << "failed to import " << itr->path().string() << ": bad signature";
                }

                if(fs::is_symlink(*itr)) itr.no_push();

                try {
                    ++itr;
                } catch(exception &e) {
                    itr.no_push();
                    continue;
                }
            }

            r.importRouter(routers);
            I2P_LOG(lg, info) << "successfully imported " << routers.size() << " routers";

            return EXIT_SUCCESS;
        }

        if(vm.count("wipe")) {
            r.deleteAllRouters();

            return EXIT_SUCCESS;
        }

        if(vm.count("get")) {
            cout << r.getConfigValue(vm["get"].as<string>()) << endl;

            return EXIT_SUCCESS;
        }

        if(!vm["set"].empty()) {
            vector<string> tokens;
            if((tokens = vm["set"].as<vector<string>>()).size() != 2) {
                I2P_LOG(lg, fatal) << "error: exactly two arguments must be provided to --set: '--set name value'";

                return EXIT_FAILURE;
            }

            r.setConfigValue(tokens[0], tokens[1]);

            return EXIT_SUCCESS;
        }

        I2P_LOG(lg, info) << "starting router";
        r.start();

        std::mutex mtx;
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return quit; });

        I2P_LOG(lg, debug) << "shutting down";

        r.stop();

        return EXIT_SUCCESS;
    } catch(boost::program_options::error &e) {
        cerr << "error parsing command line arguments: " << e.what() << endl;

        return EXIT_FAILURE;
    } catch(std::exception &e) {
        cerr << "error: " << e.what() << endl;

        return EXIT_FAILURE;
    }*/
}
