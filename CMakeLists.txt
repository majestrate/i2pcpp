project(i2pcpp CXX C)
cmake_minimum_required(VERSION 2.8.9)

set(SRCS
    Database.cpp
    InboundMessageDispatcher.cpp
    Log.cpp
    OutboundMessageDispatcher.cpp
    PeerManager.cpp
    ProfileManager.cpp
    Router.cpp
    RouterContext.cpp
    Signals.cpp
    Version.cpp
    dht/Kademlia.cpp
    dht/SearchManager.cpp
    dht/SearchState.cpp
    exceptions/FormattingError.cpp
    exceptions/RecordNotFound.cpp
    exceptions/SQLError.cpp
    exceptions/StatementPrepareError.cpp
    handlers/DatabaseSearchReply.cpp
    handlers/DatabaseStore.cpp
    handlers/DeliveryStatus.cpp
    handlers/Message.cpp
    handlers/TunnelData.cpp
    handlers/TunnelGateway.cpp
    handlers/VariableTunnelBuild.cpp
    handlers/VariableTunnelBuildReply.cpp
    i2np/DatabaseLookup.cpp
    i2np/DatabaseSearchReply.cpp
    i2np/DatabaseStore.cpp
    i2np/DeliveryStatus.cpp
    i2np/Message.cpp
    i2np/Garlic.cpp
    i2np/TunnelData.cpp
    i2np/TunnelGateway.cpp
    i2np/VariableTunnelBuild.cpp
    i2np/VariableTunnelBuildReply.cpp
    transport/ssu/AcknowledgementManager.cpp
    transport/ssu/EstablishmentManager.cpp
    transport/ssu/EstablishmentState.cpp
    transport/ssu/InboundMessageFragments.cpp
    transport/ssu/InboundMessageState.cpp
    transport/ssu/OutboundMessageFragments.cpp
    transport/ssu/OutboundMessageState.cpp
    transport/ssu/Packet.cpp
    transport/ssu/PacketBuilder.cpp
    transport/ssu/PacketHandler.cpp
    transport/ssu/PeerState.cpp
    transport/ssu/PeerStateList.cpp
    transport/Transport.cpp
    transport/UDPTransport.cpp
    tunnel/InboundTunnel.cpp
    tunnel/OutboundTunnel.cpp
    tunnel/Tunnel.cpp
    tunnel/Fragment.cpp
    tunnel/FirstFragment.cpp
    tunnel/FollowOnFragment.cpp
    tunnel/FragmentHandler.cpp
    tunnel/FragmentState.cpp
    tunnel/TunnelHop.cpp
    tunnel/TunnelManager.cpp
    tunnel/Message.cpp
    util/Base64.cpp
    util/I2PDH.cpp
    util/I2PHMAC.cpp
    util/gzip.cpp
    )

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
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -stdlib=libc++")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -stdlib=libc++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

if("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    get_target_property(i2p_LOCATION i2p LOCATION)
    add_custom_command(
        TARGET i2p
        POST_BUILD
        COMMAND ${CMAKE_STRIP} -s ${i2p_LOCATION})
endif("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")

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

# zlib
find_path(ZLIB_INCLUDE_DIR zlib.h PATHS "${ZLIB_INCLUDE_PREFIX}" NO_DEFAULT_PATH)
find_path(ZLIB_INCLUDE_DIR zlib.h)
find_library(ZLIB_LIBRARY_PATH z PATHS "${ZLIB_LIBRARY_PREFIX}" NO_DEFAULT_PATH)
find_library(ZLIB_LIBRARY_PATH z)

if(ZLIB_LIBRARY_PATH AND ZLIB_INCLUDE_DIR)
    message(STATUS "Found zlib: ${ZLIB_LIBRARY_PATH}")
else(ZLIB_LIBRARY_PATH AND ZLIB_INCLUDE_DIR)
    message(FATAL_ERROR "zlib not found")
endif(ZLIB_LIBRARY_PATH AND ZLIB_INCLUDE_DIR)

include_directories(BEFORE i2p "${ZLIB_INCLUDE_DIR}")
target_link_libraries(i2p "${ZLIB_LIBRARY_PATH}")

# Sqlite3
find_path(SQLITE3_INCLUDE_DIR sqlite3.h PATHS "${SQLITE3_INCLUDE_PREFIX}" NO_DEFAULT_PATH)
find_path(SQLITE3_INCLUDE_DIR sqlite3.h)
find_library(SQLITE3_LIBRARY_PATH sqlite3 PATHS "${SQLITE3_LIBRARY_PREFIX}" NO_DEFAULT_PATH)
find_library(SQLITE3_LIBRARY_PATH sqlite3)

if(SQLITE3_LIBRARY_PATH AND SQLITE3_INCLUDE_DIR)
    message(STATUS "Found sqlite3: ${SQLITE3_LIBRARY_PATH}")
else(SQLITE3_LIBRARY_PATH AND SQLITE3_INCLUDE_DIR)
    message(FATAL_ERROR "Sqlite3 not found")
endif(SQLITE3_LIBRARY_PATH AND SQLITE3_INCLUDE_DIR)

include_directories(BEFORE i2p "${SQLITE3_INCLUDE_DIR}")
target_link_libraries(i2p "${SQLITE3_LIBRARY_PATH}")

# zlib
find_package(ZLIB REQUIRED)
include_directories(BEFORE i2p ${ZLIB_INCLUDE_DIR}) 
target_link_libraries(i2p ${ZLIB_LIBRARIES})

# Botan
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
    include_directories(BEFORE i2p ${Boost_INCLUDE_DIRS})
    target_link_libraries(i2p ${Boost_LIBRARIES})
    add_definitions(-DBOOST_ALL_DYN_LINK)
else(Boost_FOUND)
    message(FATAL_ERROR "Boost not found")
endif(Boost_FOUND)

add_subdirectory(datatypes)
include_directories(datatypes)
target_link_libraries(i2p datatypes)

# Googletest
find_path(GTEST_INCLUDE_DIR gtest/gtest.h PATHS "${GTEST_INCLUDE_PREFIX}" NO_DEFAULT_PATH)
find_library(GTEST_LIBRARY_PATH gtest PATHS "${GTEST_LIBRARY_PREFIX}" NO_DEFAULT_PATH)
find_library(GTEST_MAIN_LIBRARY_PATH gtest_main PATHS "${GTEST_LIBRARY_PREFIX}" NO_DEFAULT_PATH)

if(GTEST_LIBRARY_PATH AND GTEST_INCLUDE_DIR)
    message(STATUS "Found gtest: ${GTEST_LIBRARY_PATH}")

    enable_testing()

    find_package(Threads)
    include_directories("${GTEST_INCLUDE_DIR}")
    set(GTEST_LINK_STUFF "${GTEST_LIBRARY_PATH}" "${GTEST_MAIN_LIBRARY_PATH}" "${CMAKE_THREAD_LIBS_INIT}" "${Boost_LIBRARIES}" "${BOTAN_LIBRARY_PATH}")

    add_executable(testi2p ${TEST_SRCS})
    target_link_libraries(testi2p ${GTEST_LINK_STUFF} datatypes)
    add_test(
        NAME testi2p
        COMMAND testi2p
        )
else(GTEST_LIBRARY_PATH AND GTEST_INCLUDE_DIR)
    message(STATUS "Google Testing Framework not found... not building tests")
endif(GTEST_LIBRARY_PATH AND GTEST_INCLUDE_DIR)
