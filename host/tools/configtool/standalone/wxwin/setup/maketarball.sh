#!/bin/sh

# Make a distribution of eCos Configtool for Linux
# Julian Smart 2001-04-27
# Usage: maketarball.sh cvs-dir bin-dir deliver-dir [ options ]
# For example: maketarball.sh ~/local/eCos /tmp/ecos-build-ct /tmp/ct-deliver

ECOSDIR=$1
CONFIGTOOLDIR=$ECOSDIR/ecc/host/tools/configtool/standalone/wxwin
SRC=$CONFIGTOOLDIR
BINARYSRC=$2
DEST=$3
BUILD=0
UPX=0
PROGNAME=$0

# Set this to the required version
VERSION=$4

dotar()
{
    rm -f -r $DEST/configtool*
    rm -f $DEST/configtool-*.*

    mkdir -p $DEST/configtool-$VERSION
    mkdir -p $DEST/configtool-$VERSION/manual
    mkdir -p $DEST/configtool-$VERSION/manual/pix
    cd $DEST/configtool-$VERSION

    cp $SRC/*.htm .
    cp $SRC/*.png .
    cp $SRC/bitmaps/splash16.png .
    zip configtool.bin *.htm *.png
    rm -f *.htm *.png
    cp $SRC/setup/ecosplatforms.tar.gz .
    cp $SRC/README.txt .
    cp $SRC/CHANGES.txt .
    cp $SRC/TODO.txt .
    cp "$ECOSDIR/ecc/release/eCos Install/Setup Files/Uncompressed Files/Disk1/license.txt" .
    cp $BINARYSRC/configtool .
    cp $CONFIGTOOLDIR/manual/*.html $CONFIGTOOLDIR/manual/*.css $CONFIGTOOLDIR/manual/*.gif manual
    cp $CONFIGTOOLDIR/manual/pix/*.gif manual/pix

    strip configtool

    if [ "$UPX" != "0" ]; then
        upx configtool
    fi

    cd ..
    
    tar cvf $DEST/configtool-$VERSION-i386.tar configtool-$VERSION/*
    gzip -c $DEST/configtool-$VERSION-i386.tar > $DEST/configtool-$VERSION-i386.tar.gz
    bzip2 -c $DEST/configtool-$VERSION-i386.tar > $DEST/configtool-$VERSION-i386.tar.bz2
}

dobuild()
{
    makect release full
}

usage()
{
    echo Usage: $PROGNAME "cvs-dir bin-dir deliver-dir version-number [ options ]"
    echo Options:
    echo "    --help         Display this help message"
    echo "    --upx          Compress executable with UPX"
    echo "    --build        Invoke 'makect release full' first"
    echo For example: $PROGNAME ~/local/eCos /tmp/ecos-build-ct /tmp/ct-deliver 1.0
    exit 1
}

# Process command line options.
shift 4
for i in "$@"; do
    case "$i" in
	--build) BUILD=1 ;;
	--upx) UPX=1 ;;
	*)
	    usage
	    exit
	    ;;
    esac
done

if [ ! -d "$DEST" ]; then
    mkdir -p $DEST
fi

if [ ! -d "$SRC" ]; then
    echo CVS source directory $SRC not found.
    usage
    exit 1
fi

if [ ! -d "$BINARYSRC" ]; then
    echo Location of configtool binary $BINARYSRC not found.
    usage
    exit 1
fi

if [ "$VERSION" = "" ]; then
    echo Pass the version number as the fourth argument.
    usage
    exit 1
fi

echo Creating Version $VERSION distribution in $DEST, using CVS directory $SRC and configtool binary in $BINARYSRC.
#echo Press return to continue.
#read dummy

# Remove all existing files
if [ ! -d "$DEST/configtool" ]; then
    rm -f -r $DEST/configtool
fi

# Skip INNO setup if INNO is 0.
if [ "$BUILD" = "1" ]; then
    dobuild
fi

dotar

echo Configtool archived.

