#!/bin/sh

if [ $# -gt 2 ] || [ $# -lt 1 ]; then
    echo "Usage: $0 [XFAIL] target-IP [port]" > /dev/stderr
    exit 1
fi

[ "xXFAIL" == "x$1" ] && { XFAIL="XFAIL"; shift; }

TARGET=$1; shift

PORT=9990; [ "x" != "x$1" ] && { PORT=$1; shift; }

while [ true ]; do

    read -a RESULT || {	exit 0; }

    echo "$TARGET: result ${RESULT[@]}"

    # I really want it to try quite hard to get the message through
    # (but not too hard if XFAIL)
    echo "${RESULT[@]} ($TARGET)" > /dev/tcp/$TARGET/$PORT || \
    echo "${RESULT[@]} ($TARGET)" > /dev/tcp/$TARGET/$PORT || \
    { sleep 1 && { \
    echo "${RESULT[@]} ($TARGET)" > /dev/tcp/$TARGET/$PORT || \
    echo "${RESULT[@]} ($TARGET)" > /dev/tcp/$TARGET/$PORT || \
    { sleep 3 && { \
    echo "${RESULT[@]} ($TARGET)" > /dev/tcp/$TARGET/$PORT || \
    echo "${RESULT[@]} ($TARGET)" > /dev/tcp/$TARGET/$PORT || \
    { [ "x$XFAIL" != "xXFAIL" ] && sleep 5 && { \
    echo "${RESULT[@]} ($TARGET)" > /dev/tcp/$TARGET/$PORT || \
    echo "${RESULT[@]} ($TARGET)" > /dev/tcp/$TARGET/$PORT || \
    { sleep 10 && { \
    echo "${RESULT[@]} ($TARGET)" > /dev/tcp/$TARGET/$PORT || \
    echo "${RESULT[@]} ($TARGET)" > /dev/tcp/$TARGET/$PORT || \
    echo "***FAILED TO SEND RESULT TO $TARGET: result ${RESULT[@]}" \
    ;};};};};};};};}
    # brute force seems as good as any means...

done

# EOF
