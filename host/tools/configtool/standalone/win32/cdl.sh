# $1==IntDir $2==OutDir $3==Additional params to configure
ECOSHOME=`pwd`/../../../../..
TCLHOME=${TCLHOME:-"`pwd`/../../../../../../ide/tcl"}
INTDIR=`cygpath -u $1`
OUTDIR=`cygpath -u $2`
mkdir -p $INTDIR
cd $INTDIR &&\
    CC=cl CXX=cl $ECOSHOME/configure --prefix=$OUTDIR --with-tcl=$TCLHOME --with-tcl_version=81 --disable-standalone-configtool $3 &&\
    MAKE_MODE=unix make install
