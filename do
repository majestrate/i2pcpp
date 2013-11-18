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

_deps() # grab/build all dependancies
{
    i2psrc="$1"
    local base="$2"
		local prefix="$3"
		
		mkdir -p $prefix

    echo "Building Dependancies..."

		echo -n "Botan..."
		if [ ! -d $base/botan ]; then
				echo "Not Found"
				get_url http://files.randombit.net/botan/v1.11/Botan-1.11.4.tbz $base/botan.tar.bz2
				_e $base/botan.tar.bz2 $base/botan
				cd $base/botan
				patch -p1 < $i2psrc/doc/botan-1.11.4.diff
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
				-DBOTAN_INCLUDE_DIR=$prefix/include/botan-1.11/ \
				-DBOTAN_LIBRARY_PREFIX=$prefix/lib/ \
				-DCMAKE_CXX_COMPILER=$CXX \
				-DCMAKE_BUILD_TYPE="Debug" \
						-DCMAKE_CXX_FLAGS="-std=c++11 -Os" \
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
		#echo "`cat $build_base/README.CHI.TXT`"
}

if [ "$TRACE" != "" ] ; then
		set -x
fi

# run with 4 jobs
runit 4 $PWD


