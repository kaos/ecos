#!/bin/sh

# Make a distribution of eCos Configtool, putting results in /v/deliver.
# ECOSDIR=/d/eCos
ECOSDIR=$1
CONFIGTOOLDIR=$ECOSDIR/ecc/host/tools/configtool/standalone/wxwin
SRC=$CONFIGTOOLDIR
BINARYSRC=/v/ConfigToolWX/ANSIRelease
DEST=/v/deliver
LOCALARCHIVEDIR=//sandpiper/ecc-share/releng/eCosTools/eCosConfigurationTool
INNOCOMPILER="/c/Program Files/Inno Setup 1.3/compil32.exe"
INNO=1
INNOONLY=0
UPX=0
MAKESCRIPT=1
COPYTOARCHIVE=0
# If your zip accepts Cygwin-style paths, then
# use cygpath, else substitute echo
CYGPATHPROG=cygpath
#CYGPATHPROG=echo
PROGNAME=$0

# Set this to the required version
VERSION=$2

findversion()
{
	echo "#include <stdio.h>" > /tmp/ctver.c
	echo "#include \"symbols.h\"" >> /tmp/ctver.c
	echo "int main() { printf(\"%.1f\", ecCONFIGURATION_TOOL_VERSION); }" >> /tmp/ctver.c
	gcc /tmp/ctver.c -I$CONFIGTOOLDIR -o /tmp/ctver
    VERSION=`/tmp/ctver`
	rm -f /tmp/ctver /tmp/ctver.c
}

doinno()
{
    mkdir -p $DEST/configtool
    mkdir -p $DEST/configtool/manual
    mkdir -p $DEST/configtool/manual/pix
    cd $DEST/configtool

    cp $SRC/about.htm .
    cp $SRC/README.txt .
    cp $SRC/*.png .
    cp "$ECOSDIR/ecc/release/eCos Install/Setup Files/Uncompressed Files/Disk1/license.txt" .
    cp $BINARYSRC/configtool.exe .
    cp $CONFIGTOOLDIR/manual/*.html $CONFIGTOOLDIR/manual/*.css $CONFIGTOOLDIR/manual/*.gif $DEST/configtool/manual
    cp $CONFIGTOOLDIR/manual/pix/*.gif $DEST/configtool/manual/pix

    if [ "$UPX" != "0" ]; then
        upx configtool.exe
    fi
    
    # Time to regenerate the Inno Install script, configtool.iss
    if [ "$MAKESCRIPT" != "0" ]; then
      echo Calling 'makeinno' to generate configtool.iss...

      sh $CONFIGTOOLDIR/setup/makeinno.sh $ECOSDIR

      # Now replace %VERSION% with the real Configtool version
      sed -e "s;%VERSION%;$VERSION;g;" < $SRC/setup/configtool.iss > /tmp/configtool.iss
      cp /tmp/configtool.iss $SRC/setup/configtool.iss
      rm -f /tmp/configtool.iss
    fi

    rm -f $DEST/setup.*
    
    # Now invoke INNO install on the new configtool.iss
    echo Invoking INNO...
    
    "$INNOCOMPILER" /cc `cygpath -w $SRC`\\setup\\configtool.iss

    cd $DEST
    mv setup.exe configtool-$VERSION-setup.exe

    if [ "$COPYTOARCHIVE" = "1" ]; then
      echo Copying to $LOCALARCHIVEDIR...
      if [ ! -d $LOCALARCHIVEDIR/`date +%Y-%m-%d` ]; then
        # Can't use -p because Cygwin mkdir fails on network paths
        mkdir "$LOCALARCHIVEDIR/`date +%Y-%m-%d`"
        exit 1
      fi

      #echo About to: cp configtool-$VERSION-setup.exe "$LOCALARCHIVEDIR/`date +%Y-%m-%d`"
      cp configtool-$VERSION-setup.exe "$LOCALARCHIVEDIR/`date +%Y-%m-%d`"
    fi
}

# We can't use e.g. this:
# ls `cat $SRC/distrib/msw/makefile.rsp` zip -@ -u $DEST/wxWindows-$VERSION-gen.zip
# because there's not enough space on the command line, plus we need to ignore the
# blank lines.
# So if we need to (not in this script so far) we do something like this instead:
# expandlines $SRC/setup/files.rsp temp.txt
# zip -@ `$CYGPATHPROG -w $DEST/archive.zip` < temp.txt

expandlines()
{
    toexpand=$1
    outputfile=$2

    rm -f $outputfile
    touch $outputfile
    for line in `cat $toexpand` ; do
      if [ $line != "" ]; then
        ls $line >> $outputfile
      fi
    done
}

usage()
{
    echo Usage: $PROGNAME "eCosDir Version [ options ]" 1>&2
    echo Options:
    echo "    --help          Display this help message"
    echo "    --upx           Compress executable with UPX"
    echo "    --nomakescript  Don't generate the .iss script"
    echo "    --copytoarchive Copy to where archives are stored locally"
# At present, we remove these options. If later, we build e.g. a zip file
# as an alternative to setup.exe, then we'll reinstate the options.
# For now, always just build the setup.exe.
#	    echo "    --inno         Also build setup.exe"
#	    echo "    --innoonly     Build setup.exe only"
    exit 1
}

if [ "$1" = "" ]; then
    usage
    exit
fi

# Process command line options.
shift 1
if [ "$1" != "" ]; then
    shift 1
fi

for i in "$@"; do
    case "$i" in
	--inno) INNO=1 ;;
	--innoonly) INNOONLY=1 ;;
	--upx) UPX=1 ;;
	--nomakescript) MAKESCRIPT=0 ;;
    --copytoarchive) COPYTOARCHIVE=1 ;;
	*)
	    usage
	    exit
	    ;;
    esac
done

if [ ! -d "$SRC" ]; then
    mkdir -p $SRC
fi

if [ ! -d "$DEST" ]; then
    mkdir -p $DEST
fi

if [ ! -d "$SRC" ]; then
    echo $SRC not found.
    exit 1
fi

if [ ! -d "$DEST" ]; then
    echo $DEST not found.
    exit 1
fi

if [ "$VERSION" = "" ] || [ "$VERSION" = "0" ]; then
    findversion
    echo Found version $VERSION
fi

echo Creating distribution in $DEST. Options are $@.

# Remove all existing files
if [ ! -d "$DEST/configtool" ]; then
    rm -f -r $DEST/configtool
fi

# Skip INNO setup if INNO is 0.
if [ "$INNO" = "1" ]; then
    doinno
fi

echo Configtool archived.

