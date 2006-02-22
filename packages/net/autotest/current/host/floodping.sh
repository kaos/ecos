#!/bin/sh

# timeout is the total time to run the test, or thereabouts.
#
# ping/rest times are to let us do stuff like flood for 3 seconds then do
# nothing for 5 seconds, over the total test time.
#
# That way this script can be used to torment other test variants such as
# the SNMP walk, ordinary pinging and so on, all making for a more diverse
# test environment.


if [ $# -lt 1 ]; then
    echo "Usage: $0 [XFAIL] target [timeout [size [ping rest]]" > /dev/stderr
    exit 1
fi

[ "xXFAIL" == "x$1" ] && { XFAIL="XFAIL"; shift; }
TARGET=$1
if [ "x" == "x$2" ]; then TIMEOUT=10;   else TIMEOUT=$2;  fi
if [ "x" == "x$3" ]; then PINGSIZE=64;  else PINGSIZE=$3; fi
if [ "x" == "x$4" ]; then PINGTIME=19;  else PINGTIME=$4; fi
if [ "x" == "x$5" ]; then RESTTIME=1;   else RESTTIME=$5; fi

# By default, flood for 19 seconds then rest for one.
# We take one second off occasionally to allow eg. DHCP to stay alive in
# configurations/environments where it is so short lived.

if [ "x$XFAIL" != "xXFAIL" ]; then
    # first try normal size packets
    if ping -n -c 5 $TARGET > /dev/null ; then
	echo "INFO:<$$: $TARGET is up>" > /dev/stderr
    else
	echo "FAIL:<$$: $TARGET is down>" > /dev/stderr; exit 1
    fi
fi

# allow other pingers &c to start up:
sleep 5

declare -i COUNT=0

# then the size we were asked to do, for the time requested

while [ $SECONDS -le $TIMEOUT ]; do

# Use an explicit path in '.' because this must be an SUID script to be
# able to use the flood option.  Direct stderr to tmp also because we
# cannot report results whilst flooding.
./_suping -f -n -w $PINGTIME -q -s $PINGSIZE $TARGET > /tmp/ping$$ 2>&1

# dump this out for the controller logfile
cat /tmp/ping$$

INFO1=$(grep "bytes of data" /tmp/ping$$)
INFO2=$(grep "packet loss" /tmp/ping$$)
INFO3=$(grep "round-trip" /tmp/ping$$)

rm -f /tmp/ping$$

# but do not print it all out to stderr (and so to sendack and over the
# net); the two cannot coexist reliably.

#[ "x" != "x$INFO1" ] && echo "INFO:<$$: $INFO1>" > /dev/stderr
#[ "x" != "x$INFO2" ] && echo "INFO:<$$: $INFO2>" > /dev/stderr
#[ "x" != "x$INFO3" ] && echo "INFO:<$$: $INFO3>" > /dev/stderr

# then a delay for other traffic - else other tests fail
sleep $RESTTIME

((COUNT++));
done

# allow other flood pingers to finish:
sleep 5

if [ "x$XFAIL" == "xXFAIL" ]; then
    echo "PASS:<$$: flood ping XFAIL, $PINGSIZE bytes $TIMEOUT seconds $COUNT sessions>" > /dev/stderr
    exit 0
fi

# we cannot really look for pass/fail from a flood ping...

echo "PASS:<$$: flood ping OK, $PINGSIZE bytes $TIMEOUT seconds $COUNT sessions>" > /dev/stderr

# EOF
