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
    tmp="`mktemp -d`"
    cd $tmp
    case $1 in
				*.gz) tar -zxf $1 ;;
				*.bz2) tar -jxf $1 ;;
				*.zip) unzip $1 ;;
    esac
    mv * $2
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
    if [ "`which wget`" != "" ] ; then
				wget $1 -O $2 --no-check-certificate
    elif [ "`which curl`" != "" ] ; then
				curl $1 -o $2
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
		cd $1
		./configure --prefix=$2 && make -j$jobs 
		cd -
		echo "Installing GTest"
		cp -a $1/libs/libgtest_main.la $2/lib
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

		if [ ! -d $base/sqlite3 ]; then
				get_url https://www.sqlite.org/2013/sqlite-autoconf-3071700.tar.gz $base/sqlite3.tar.gz
				_e $base/sqlite3.tar.gz $base/sqlite3
		fi

		if [ ! -e $base/sqlite3-built ]; then
				_ac_build $base/sqlite3 $prefix
				echo "`date`" > $base/sqlite3-built
		fi

		#if [ ! -d $base/gtest ]; then
		#		get_url https://googletest.googlecode.com/files/gtest-1.6.0.zip $base/gtest.zip
		#		_e $base/gtest.zip $base/gtest
		#fi
		
		#if [ ! -e $base/gtest-built ]; then
		#		_build_gtest $base/gtest $prefix
		#		echo "`date`" > $base/gtest-built
		#fi
    
		if [ ! -d $base/botan ]; then
				get_url http://files.randombit.net/botan/v1.11/Botan-1.11.3.tbz $base/botan.tar.bz2
				_e $base/botan.tar.bz2 $base/botan
				cd $base/botan
				patch -p0 < $i2psrc/doc/botan-1.11.3.diff
				cd -
    fi

    if [ ! -e $base/botan-built ]; then
				_build_botan $base/botan $prefix
				echo "`date`" > $base/botan-built
		fi

    if [ ! -d $base/boost-log ]; then
				get_url http://superb-dca2.dl.sourceforge.net/project/boost-log/boost-log-2.0-r862.zip $base/boost-log.zip
				_e $base/boost-log.zip $base/boost-log
    fi

    if [ ! -d $base/boost ]; then	
				get_url http://superb-dca3.dl.sourceforge.net/project/boost/boost/1.53.0/boost_1_53_0.tar.gz $base/boost.tar.gz
				_e $base/boost.tar.gz $base/boost
    fi

		if [ ! -e $base/boost-built ] ; then
				_build_boost $base/boost $base/boost-log $prefix
				echo "`date`" > $base/boost-built
		fi
}

_build_i2p() # build i2p itself
{
    base="$1"
    build="$2"
    prefix="$3"
		#echo "Removing Last build..."
    #rm -rf $build
    mkdir -p $build
    cmake="`which cmake`"
    echo "Building I2P..."
    cd $build
    $cmake \
				-DSQLITE3_INCLUDE_DIR=$prefix/include/ \
				-DSQLITE3_LIBRARY_PREFIX=$prefix/lib/ \
				-DBOTAN_INCLUDE_DIR=$prefix/include/botan-1.11/ \
				-DBOTAN_LIBRARY_PREFIX=$prefix/lib/ \
				-DBOOST_ROOT=$prefix/boost/ \
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
    base=$PWD
		build_base=$2
    build="`readlink -f $build_base/build`"
    mkdir -p $build

    prefix=$build/prefix
    mkdir -p $prefix

    export CC="`which cc`"
    export CXX="`which c++`"
    export jobs="$1"
    export MAKEOPTS="-j$jobs"
    
    
		_deps $base $build $prefix
    _build_i2p $base $build/i2p $prefix
		echo "`cat $base/README.CHI.TXT`"
}

# run with 4 jobs
runit 4 $PWD


