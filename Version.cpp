/* The contents of this file are adapted from a
 * similar file used in the bitcoin-qt client.
 */

#define CLIENT_VERSION_MAJOR        0
#define CLIENT_VERSION_MINOR        0
#define CLIENT_VERSION_REVISION    0
#define CLIENT_VERSION_BUILD        0
#define CLIENT_VERSION_IS_RELEASE false
#define CLIENT_VERSION_SUFFIX        "-alpha"

#define STRINGIZE(X) DO_STRINGIZE(X)
#define DO_STRINGIZE(X) #X

#ifdef HAVE_BUILD_INFO
#    include "build.h"
#endif

//$Format:%n#define GIT_ARCHIVE 1$
#ifdef GIT_ARCHIVE
#    define GIT_COMMIT_ID "$Format:%h$"
#    define GIT_COMMIT_DATE "$Format:%cD"
#endif

#define BUILD_DESC_FROM_COMMIT(maj,min,rev,build,commit) \
    "v" DO_STRINGIZE(maj) "." DO_STRINGIZE(min) "." DO_STRINGIZE(rev) "." DO_STRINGIZE(build) "-g" commit

#define BUILD_DESC_FROM_UNKNOWN(maj,min,rev,build) \
    "v" DO_STRINGIZE(maj) "." DO_STRINGIZE(min) "." DO_STRINGIZE(rev) "." DO_STRINGIZE(build) "-unk"

#ifndef BUILD_DESC
#    ifdef GIT_COMMIT_ID
#        define BUILD_DESC BUILD_DESC_FROM_COMMIT(CLIENT_VERSION_MAJOR, CLIENT_VERSION_MINOR, CLIENT_VERSION_REVISION, CLIENT_VERSION_BUILD, GIT_COMMIT_ID)
#    else
#        define BUILD_DESC BUILD_DESC_FROM_UNKNOWN(CLIENT_VERSION_MAJOR, CLIENT_VERSION_MINOR, CLIENT_VERSION_REVISION, CLIENT_VERSION_BUILD)
#    endif
#endif

#ifndef BUILD_DATE
#    ifdef GIT_COMMIT_DATE
#        define BUILD_DATE GIT_COMMIT_DATE
#    else
#        define BUILD_DATE __DATE__ ", " __TIME__
#    endif
#endif

#include "Version.h"

namespace i2pcpp {
    const int CLIENT_VERSION =
           1000000 * CLIENT_VERSION_MAJOR
        +   10000 * CLIENT_VERSION_MINOR
        +   100 * CLIENT_VERSION_REVISION
        +   1 * CLIENT_VERSION_BUILD;

    const std::string CLIENT_NAME("i2pcpp");
    const std::string CLIENT_BUILD(BUILD_DESC CLIENT_VERSION_SUFFIX);

    const std::string CLIENT_DATE(BUILD_DATE);
}
