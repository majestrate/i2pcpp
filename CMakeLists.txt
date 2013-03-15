project(i2pcpp CXX C)
cmake_minimum_required(VERSION 2.8.9)

set(i2pcpp_VERSION_MAJOR 0)
set(i2pcpp_VERSION_MINOR 1)

set(PACKAGE_VERSION "v${i2pcpp_VERSION_MAJOR}.${i2pcpp_VERSION_MINOR}-")

set(PACKAGE_NAME i2pcpp)
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")

set(SRCS
	datatypes/ByteArray.cpp
	datatypes/Datatype.cpp
	datatypes/Date.cpp
	datatypes/Endpoint.cpp
	datatypes/RouterHash.cpp
	exceptions/FormattingError.cpp
	)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -O0 -Wall -std=c++11 -stdlib=libc++")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2 -w -std=c++11 -stdlib=libc++")

add_executable(i2p main.cpp ${SRCS})

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
target_link_libraries(i2p "${BOTAN_LIBRARY_PATH}")

# Boost
find_package(Boost 1.52.0 REQUIRED COMPONENTS system)
include_directories(BEFORE i2p "${Boost_INCLUDE_DIRS}")
target_link_libraries(i2p "${Boost_SYSTEM_LIBRARIES}")

# Googletest
find_path(GTEST_INCLUDE_DIR gtest/gtest.h PATHS "${GTEST_INCLUDE_PREFIX}" NO_DEFAULT_PATH)
find_library(GTEST_LIBRARY_PATH gtest PATHS "${GTEST_LIBRARY_PREFIX}" NO_DEFAULT_PATH)
find_library(GTEST_MAIN_LIBRARY_PATH gtest_main PATHS "${GTEST_LIBRARY_PREFIX}" NO_DEFAULT_PATH)

if(GTEST_LIBRARY_PATH AND GTEST_INCLUDE_DIR)
	message(STATUS "Found gtest: ${GTEST_LIBRARY_PATH}")

	enable_testing()

	find_package(Threads)
	include_directories("${GTEST_INCLUDE_DIR}")
	set(GTEST_LINK_STUFF "${GTEST_LIBRARY_PATH}" "${GTEST_MAIN_LIBRARY_PATH}" "${CMAKE_THREAD_LIBS_INIT}" "${Boost_SYSTEM_LIBRARIES}" "${BOTAN_LIBRARY_PATH}")

	add_executable(testi2p tests/datatypes.cpp ${SRCS})
	target_link_libraries(testi2p ${GTEST_LINK_STUFF})
	add_test(
		NAME testi2p
		COMMAND testi2p
	)

  # ...
else(GTEST_LIBRARY_PATH AND GTEST_INCLUDE_DIR)
	message(STATUS "Google Testing Framework not found... not building tests")
endif(GTEST_LIBRARY_PATH AND GTEST_INCLUDE_DIR)


