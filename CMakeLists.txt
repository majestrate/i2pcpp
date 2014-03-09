project(i2pcpp CXX C)
cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/lib)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# --- EXTERNAL LIBRARIES ---

# pthreads
find_package(Threads)

# zlib
find_package(ZLIB REQUIRED)

# Boost
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.55.0 REQUIRED COMPONENTS system thread filesystem chrono date_time log program_options OPTIONAL_COMPONENTS unit_test_framework)

# Sqlite3
find_package(SQLite3 REQUIRED)

# Botan
find_package(Botan REQUIRED)

# --- INTERNAL COMPONENTS ---

# libs
add_subdirectory(lib)

# frontends
#add_subdirectory(frontends)

# tests
if(NOT DEFINED I2PCPP_SKIP_TESTS)
    add_subdirectory(tests)
    enable_testing()
    add_test(all "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/testi2p")
endif(NOT DEFINED I2PCPP_SKIP_TESTS)
