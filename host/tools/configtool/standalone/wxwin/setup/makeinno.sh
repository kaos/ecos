#! /bin/sh
# Make an Inno Setup distribution list, where files and dirs are represented by
# sections like this:
# [Dirs]
#     Name: {app}\backgrounds
# 
# [Files]
#     Source: C:\program\setup\about.htm; DestDir: {app}\; DestName: about.htm

ECOSDIR=$1
CONFIGTOOLDIR=$ECOSDIR/ecc/host/tools/configtool/standalone/wxwin
DESTDIR=/v/deliver
SRCDIR=/v/deliver/configtool

if [ ! -d "$ECOSDIR" ]; then
    echo eCos root directory $ECOSDIR not found.
    exit 1
fi

if [ ! -d "$SRCDIR" ]; then
    echo Source directory $SRCDIR not found.
    exit 1
fi

if [ ! -d "$DESTDIR" ]; then
    echo Destination directory $DESTDIR not found.
    exit 1
fi

SRCDIR=`cygpath -u $SRCDIR`
DESTDIR=`cygpath -u $DESTDIR`
TEMPDIR=`cygpath -u $TEMP`

# Generate a list of all files in the distribution.
# We pass the output through sed in order to remove the preceding "./"
cd $SRCDIR
find . -print | sed -e "s/\.\\///g" > $TEMPDIR/files1.tmp

echo "[Dirs]" > $TEMPDIR/files2.tmp

for line in `cat $TEMPDIR/files1.tmp` ; do

    # If a directory, add to file
    if [ -d $line ] ; then
        # The relative path
        line2=`cygpath -w $line`

        echo "  Name: {app}\\"$line2 >> $TEMPDIR/files2.tmp
    fi
done

echo "" >> $TEMPDIR/files2.tmp
echo "[Files]" >> $TEMPDIR/files2.tmp

for line in `cat $TEMPDIR/files1.tmp` ; do

    # If not a directory, add to file
    if [ ! -d $line ] ; then
        # The relative path
        line2=`cygpath -w $line`
        # The absolute path
        line1=`cygpath -w $SRCDIR`"\\"$line2
        pathonly=`find $line -printf "%h"`

        echo "  Source: "$line1"; DestDir: {app}\\"$pathonly >> $TEMPDIR/files2.tmp
    fi
done

echo "" >> $TEMPDIR/files2.tmp

# Concatenate the 3 sections
cat $CONFIGTOOLDIR/setup/innotop.txt $TEMPDIR/files2.tmp $CONFIGTOOLDIR/setup/innobott.txt > $CONFIGTOOLDIR/setup/configtool.iss

rm -f $TEMPDIR/files1.tmp


