#!/bin/sh

if [ $# -lt 1 ]; then
    echo "Usage: $0 [XFAIL] target [timeout]" > /dev/stderr
    exit 1
fi

[ "xXFAIL" == "x$1" ] && { XFAIL="XFAIL"; shift; }
TARGET=$1
if [ "x" == "x$2" ]; then TIMEOUT=10;   else TIMEOUT=$2;  fi

if [ "x$XFAIL" != "xXFAIL" ]; then
    # first try normal size packets
    if ping -n -c 5 $TARGET > /dev/null ; then
	echo "INFO:<$$: $TARGET is up>" > /dev/stderr
    else
	echo "FAIL:<$$: $TARGET is down>" > /dev/stderr; exit 1
    fi
fi

export MIBS=all

declare -i COUNT=0
declare -i FAILS=0

# then the size we were asked to do, for the time requested

while [ $SECONDS -le $TIMEOUT ]; do

snmpwalk -OS $TARGET public

snmpstatus $TARGET public || { ((FAILS++)); [ "x$XFAIL" == "xXFAIL" ]; } || break

((COUNT++));

done


if [ "x$XFAIL" == "xXFAIL" ]; then
    echo "PASS:<$$: snmpwalk XFAIL, $TIMEOUT seconds $COUNT snmpwalks $FAILS failed>" > /dev/stderr
    exit 0
fi

if [ $FAILS -ne 0 ]; then
    echo "FAIL:<$$: snmpwalk $TIMEOUT seconds $COUNT snmpwalks - status failed>" > /dev/stderr
    exit 1
fi

echo "PASS:<$$: snmpwalk $TIMEOUT seconds $COUNT snmpwalks>" > /dev/stderr

# EOF
