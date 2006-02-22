// _suping.c

#include <unistd.h>

// This a piece of lameness to convince ping to do a floodping.
int main( int argc, char **argv )
{
    setuid( 0 );
    execvp( "ping", argv );
    perror( "What, no ping?" );
    exit( 1 );
}

// EOF
