#ifndef SMUGGLER_H
#define SMUGGLER_H

#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <sys/ioctl.h>

#define PACKET_LENGTH 44

typedef unsigned int u_int;

using namespace std;

class Smuggler
{
public:
    Smuggler(const string &interface, const string &ipv6, const u_int &metric, const string &nextHop, const u_int &routerTag, const u_int &prefixLen);

    ~Smuggler();

    //! Returns true, if there was error somewhere, false otherwise
    bool error() const;

private:
    //! Crafts a packet
    void craftPacket(const string &ipv6, const u_int &metric, const string &nextHop, const u_int &routerTag, const u_int &prefixLen);

    //! Sends a packet via multicast
    void sendPacket(const string &interface);

private:
    u_char *packet_;

    bool error_;

    bool nextHopSpecified_ = false;
};

#endif // SMUGGLER_H
