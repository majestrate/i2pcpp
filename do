#!/bin/bash
#
# automagic build script
# get botan and boost (with boost-log)
# build botan and boost
# build i2p
#
# author chisquare
#
# license GPL 3.0
#

set -e

#set -x

_e() # extract
{
		echo "extract $1 to $2"
    tmp="`mktemp -d`"
    cd $tmp
    case $1 in
				*.gz) tar -zxf $1 ;;
				*.bz2) tar -jxf $1 ;;
				*.zip) unzip $1 ;;
    esac
    cp -rf * $2
    rm -rf $tmp
    cd -
}

_ac_build() #build with autotools
{
    cd $1
    ./configure --prefix=$2 && make -j$jobs && make -j$jobs install 
    cd -
		
}

get_url() # download url
{
		echo "download $1 to $2"
    if [ "`which wget`" != "" ] ; then
				wget $1 -O $2 --no-check-certificate
    elif [ "`which curl`" != "" ] ; then
				curl $1 -o $2
		else
				echo "dont have wget or curl"
				exit 1
    fi
}

_build_botan() # build patched botan
{

		echo "Building Botan..."
		cpu='x86_64'
		if [ "$BUILD32" != "" ]; then
				cpu='x86'
		fi
    srcdir="$1"
    cd $srcdir
    python ./configure.py --prefix=$2 --with-zlib --cpu=$cpu
    make -j$jobs && echo "Installing Botan..." && make install -j$jobs
    cd -
}

_build_gtest() # build gtest
{
		echo "Building GTest..."
		mkdir -p $1/build
		cd $1/build
		cmake $1
		make -j$jobs
		cd -
		echo "Installing GTest"
		cp -a $1/build/lib*.a $2
}

_build_boost() # build boost with boost-log
{
		echo "Building Boost..."
    boostdir="$1"
    logdir="$2"
    prefix="$3"
    cp -rf $logdir/{boost,libs} $boostdir
    cd $boostdir
    ./bootstrap.sh --prefix=$prefix --with-libraries=system,thread,filesystem,program_options,log
    ./b2 -tx -j $jobs -a 
    cd -
		echo "Installing Boost..."
		cp -a $boostdir $prefix
}



_deps() # grab/build all dependancies
{
    i2psrc="$1"
    local base="$2"
		local prefix="$3"
		
		mkdir -p $prefix

    echo "Building Dependancies..."

		echo -n "Sqlite3..."
		if [ ! -d $base/sqlite3 ]; then
				echo "Not found"
				get_url https://www.sqlite.org/2013/sqlite-autoconf-3071700.tar.gz $base/sqlite3.tar.gz
				_e $base/sqlite3.tar.gz $base/sqlite3
		else
				echo "Found"
		fi

		if [ ! -e $base/sqlite3-built ]; then
				echo "Buidling Sqlite3"
				_ac_build $base/sqlite3 $prefix
				echo "`date`" > $base/sqlite3-built
		fi


		echo -n "GTest..."

		if [ ! -d $prefix/gtest ]; then
				echo "Not found"
				get_url https://googletest.googlecode.com/files/gtest-1.6.0.zip $base/gtest.zip
				_e $base/gtest.zip $prefix/gtest
		else
				echo "Found"
		fi
		
		if [ ! -e $base/gtest-built ]; then
				echo "Building Test"
				_build_gtest $prefix/gtest $prefix 
				echo "`date`" > $base/gtest-built
		fi
    
		echo -n "Botan..."
		if [ ! -d $base/botan ]; then
				echo "Not Found"
				get_url http://files.randombit.net/botan/v1.11/Botan-1.11.3.tbz $base/botan.tar.bz2
				_e $base/botan.tar.bz2 $base/botan
				cd $base/botan
				patch -p0 < $i2psrc/doc/botan-1.11.3.diff
				cd -
		else
				echo "Found"
    fi

    if [ ! -e $base/botan-built ]; then
				echo "Buiding Botan"
				_build_botan $base/botan $prefix
				echo "`date`" > $base/botan-built
		fi

		if [ "$BOOST_REBUILD" != "" ] ; then
				echo "nuking boost files so we can rebuild"
				rm -rf $base/boost{,*.tar.gz}
		fi
		
		echo -n "Boost.Log..."
    if [ ! -d $base/boost-log ]; then
				echo "Not Found"
				#get_url http://superb-dca2.dl.sourceforge.net/project/boost-log/boost-log-2.0-r862.zip $base/boost-log.zi[
				#e $base/boost-log.zip $base/boost-log
				svn checkout svn://svn.code.sf.net/p/boost-log/code/trunk/boost-log $base/boost-log
		else
				echo "Found... Updating"
				svn update $base/boost-log
    fi


		#boost_version="48"
		boost_version="54"
		echo -n "Boost vesrion 1.$boost_version.0..."
    if [ ! -d $base/boost ] ; then
				echo "Not Found"
				get_url "http://superb-dca3.dl.sourceforge.net/project/boost/boost/1.$boost_version.0/boost_1_$boost_version""_0.tar.gz" $base/boost.tar.gz 
				_e $base/boost.tar.gz $base/boost
		else
				echo "Found"
    fi

		if [ ! -e $base/boost-built ] ; then
				_build_boost $base/boost $base/boost-log $prefix
				echo "`date`" > $base/boost-built
		fi
}

_build_i2p() # build i2p itself
{

    echo "Building I2P..."
    base="$1"
    build="$2"
    prefix="$3"
		if [ "$REBUILD_I2P" != "" ] ; then
				echo "Nuke Last I2P Build..."
				rm -rf $build
		fi
    mkdir -p $build
    cmake="`which cmake`"
		if [ "$cmake" == "" ] ; then
				echo "No cmake!? WTF"
				exit 1
		fi
    cd $build
    $cmake \
				-DSQLITE3_INCLUDE_DIR=$prefix/include/ \
				-DSQLITE3_LIBRARY_PREFIX=$prefix/lib/ \
				-DBOTAN_INCLUDE_DIR=$prefix/include/botan-1.11/ \
				-DBOTAN_LIBRARY_PREFIX=$prefix/lib/ \
				-DBOOST_ROOT=$prefix/boost/ \
				-DBOOST_LIBRARY_PREFIX=$prefix/boost/stage/lib \
				-DBOOST_INCLUDE_DIR=$prefix/boost/ \
				-DGTEST_ROOT=$prefix/gtest/ \
				-DGTEST_LIBRARY=$prefix/libgtest.a \
				-DGTEST_MAIN_LIBRARY=$prefix/libgtest_main.a \
				-DCMAKE_CXX_COMPILER=$CXX \
				-DCMAKE_BUILD_TYPE="Debug" \
				$base
    make -j$jobs
}

_ensure_gcc() # make sure we have g++
{
		if [ "`which clang++`" == "" ] ; then
				if [ "`which g++`" == "" ] ; then
						echo "install g++"
						exit 1;
				fi
		fi
}

runit() # run the damn thing :3
{
		if [ "`which cmake`" == "" ]; then
				echo "No Cmake?"
				exit 1
		fi
		build_base=$2
    build="`readlink -f $build_base/build`"

		if [ "$REBUILD" != "" ] ; then
				echo "Nuke Old Build Directory"
				rm -rf $build
		fi
    mkdir -p $build

    prefix=$build/prefix
    mkdir -p $prefix

    export CC="`which cc`"
    export CXX="`which c++`"
    export jobs="$1"
    export MAKEOPTS="-j$jobs"
    
    
		_deps $build_base $build $prefix
    _build_i2p $build_base $build/i2p $prefix
		echo "`cat $build_base/README.CHI.TXT`"
}

if [ "$TRACE" != "" ] ; then
		set -x
fi

cd ~/src/i2pcpp

# run with 4 jobs
runit 4 $PWD


