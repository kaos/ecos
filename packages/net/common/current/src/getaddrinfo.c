//==========================================================================
//
//      lib/getaddrinfo.c
//
//      getaddrinfo(), freeaddrinfo(), gai_strerror(), getnameinfo()
//
//==========================================================================
//####ECOSPDCOPYRIGHTBEGIN####
//
// Copyright (C) 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2003 Gary Thomas
// All Rights Reserved.
//
// Permission is granted to use, copy, modify and redistribute this
// file.
//
//####ECOSPDCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2002-03-05
// Purpose:      
// Description:  
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <sys/param.h>
#include <sys/socket.h>           // PF_xxx
#include <netinet/in.h>           // IPPROTO_xx
#include <net/netdb.h>
#include <netdb.h>                // DNS support routines
#include <errno.h>
#include <cyg/infra/cyg_ass.h>

extern int  sprintf(char *, const char *, ...);
extern long strtol(const char *, char **, int);
extern void *malloc(size_t);
extern void *calloc(int, size_t);
extern void free(void *);

// This routine is the real meat of the host->address translation
static int
_getaddr(struct addrinfo *ai, const char *node, 
         const struct addrinfo *hints, int family, int port)
{
    struct hostent *_hent;

    switch (family) {
    case AF_INET:
    {
        struct sockaddr_in *sa;
        sa = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
        memset(sa, 0, sizeof(*sa));
        ai->ai_addr = (struct sockaddr *)sa;
        ai->ai_addrlen = sizeof(*sa);
        if (ai->ai_addr == (struct sockaddr *)NULL) {
            return EAI_MEMORY;
        }
        sa->sin_family = AF_INET;
        sa->sin_len = sizeof(*sa);
        sa->sin_port = htons(port);
        if (node == (char *)NULL) {
            if (hints->ai_flags & AI_PASSIVE) {
                sa->sin_addr.s_addr = htonl(INADDR_ANY);
            } else {
                sa->sin_addr.s_addr = htonl(INADDR_LOOPBACK);
            }
        } else {
            _hent = gethostbyname(node);
            if (_hent) {
                memcpy(&sa->sin_addr.s_addr, _hent->h_addr, sizeof(struct in_addr));
            } else {
                // For now, only numeric "presentation" addresses supported
                if (!inet_pton(AF_INET, (char *)node, (char *)&sa->sin_addr.s_addr)) {
                    return EAI_FAIL;  // Couldn't resolve name
                }
            }
        }
    }
    break;
#ifdef CYGPKG_NET_INET6
    case AF_INET6:
    {
        struct sockaddr_in6 *sa;
        sa = (struct sockaddr_in6 *)malloc(sizeof(struct sockaddr_in6));
        memset(sa, 0, sizeof(*sa));
        ai->ai_addr = (struct sockaddr *)sa;
        ai->ai_addrlen = sizeof(*sa);
        if (ai->ai_addr == (struct sockaddr *)NULL) {
            return EAI_MEMORY;
        }
        sa->sin6_family = AF_INET6;
        sa->sin6_len = sizeof(*sa);
        sa->sin6_port = htons(port);
        if (node == (char *)NULL) {
            if (hints->ai_flags & AI_PASSIVE) {
                sa->sin6_addr = in6addr_any;
            } else {
                sa->sin6_addr = in6addr_loopback;
            }
        } else {
            // For now, only numeric "presentation" addresses supported
            if (!inet_pton(AF_INET6, (char *)node, (char *)&sa->sin6_addr)) {
                return EAI_FAIL;  // Couldn't resolve name
            }
        }
    }
    break;
#endif
    }
    return EAI_NONE;
}

int   
getaddrinfo(const char *nodename, const char *servname,
            const struct addrinfo *hints, struct addrinfo **res)
{
    struct addrinfo dflt_hints;
    struct protoent *proto = (struct protoent *)NULL;
    struct addrinfo *ai;
    char *protoname;
    int port = 0;
    int family;
    int err;

    if (hints == (struct addrinfo *)NULL) {
        dflt_hints.ai_flags = 0;  // No special flags
        dflt_hints.ai_family = PF_UNSPEC;
        dflt_hints.ai_socktype = 0;
        dflt_hints.ai_protocol = 0;
        hints = &dflt_hints;
    }
    // Prevalidate parameters
    if ((nodename == (char *)NULL) && (servname == (char *)NULL)) {
        return EAI_NONAME;
    }
    switch (hints->ai_family) {
    case PF_UNSPEC:
    case PF_INET:
        family = AF_INET;
        break;
#ifdef CYGPKG_NET_INET6
    case PF_INET6:
        family = AF_INET6;
        break;
#endif
    default:
        return EAI_FAMILY;
    }
    // Allocate the first/primary result
    *res = ai = (struct addrinfo *)calloc(1, sizeof(struct addrinfo));
    if (ai == (struct addrinfo *)NULL) {
        return EAI_MEMORY;
    }
    // Handle request
    if (hints->ai_protocol != 0) {
        proto = getprotobynumber(hints->ai_protocol);
    }
    ai->ai_family = family;
    if (servname != (char *)NULL) {
        switch (hints->ai_socktype) {
        case 0:
        case SOCK_STREAM:
            protoname = "tcp";
            ai->ai_socktype = SOCK_STREAM;
            break;
        case SOCK_DGRAM:
            protoname = "udp";
            ai->ai_socktype = SOCK_DGRAM;
            break;
        default:
            freeaddrinfo(ai);
            return EAI_SOCKTYPE;
        }
        // See if this is just a port #
        if ((port = strtol(servname, 0, 0)) >= 0) {
            ai->ai_socktype = hints->ai_socktype;
            if (hints->ai_socktype == 0) {
                // Need to have complete binding type/port
                freeaddrinfo(ai);
                return EAI_SERVICE;
            }
        } else {
            struct servent *serv = (struct servent *)NULL;

            serv = getservbyname(servname, protoname);
            if (serv == (struct servent *)NULL) {
                if (hints->ai_socktype == 0) {
                    protoname = "udp";
                    serv = getservbyname(servname, protoname);
                }
            }
            if (serv == (struct servent *)NULL) {
                freeaddrinfo(ai);
                return EAI_SERVICE;
            }
            port = ntohs(serv->s_port);      
        }
        proto = getprotobyname(protoname);
        if (hints->ai_protocol && (hints->ai_protocol != proto->p_proto)) {
            freeaddrinfo(ai);
            return EAI_SOCKTYPE;
        }
        ai->ai_protocol = proto->p_proto;
    }
    // Iterate through address types and create addresses
    // Note: this does not handle the case where a given service can be
    // handled via multiple protocols, e.g. http/tcp & http/udp
    err = _getaddr(ai, nodename, hints, family, port);
    if (err != EAI_NONE) {
        freeaddrinfo(ai);
        return err;
    }
#ifdef CYGPKG_NET_INET6
    if (hints->ai_family == PF_UNSPEC) {
        // Add IPv6 address as well
        struct addrinfo *nai = (struct addrinfo *)calloc(1, sizeof(struct addrinfo));
        if (nai == (struct addrinfo *)NULL) {
            freeaddrinfo(ai);
            return EAI_MEMORY;
        }
        ai->ai_next = nai;
        nai->ai_family = PF_INET6;
        nai->ai_socktype = ai->ai_socktype;
        nai->ai_protocol = ai->ai_protocol;
        err = _getaddr(nai, nodename, hints, AF_INET6, port);
        if (err != EAI_NONE) {
            freeaddrinfo(*res);
            return err;
        }
    }
#endif
    // Note: null nodename is the same as 'localhost'
    if (nodename == (char *)NULL) {
        nodename = (const char *)"localhost";
    }
    if (hints->ai_flags & AI_CANONNAME) {
        // Hack - until host name handling is more complete
        ai = *res;  // Canonical name is only in primary record
        ai->ai_canonname = malloc(strlen(nodename)+1);
        if (ai->ai_canonname) {
            strcpy(ai->ai_canonname, nodename);
        }
    }
    if (hints->ai_flags & AI_PASSIVE) {
        // Incomplete addressing - used for bind/listen
    } else {
        // Complete addressing - used for connect/send/...
    }
    return EAI_NONE;  // No errors
}

void  
freeaddrinfo(struct addrinfo *ai)
{
    struct addrinfo *next = ai;

    while ((ai = next) != (struct addrinfo *)NULL) {
        if (ai->ai_canonname) free(ai->ai_canonname);
        if (ai->ai_addr) free(ai->ai_addr);
        next = ai->ai_next;
        free(ai);
    }
}

char 
*gai_strerror(int err)
{
    switch (err) {
    case EAI_NONE:
        return "EAI - no error";
    case EAI_AGAIN:
        return "EAI - temporary failure in name resolution";
    case EAI_BADFLAGS:
        return "EAI - invalid flags";
    case EAI_FAIL:
        return "EAI - non-recoverable failure in name resolution";
    case EAI_FAMILY:
        return "EAI - family not supported";
    case EAI_MEMORY:
        return "EAI - memory allocation failure";
    case EAI_NONAME:
        return "EAI - hostname nor servname provided, or not known";
    case EAI_SERVICE:
        return "EAI - servname not supported for socket type";
    case EAI_SOCKTYPE:
        return "EAI - socket type not supported";
    case EAI_SYSTEM:
        return "EAI - system error";
    case EAI_BADHINTS:
        return "EAI - inconsistent hints";
    case EAI_PROTOCOL:
        return "EAI - bad protocol";
    default:
        return "EAI - unknown error";
    }
}

// Set of flags implemented
#define NI_MASK (NI_NUMERICHOST|NI_NUMERICSERV|NI_NOFQDN|NI_NAMEREQD|NI_DGRAM)

int 
getnameinfo (const struct sockaddr *sa, socklen_t salen, 
             char *host, socklen_t hostlen, 
             char *serv, socklen_t servlen, 
             unsigned int flags)
{
    int port;
    char *s;
    struct servent *se;

    if ((flags & ~NI_MASK) != 0) {
        return EAI_BADFLAGS;
    }
    switch (sa->sa_family) {
    case PF_INET:
#ifdef CYGPKG_NET_INET6
    case PF_INET6:
#endif
        if (host != (char *)NULL) {
            s = _inet_ntop((struct sockaddr *)sa, host, hostlen);
            if (!s) {
                return EAI_FAIL;
            }
        }
        if (serv != (char *)NULL) {
            port = _inet_port((struct sockaddr *)sa);
            if (!port) {
                return EAI_FAIL;
            }
            se = (struct servent *)NULL;
            if ((flags & NI_NUMERICSERV) == 0) {
                if ((flags & NI_DGRAM) == 0) {
                    se = getservbyport(port, "tcp");
                }
                if (se == (struct servent *)NULL) {
                    se = getservbyport(port, "ucp");
                }
            }
            if (se != (struct servent *)NULL) {
                sprintf(serv, "%s/%s", se->s_name, se->s_proto);
            } else {
                sprintf(serv, "%d", port);
            }
        }
        break;
    default:
        return EAI_FAMILY;
    }
    return EAI_NONE;
}
