#!/bin/sh

if [ $# -lt 1 ]; then
    echo "Usage: $0 [XFAIL] target [timeout [size]]" > /dev/stderr
    exit 1
fi

[ "xXFAIL" == "x$1" ] && { XFAIL="XFAIL"; shift; }
TARGET=$1
if [ "x" == "x$2" ]; then TIMEOUT=10;   else TIMEOUT=$2; fi
if [ "x" == "x$3" ]; then PINGSIZE=64;  else PINGSIZE=$3; fi

if [ "x$XFAIL" != "xXFAIL" ]; then
    # first try normal size packets
    if ping -n -c 5 $TARGET > /dev/null ; then
	echo "INFO:<$$: $TARGET is up>" > /dev/stderr
    else
	echo "FAIL:<$$: $TARGET is down>" > /dev/stderr; exit 1
    fi
fi

# then the size we were asked to do, for the time requested
ping -n -c $TIMEOUT -s $PINGSIZE $TARGET > /tmp/ping$$ && PINGRESULT=ok

# dump this out for the controller logfile
cat /tmp/ping$$

INFO1=$(grep "bytes of data" /tmp/ping$$)
INFO2=$(grep "packet loss" /tmp/ping$$)
INFO3=$(grep "round-trip" /tmp/ping$$)

rm -f /tmp/ping$$

[ "x" != "x$INFO1" ] && echo "INFO:<$$: $INFO1>" > /dev/stderr
[ "x" != "x$INFO2" ] && echo "INFO:<$$: $INFO2>" > /dev/stderr
[ "x" != "x$INFO3" ] && echo "INFO:<$$: $INFO3>" > /dev/stderr

if [ "x$XFAIL" == "xXFAIL" ]; then
    echo "PASS:<$$: slow ping XFAIL, $PINGSIZE bytes $TIMEOUT packets/seconds>" > /dev/stderr
    exit 0
fi

if [ "x" == "x$PINGRESULT" ]; then
    echo "FAIL:<$$: slow ping failed, $PINGSIZE bytes $TIMEOUT packets/seconds>" > /dev/stderr
    exit 1
fi

echo "PASS:<$$: slow ping OK, $PINGSIZE bytes $TIMEOUT packets/seconds>" > /dev/stderr

# EOF
