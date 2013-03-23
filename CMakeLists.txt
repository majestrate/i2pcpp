project(i2pcpp CXX C)
cmake_minimum_required(VERSION 2.8.9)

set(i2pcpp_VERSION_MAJOR 0)
set(i2pcpp_VERSION_MINOR 1)

set(PACKAGE_VERSION "v${i2pcpp_VERSION_MAJOR}.${i2pcpp_VERSION_MINOR}-")

set(PACKAGE_NAME i2pcpp)
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")

set(SRCS
	Database.cpp
	Log.cpp
	Router.cpp
	datatypes/Certificate.cpp
	datatypes/ByteArray.cpp
	datatypes/Datatype.cpp
	datatypes/Date.cpp
	datatypes/Endpoint.cpp
	datatypes/Mapping.cpp
	datatypes/RouterAddress.cpp
	datatypes/RouterHash.cpp
	datatypes/RouterIdentity.cpp
	datatypes/RouterInfo.cpp
	datatypes/SessionKey.cpp
	exceptions/FormattingError.cpp
	exceptions/RecordNotFound.cpp
	exceptions/StatementPrepareError.cpp
	transport/ssu/EstablishmentManager.cpp
	transport/ssu/EstablishmentState.cpp
	transport/ssu/InboundMessageFragments.cpp
	transport/ssu/InboundMessageState.cpp
	transport/ssu/Packet.cpp
	transport/ssu/PacketBuilder.cpp
	transport/ssu/PacketHandler.cpp
	transport/ssu/PeerState.cpp
	transport/ssu/PeerStateList.cpp
	transport/Transport.cpp
	transport/UDPTransport.cpp
	util/Base64.cpp
	util/I2PHMAC.cpp
	)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -O0 -Wall -std=c++11 -stdlib=libc++")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2 -w -std=c++11 -stdlib=libc++")

add_executable(i2p main.cpp ${SRCS})
add_executable(generate_keys tools/generate_keys.cpp)

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

# Botan
find_path(BOTAN_INCLUDE_DIR botan/botan.h PATHS "${BOTAN_INCLUDE_PREFIX}" NO_DEFAULT_PATH)
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

include_directories(BEFORE generate_keys ${BOTAN_INCLUDE_DIR})
target_link_libraries(generate_keys ${BOTAN_LIBRARY_PATH})

# Boost
find_package(Boost 1.53.0 REQUIRED COMPONENTS system thread filesystem chrono date_time log)
include_directories(BEFORE i2p ${Boost_INCLUDE_DIR})
target_link_libraries(i2p ${Boost_LIBRARIES})
add_definitions(-DBOOST_ALL_DYN_LINK)

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

	add_executable(testi2p tests/testi2p.cpp ${SRCS})
	target_link_libraries(testi2p ${GTEST_LINK_STUFF})
	add_test(
		NAME testi2p
		COMMAND testi2p
		)

	# ...
else(GTEST_LIBRARY_PATH AND GTEST_INCLUDE_DIR)
	message(STATUS "Google Testing Framework not found... not building tests")
endif(GTEST_LIBRARY_PATH AND GTEST_INCLUDE_DIR)


