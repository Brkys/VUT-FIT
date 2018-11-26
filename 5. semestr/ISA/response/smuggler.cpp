#include "smuggler.h"

Smuggler::Smuggler(const string &interface, const string &ipv6, const u_int &metric, const string &nextHop, const u_int &routerTag, const u_int &prefixLen)
{
    //cout << interface << endl << ipv6 << endl << metric << endl << nextHop << endl << routerTag << endl << prefixLen << endl;
    craftPacket(ipv6, metric, nextHop, routerTag, prefixLen);
    if(!error_)
    {
        sendPacket(interface);
    }
}

Smuggler::~Smuggler()
{
    free(packet_);
}


void Smuggler::craftPacket(const string &ipv6, const u_int &metric, const string &nextHop, const u_int &routerTag, const u_int &prefixLen)
{    
    packet_ = static_cast<u_char *>(malloc(sizeof(u_char) * PACKET_LENGTH - (strcmp(nextHop.c_str(), "::") == 0 ? 20 : 0))); //L

    if(!packet_)
    {
        error_ = true;
        return;
    }

    packet_[0] = 0x02; //Response
    packet_[1] = 0x01; //Version
    packet_[2] = packet_[3] = 0x00; //Reserved

    //Setting next hop (if there is any)
    if(strcmp(nextHop.c_str(), "::") != 0)
    {
        nextHopSpecified_ = true;

        //Setting second entry IPv6 as next hop (4 - 19)
        struct in6_addr nextHopipv6Result;
        if(inet_pton(AF_INET6, nextHop.c_str(), &nextHopipv6Result))
        {
            for(int i = 0; i < 16;i++)
            {
                packet_[i+4] = static_cast<u_char>(nextHopipv6Result.s6_addr[i]);
            }
        }
        else
        {
            cerr << "Error: Unknown IPv6 format as next hop!" << endl;
            error_ = true;
            return;
        }

        //Must be zero (20 - 22)
        for(int i = 20; i < 23; i++)
            packet_[i] = 0x00;

        //Must be 0xFF - defined by next hop(23)
        packet_[23] = 0xff;

        //Setting first entry IPv6 (24 - 39)
        struct in6_addr ipv6Result;
        if(inet_pton(AF_INET6, ipv6.c_str(), &ipv6Result))
        {

            for(int i = 0; i < 16;i++)
            {
                packet_[i+24] = static_cast<u_char>(ipv6Result.s6_addr[i]);
            }
        }
        else
        {
            cerr << "Error: Unknown IPv6 format!" << endl;
            error_ = true;
            return;
        }

        //Setting first entry router tag (40 - 41)
        memcpy(&packet_[40], &routerTag, 2);
        packet_[40] ^= packet_[41];
        packet_[41] ^= packet_[40];
        packet_[40] ^= packet_[41];

        //Setting first entry preffix length (42)
        packet_[42] = static_cast<u_char>(prefixLen);

        //Setting first entry metric (43)
        packet_[43] = static_cast<u_char>(metric);
    }
    else  //Next hop is not set
    {
        //Setting first entry IPv6 (4 - 19)
        struct in6_addr ipv6Result;
        if(inet_pton(AF_INET6, ipv6.c_str(), &ipv6Result))
        {

            for(int i = 0; i < 16;i++)
            {
                packet_[i+4] = static_cast<u_char>(ipv6Result.s6_addr[i]);
            }
        }
        else
        {
            cerr << "Error: Unknown IPv6 format!" << endl;
            error_ = true;
            return;
        }

        //Setting first entry router tag (20 - 21)
        memcpy(&packet_[20], &routerTag, 2);
        packet_[20] ^= packet_[21];
        packet_[21] ^= packet_[20];
        packet_[20] ^= packet_[21];

        //Setting first entry preffix length (22)
        packet_[22] = static_cast<u_char>(prefixLen);

        //Setting first entry metric (23)
        packet_[23] = static_cast<u_char>(metric);

    }
}

void Smuggler::sendPacket(const string &interface)
{
    //Getting interface index
    u_int interfaceIndex = if_nametoindex(interface.c_str());

    struct ifaddrs *interfaces, *interfaceList;
    string linkLocalIPv6;

    //Getting local link IPv6
    if (getifaddrs(&interfaces) == -1) {
        perror("Error: Getting address from interface failed!");
        error_ = true;
        return;
    }
    interfaceList = interfaces;
    for(; interfaces != nullptr; interfaces = interfaces->ifa_next) {
        if ((interfaces->ifa_addr) && interfaces->ifa_addr->sa_family != AF_INET6)
            continue;
        char tmp[50];
        struct sockaddr_in6 *in6tmp = reinterpret_cast<struct sockaddr_in6*>(interfaces->ifa_addr);
        inet_ntop(AF_INET6, &in6tmp->sin6_addr, tmp, sizeof(tmp));
        if(strcmp(interfaces->ifa_name, interface.c_str()) == 0 && IN6_IS_ADDR_LINKLOCAL((const struct sockaddr_in6 *)&in6tmp->sin6_addr))
        {
            linkLocalIPv6 = string(tmp);
            break;
        }
    }
    freeifaddrs(interfaceList);

    int sock;

    // Create a datagram socket to send packet
    if((sock = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) {
        cerr << "Error: Opening datagram socket failed!" << endl;
        error_ = true;
        return;
    }

    int reuseAddr = 1;
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr)) < 0) {
        cerr << "Error: Socket SO_REUSEADDR configuration error!" << endl;
        error_= true;
        return;
    }

    int TTL = 255;
    if(setsockopt(sock, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &TTL, sizeof(TTL))) {
        cerr << "Error: Setting local interface error" << endl;
        error_ = true;
        return;
    }

    if(setsockopt(sock, IPPROTO_IPV6, IPV6_MULTICAST_IF, &interfaceIndex, sizeof(interfaceIndex))) {
        cerr << "Error: Setting local interface error" << endl;
        error_ = true;
        return;
    }

    struct sockaddr_in6 bindAddr;
    bzero(&bindAddr, sizeof(bindAddr));
    bindAddr.sin6_family = AF_INET6;
    bindAddr.sin6_port = htons(521);
    bindAddr.sin6_scope_id = interfaceIndex;
    inet_pton(AF_INET6, linkLocalIPv6.c_str(), &bindAddr.sin6_addr);

    struct sockaddr_in6 destAddr;
    bzero(&destAddr, sizeof(destAddr));
    destAddr.sin6_family = AF_INET6;
    destAddr.sin6_port = htons(521); //Port RIPng
    destAddr.sin6_scope_id = interfaceIndex;
    inet_pton(AF_INET6, "FF02::9", &destAddr.sin6_addr);

    if(bind(sock, (struct sockaddr *) &bindAddr, sizeof (bindAddr)) == -1)
    {
        cerr << "Error: Unable to bind socket" << endl;
        error_ = true;
        return;
    }

    if(sendto(sock, packet_, sizeof(u_char) * PACKET_LENGTH - (nextHopSpecified_ ? 0 : 20), 0, (struct sockaddr *) &destAddr, sizeof (destAddr)) == -1)
    {
        cerr << "Error: Failed to send packet" << endl;
        error_ = true;
        close(sock);
        return;
    }
    close(sock);
}

bool Smuggler::error() const
{
    return error_;
}
