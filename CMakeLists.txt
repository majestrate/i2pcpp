project(i2pcpp CXX C)
cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/cmake)

<<<<<<< HEAD
set(TEST_SRCS
    dht/Kademlia.cpp
    exceptions/FormattingError.cpp
    exceptions/RecordNotFound.cpp
    tests/testi2p.cpp
    transport/ssu/EstablishmentState.cpp
    transport/ssu/Packet.cpp
    transport/ssu/PacketBuilder.cpp
    util/Base64.cpp
    util/I2PDH.cpp
    util/I2PHMAC.cpp
    )

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -stdlib=libc++")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -stdlib=libc++")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "-Oz")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-constexpr-not-const")
else("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    message(WARNING "Beware: Only clang is officially supported!")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

if("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    get_target_property(i2p_LOCATION i2p LOCATION)
    add_custom_command(
        TARGET i2p
        POST_BUILD
        COMMAND ${CMAKE_STRIP} -s ${i2p_LOCATION})
endif("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_executable(i2p main.cpp ${SRCS})

# Version information
add_custom_command(
  OUTPUT build.h
  COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/share/genbuild.sh" "${CMAKE_CURRENT_BINARY_DIR}/build.h"
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMENT "Generating version file")

add_custom_target(version ALL DEPENDS build.h)
add_definitions(-DHAVE_BUILD_INFO)
add_dependencies(i2p version)

# Database schema
add_custom_command(
  OUTPUT schema.sql.o
  COMMAND ${CMAKE_LINKER} -r -b binary -o ${CMAKE_CURRENT_BINARY_DIR}/schema.sql.o schema.sql
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/share
  COMMENT "Generating database schema object")

add_custom_target(schema ALL DEPENDS schema.sql.o)
add_dependencies(i2p schema)
target_link_libraries(i2p "${CMAKE_CURRENT_BINARY_DIR}/schema.sql.o")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
=======
# --- EXTERNAL LIBRARIES ---
>>>>>>> gitrepo/master

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

# zlib
find_package(ZLIB REQUIRED)
include_directories(BEFORE i2p ${ZLIB_INCLUDE_DIR}) 
target_link_libraries(i2p ${ZLIB_LIBRARIES})

# Botan
<<<<<<< HEAD
find_path(BOTAN_INCLUDE_DIR botan/botan.h PATHS "${BOTAN_INCLUDE_PREFIX}" /usr/include/botan-1.11 /usr/local/include/botan-1.11)
find_path(BOTAN_INCLUDE_DIR botan/botan.h)
find_library(BOTAN_LIBRARY_PATH botan-1.11 PATHS "${BOTAN_LIBRARY_PREFIX}" NO_DEFAULT_PATH)
find_library(BOTAN_LIBRARY_PATH botan-1.11)

if(BOTAN_LIBRARY_PATH AND BOTAN_INCLUDE_DIR)
    message(STATUS "Found botan: ${BOTAN_LIBRARY_PATH}")
else(BOTAN_LIBRARY_PATH AND BOTAN_INCLUDE_DIR)
    message(FATAL_ERROR "Botan 1.11 not found")
endif(BOTAN_LIBRARY_PATH AND BOTAN_INCLUDE_DIR)

include_directories(BEFORE i2p ${BOTAN_INCLUDE_DIR})
target_link_libraries(i2p ${BOTAN_LIBRARY_PATH})

# Boost
if("${NO_BOOST_LOG}" STREQUAL "1")
    message(STATUS "Not using Boost Log")
    find_package(Boost 1.49.0 REQUIRED COMPONENTS system thread filesystem chrono date_time program_options)
else("${NO_BOOST_LOG}" STREQUAL "1")
    message(STATUS "Using Boost Log")
    find_package(Boost 1.55.0 REQUIRED COMPONENTS system thread filesystem chrono date_time program_options log)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_BOOST_LOG")
endif("${NO_BOOST_LOG}" STREQUAL "1")

if(Boost_FOUND)
    set(Boost_USE_STATIC_LIBS OFF)
    set(Boost_USE_MULTITHREADED ON)
    set(Boost_USE_STATIC_RUNTIME OFF)
    include_directories(BEFORE i2p ${Boost_INCLUDE_DIRS})
    target_link_libraries(i2p ${Boost_LIBRARIES})
    add_definitions(-DBOOST_ALL_DYN_LINK)
else(Boost_FOUND)
    message(FATAL_ERROR "Boost not found")
endif(Boost_FOUND)
=======
find_package(Botan REQUIRED)
>>>>>>> gitrepo/master

# websocketpp
find_path(WEBSOCKETPP_INCLUDE_DIRS websocketpp/server.hpp PATHS "${WEBSOCKETPP_INCLUDEDIR}" NO_DEFAULT_PATH)
find_path(WEBSOCKETPP_INCLUDE_DIRS websocketpp/server.hpp)

# --- INTERNAL COMPONENTS ---

# libs
add_subdirectory(lib)

# i2pcpp
add_subdirectory(src)

<<<<<<< HEAD
# Setup datatypes library
add_subdirectory(datatypes)
include_directories(datatypes)
target_link_libraries(i2p datatypes)

# Lua
find_package(Lua)
if(LUA_FOUND)
    add_subdirectory(lua)
    include_directories(BEFORE i2p ${LUA_INCLUDE_DIR})
    target_link_libraries(i2p ${LUA_LIBRARIES})
endif(LUA_FOUND)
=======
# tests
if(NOT DEFINED I2PCPP_SKIP_TESTS)
    add_subdirectory(tests)
    enable_testing()
    add_test(all "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/testi2p")
endif(NOT DEFINED I2PCPP_SKIP_TESTS)
>>>>>>> gitrepo/master
