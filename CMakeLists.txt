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
	datatypes/Certificate.cpp
	datatypes/ByteArray.cpp
	datatypes/BuildRecord.cpp
	datatypes/BuildRequestRecord.cpp
	datatypes/BuildResponseRecord.cpp
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
	exceptions/SQLError.cpp
	exceptions/StatementPrepareError.cpp
	handlers/DatabaseSearchReply.cpp
	handlers/DatabaseStore.cpp
	handlers/DeliveryStatus.cpp
	handlers/Message.cpp
	handlers/TunnelGateway.cpp
	handlers/VariableTunnelBuild.cpp
	i2np/DatabaseLookup.cpp
	i2np/DatabaseSearchReply.cpp
	i2np/DatabaseStore.cpp
	i2np/DeliveryStatus.cpp
	i2np/Message.cpp
	i2np/TunnelGateway.cpp
	i2np/VariableTunnelBuild.cpp
	transport/ssu/AcknowledgementScheduler.cpp
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
	tunnel/Tunnel.cpp
	tunnel/TunnelHop.cpp
	tunnel/TunnelManager.cpp
	util/Base64.cpp
	util/I2PDH.cpp
	util/I2PHMAC.cpp
	)

set(TEST_SRCS
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
endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

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

find_path(BOTAN_INCLUDE_DIR botan/botan.h PATHS "${BOTAN_INCLUDE_PREFIX}" "/usr/local/include/botan-1.11" "/usr/include/botan-1.11")
find_path(BOTAN_INCLUDE_DIR botan/botan.h )


if(BOTAN_INCLUDE_DIR)
	message(STATUS "Found botan Include Files: ${BOTAN_INCLUDE_DIR}")
else(BOTAN_INCLUDE_DIR)
	message(FATAL_ERROR "Botan 1.11 Include Files not found")
endif(BOTAN_INCLUDE_DIR)

find_library(BOTAN_LIBRARY_PATH botan-1.11 PATHS "${BOTAN_LIBRARY_PREFIX}" NO_DEFAULT_PATH)
find_library(BOTAN_LIBRARY_PATH botan-1.11)


if(BOTAN_LIBRARY_PATH)
	message(STATUS "Found botan Library: ${BOTAN_LIBRARY_PATH}")
else(BOTAN_LIBRARY_PATH)
	message(FATAL_ERROR "Botan 1.11 Library not found")
endif(BOTAN_LIBRARY_PATH)


include_directories(BEFORE i2p ${BOTAN_INCLUDE_DIR})
target_link_libraries(i2p ${BOTAN_LIBRARY_PATH})

# Boost
find_package(Boost 1.53.0 REQUIRED COMPONENTS system thread filesystem chrono date_time program_options log)
include_directories(BEFORE i2p ${Boost_INCLUDE_DIR})
target_link_libraries(i2p ${Boost_LIBRARIES} )
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

	add_executable(testi2p ${TEST_SRCS})
	target_link_libraries(testi2p ${GTEST_LINK_STUFF})
	add_test(
		NAME testi2p
		COMMAND testi2p
		)
else(GTEST_LIBRARY_PATH AND GTEST_INCLUDE_DIR)
	message(STATUS "Google Testing Framework not found... not building tests")
endif(GTEST_LIBRARY_PATH AND GTEST_INCLUDE_DIR)

find_package(PythonLibs)

add_library(i2py SHARED py/i2py.cpp)

include_directories(BEFORE i2py ${PYTHON_INCLUDE_PATH})
include_directories(BEFORE i2py ${BOTAN_INCLUDE_PATH})
target_link_libraries(i2py ${BOTAN_LIBRARY_PATH})

GET_TARGET_PROPERTY(LIB_NAME i2py LOCATION)
SET(NEW_LIB_NAME i2py.so)

ADD_CUSTOM_COMMAND(
  TARGET i2py
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${LIB_NAME} ${NEW_LIB_NAME}
  )
ADD_CUSTOM_COMMAND(
  TARGET i2py
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${i2pcpp_SOURCE_DIR}/py/i2prouter.py i2prouter.py
  )

ADD_CUSTOM_COMMAND(
  TARGET i2py
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${i2pcpp_SOURCE_DIR}/doc/db_schema.sql db_schema.sql
)
