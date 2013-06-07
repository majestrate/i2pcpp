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

set +e

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
    mkdir -p $2
    cd $2
    $1/configure --prefix=$3 && make && make install
    cd -
}

get_url() # download url
{
    if [ "`which wget`" != "" ] ; then
	wget $1 -O $2
    elif [ "`which curl`" != "" ] ; then
	curl $1 -o $2
    fi
}

_botan_build() # build patched botan
{
    srcdir="$1"
    cd $srcdir
    python ./configure.py --prefix=$2 --with-zlib
    make -j$jobs && make install -j$jobs
    cd -
}

_build_boost() # build boost with boost-log
{
    boostdir="$1"
    logdir="$2"
    prefix="$3"
    cp -rf $logdir/{boost,libs} $boostdir
    cd $boostdir
    ./bootstrap.sh --prefix=$prefix
    ./b2 -tx -j $jobs -a 
    cd -
    cp -av $boostdir $prefix
    
}

get_deps() # grab all dependancies
{
    i2psrc="$1"
    local base="$2"
    echo "Downloading Dependancies..."
    if [ ! -d $base/botan ]; then
	get_url http://files.randombit.net/botan/v1.11/Botan-1.11.3.tbz $base/botan.tar.bz2
	_e $base/botan.tar.bz2 $base/botan
	cd $base/botan
	patch -p0 < $i2psrc/doc/botan-1.11.3.diff
	cd -
    fi
    
    if [ ! -d $base/boost ]; then	
	get_url http://superb-dca3.dl.sourceforge.net/project/boost/boost/1.53.0/boost_1_53_0.tar.gz $base/boost.tar.gz
	_e $base/boost.tar.gz $base/boost
    fi
    
    if [ ! -d $base/boost-log ]; then
	get_url http://superb-dca2.dl.sourceforge.net/project/boost-log/boost-log-2.0-r862.zip $base/boost-log.zip
	_e $base/boost-log.zip $base/boost-log
    fi

}

build_deps() # build dependancies
{
    base="$1"
    src="$2"
    echo "Build Dependancies..."
    prefix="$3"
    
    echo "Building Botan..."
    _botan_build $src/botan $prefix

    echo "Building Boost..."
    _build_boost $src/boost $src/boost-log $prefix

}

build_i2p() # build i2p itself
{
    base="$1"
    build="$2"
    prefix="$3"
    rm -rf $build
    mkdir $build
    cmake="`which cmake`"
    echo "Building I2P..."
    cd $build
    $cmake  -DBOTAN_INCLUDE_DIR=$prefix/include/botan-1.11 -DBOTAN_LIBRARY_PREFIX=$prefix/lib -DBOOST_ROOT=$prefix/boost/ $base
    make -j$jobs
}
ensure_gcc() # make sure we have g++
{
    if [ "`which g++`" == "" ] ; then
	echo "install g++"
	exit 1;
    fi
}

runit() # run the damn thing :3
{
    base=$PWD
    build="`readlink -f $base/build`"
    mkdir -p $build

    prefix=$build/prefix
    mkdir -p $prefix
    
    local t=$build/tmp
    mkdir -p $t

    ensure_gcc

    export CC="`which cc`"
    export CXX="`which c++`"
    export jobs="$1"
    export MAKEOPTS="-j$jobs"
    mkdir -p $t
    get_deps $base $t
    mkdir -p $prefix
    build_deps $base $t $prefix
    build_i2p $base $build/i2p $prefix
}

runit 4
