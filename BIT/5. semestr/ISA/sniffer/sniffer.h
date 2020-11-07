#ifndef SNIFFER_H
#define SNIFFER_H

#define UNUSED(x) (void)(x)
#define RIPv1_OFFSET 42
#define RIPng_OFFSET 62

#include <string>
#include <pcap/pcap.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>

using namespace std;

class Sniffer
{
public:
    //Functions
    Sniffer(const string &interface);

    bool error() const;

private:
    //Functions
    //! Sets up filter for sniffing packets
    void setUp(const string &interface);

    //! Starts to sniff packets
    void sniffPackets();

    //! Callback to when packet is sniffed
    static void gotPacket(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

    //! Prints out error and sets error to true
    void error(const string &str);

private:
    //Attributes
    bool error_ = false;

    pcap_t *handle_;

    char errorBuffer_[PCAP_ERRBUF_SIZE];

    struct bpf_program filter_;

    string filterSettings_;

    bpf_u_int32 net_;

    bpf_u_int32 mask_;
};

#endif // SNIFFER_H
