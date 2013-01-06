project(i2pcpp CXX)
cmake_minimum_required(VERSION 2.8.9)

set(i2pcpp_VERSION_MAJOR 0)
set(i2pcpp_VERSION_MINOR 1)

set(PACKAGE_VERSION "v${i2pcpp_VERSION_MAJOR}.${i2pcpp_VERSION_MINOR}-")

set(PACKAGE_NAME i2pcpp)
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")

set(SRCS
	Database.cpp
	i2p.cpp
	InboundMessageDispatcher.cpp
	Job.cpp
	JobQueue.cpp
	JobRunner.cpp
	JobScheduler.cpp
	MessageHandler.cpp
	OutboundMessageDispatcher.cpp
	Router.cpp
	RouterContext.cpp
	Thread.cpp
	Transport.cpp
	handlers/DatabaseStore.cpp
	handlers/DeliveryStatus.cpp
	i2np/DatabaseStore.cpp
	i2np/DeliveryStatus.cpp
	i2np/Message.cpp
	jobs/DatabaseStore.cpp
	jobs/MessageHandlerJob.cpp
	ssu/EstablishmentManager.cpp
	ssu/EstablishmentState.cpp
	ssu/InboundMessageFragments.cpp
	ssu/InboundMessageState.cpp
	ssu/MessageSender.cpp
	ssu/OutboundMessageState.cpp
	ssu/PacketHandler.cpp
	ssu/PeerState.cpp
	ssu/MessageReceiver.cpp
	ssu/Packet.cpp
	ssu/PacketBuilder.cpp
	ssu/UDPReceiver.cpp
	ssu/UDPSender.cpp
	ssu/UDPTransport.cpp
	util/Base64.cpp
	util/I2PHMAC.cpp
	util/LockingQueue.cpp
	datatypes/RouterInfo.cpp
	datatypes/Certificate.cpp
	datatypes/RouterAddress.cpp
	datatypes/Endpoint.cpp
	datatypes/Mapping.cpp
	datatypes/Date.cpp
	datatypes/RouterIdentity.cpp
	datatypes/RouterHash.cpp
	datatypes/SessionTag.cpp
	datatypes/ByteArray.cpp
	datatypes/Datatype.cpp
	datatypes/SessionKey.cpp
	)

set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -O0 -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -w")

add_definitions("-std=c++11")
add_definitions("-stdlib=libc++")

add_executable(i2p ${SRCS})
add_executable(generate_encryption_key tools/generate_encryption_key.cpp)
add_executable(generate_signing_key tools/generate_signing_key.cpp)

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

include_directories(BEFORE i2p "${BOTAN_INCLUDE_DIR}")
include_directories(BEFORE generate_encryption_key "${BOTAN_INCLUDE_DIR}")
include_directories(BEFORE generate_signing_key "${BOTAN_INCLUDE_DIR}")
target_link_libraries(i2p "${BOTAN_LIBRARY_PATH}")
target_link_libraries(generate_encryption_key "${BOTAN_LIBRARY_PATH}")
target_link_libraries(generate_signing_key "${BOTAN_LIBRARY_PATH}")

# Boost
find_package(Boost 1.52.0 REQUIRED COMPONENTS system)
include_directories(BEFORE i2p "${Boost_INCLUDE_DIRS}")
target_link_libraries(i2p "${Boost_LIBRARIES}")
