#!/bin/bash

# This script compiles the peak linux driver with chardev support and installs it.
# Note that you need sudo rights to execute this.

PKG_NAME="peak-linux-driver"
PKG_VER=7.15.2

TARBALL=${PKG_NAME}-${PKG_VER}.tar.gz
TARBALL_URL=http://www.peak-system.com/fileadmin/media/linux/files/${TARBALL}

KERNEL_VER=/usr/src/linux-headers-"`uname -r`"

BUILD_DIR=/tmp/build-pcan
SOURCE_DIR=$BUILD_DIR/$PKG_NAME-$PKG_VER

mkdir $BUILD_DIR
cd $BUILD_DIR
wget $TARBALL_URL
tar xfz $TARBALL
cd $SOURCE_DIR
make NET=NO
sudo make install

sudo modprobe pcan

rm -rf $BUILD_DIR
