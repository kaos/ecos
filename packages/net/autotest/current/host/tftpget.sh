#!/bin/sh

if [ $# -lt 1 ]; then
    echo "Usage: $0 target [tempfile [timeout [filesize]]]" > /dev/stderr
    exit 1
fi

[ "xXFAIL" == "x$1" ] && { XFAIL="XFAIL"; shift; }
TARGET=$1
if [ "x" == "x$2" ]; then TMP=tftpdata; else TMP=$2; fi
if [ "x" == "x$3" ]; then TIMEOUT=10;   else TIMEOUT=$3; fi
if [ "x" == "x$4" ]; then SIZE=unknown; else SIZE=$4; fi

if [ -f $TMP ]; then
    SRC=${TMP}
    TMP=${TMP}.tmp
else
    SRC=${TMP}.src
    TMP=${TMP}.tmp
fi

if [ -f $SRC ]; then
    [ $SIZE == unknown ] ||  \
    { echo "FAIL: '$SRC' exists but size given" > /dev/stderr; exit 1; }
else
    [ $SIZE == unknown ] && SIZE=876543
    tmpfile $SRC $SIZE $$ || \
    { echo "FAIL: Sourcefile '$SRC' create" > /dev/stderr; exit 1; }
fi

if [ ! -f $SRC ]; then
    echo "FAIL: Sourcefile '$SRC' not found" > /dev/stderr; exit 1
fi

if [ "x$XFAIL" != "xXFAIL" ]; then
    if ping -n -c5 $TARGET > /dev/null ; then
	echo "INFO:<$$: $TARGET is up>" > /dev/stderr
    else
	echo "FAIL:<$$: $TARGET is down>" > /dev/stderr; exit 1
    fi
fi

tftp $TARGET  <<-EOF
	rexmt 1
	binary
	put $SRC $TMP
EOF

declare -i COUNT=0
declare -i FAILS=0

# This test is to test tftp GET not PUT so gloss over one failed put
# (ie. if it reports corrupt the first time, put it again).
rm -f $TMP
tftp $TARGET  <<-EOF
	rexmt 1
	binary
	get $TMP $TMP
EOF
if ! cmp -s $SRC $TMP; then
echo "INFO:<$$: putting $SIZE bytes to $TARGET a second time>"
tftp $TARGET  <<-EOF
	rexmt 1
	binary
	put $SRC $TMP
EOF
else
((COUNT++));
fi

while [ $SECONDS -le $TIMEOUT ]; do
rm -f $TMP
tftp $TARGET  <<-EOF
	rexmt 1
	binary
	get $TMP $TMP
EOF
[ -f $TMP ]       || [ "x$XFAIL" == "xXFAIL" ] || break
cmp -s $SRC $TMP  || { ((FAILS++)); [ "x$XFAIL" == "xXFAIL" ]; } || break
((COUNT++));
done

if [ "x$XFAIL" == "xXFAIL" ]; then
    echo "PASS:<$$: tftp get XFAIL, $TIMEOUT seconds $SIZE bytes $FAILS/$COUNT fails>" > /dev/stderr
    exit 0
fi

if [ 1 -lt $FAILS ]; then
    echo "FAIL:<$$: wierd, multiple failures $FAILS in $COUNT xfers>" > /dev/stderr
fi

if [ ! -f $TMP ]; then
    echo "FAIL:<$$: temp file $TMP nonexistent $COUNT xfers>" > /dev/stderr
    exit 1
fi

if ! cmp -s $SRC $TMP; then
    echo "FAIL:<$$: temp file $TMP corrupt $COUNT xfers>" > /dev/stderr
    exit 1
fi

echo "PASS:<$$: tftp get OK, $TIMEOUT seconds $SIZE bytes $COUNT xfers>" > /dev/stderr

# EOF
