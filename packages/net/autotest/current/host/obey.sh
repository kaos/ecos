#!/bin/sh

if [ $# -gt 1 ]; then
    echo "Usage: $0 [acknowledgement-agent]" > /dev/stderr
    exit 1
fi

# add . to the path for testing.
export PATH=$PATH:.

# get a script for returning results to the testcase; its API is
#      echo .... | SENDACK [XFAIL] <target>
if [ $# -gt 0 ]; then
    SENDACK=$1; shift
else
    SENDACK=sendack.sh
fi

TMP=/tmp/auto	;# root of temporary filespace

WF=${TMP}/wf	;# work files for tftp et al
LG=${TMP}/log	;# log files for individual tests

declare -i unique=0 ;# unique IDs for workfiles and logfiles

while [ true ]; do

    ((unique++))

    read -a ORDERS || {	echo "FAIL: EOF on read" > /dev/stderr; exit 1; }

    set ${ORDERS[@]}

    [ "xQUIT" == "x$1" ] && break

    [ "xORDERS:" != "x$1" ] && \
	{ echo "No ORDERS tag" > /dev/stderr; continue; }

    XFAIL=""; [ "xXFAIL" == "x$2" ] && { XFAIL="XFAIL"; shift; }

    shift; TEST=$1
    shift; TARGET=$1
    shift; PARAMS=($@)

    echo "test $TEST [$XFAIL]; target $TARGET; params ${PARAMS[@]}"

    # Be robust against zealous GC of workfiles - as late as possible
    # before we use them, it's in the read above that we sleep.
    [ ! -d ${TMP} ] && mkdir ${TMP}
    [ ! -d ${WF}  ] && mkdir ${WF}
    [ ! -d ${LG}  ] && mkdir ${LG}

    # Now the main switch that runs the test script: this is where
    # new testcases are added:
    case $TEST in

    TFTP_SERV_GET)
    tftpget.sh $XFAIL $TARGET $WF/tftpget.$unique ${PARAMS[@]} \
	    2>&1 >$LG/tftpget.$unique   | $SENDACK $XFAIL $TARGET & ;;

    TFTP_SERV_PUT)
    tftpput.sh $XFAIL $TARGET $WF/tftpput.$unique ${PARAMS[@]} \
	    2>&1 >$LG/tftpput.$unique   | $SENDACK $XFAIL $TARGET & ;;

    SLOW_PING)
    slowping.sh $XFAIL $TARGET ${PARAMS[@]} \
	    2>&1 >$LG/slowping.$unique  | $SENDACK $XFAIL $TARGET & ;;

    FLOOD_PING)
    floodping.sh $XFAIL $TARGET ${PARAMS[@]} \
	    2>&1 >$LG/floodping.$unique  | $SENDACK $XFAIL $TARGET & ;;

    SNMP_WALK)
    snmpwalk.sh $XFAIL $TARGET ${PARAMS[@]} \
	    2>&1 >$LG/snmpwalk.$unique  | $SENDACK $XFAIL $TARGET & ;;

    *) echo "Unknown order $TEST" > /dev/stderr ;;

    esac

done

# EOF
