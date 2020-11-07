#include "sniffer.h"

Sniffer::Sniffer(const string &interface)
{
    setUp(interface);
    if(!error_)
        sniffPackets();
}

bool Sniffer::error() const
{
    return error_;
}

void Sniffer::setUp(const string &interface)
{
    // Checking opening of device
    if((handle_ = pcap_open_live(interface.c_str(), BUFSIZ, 1, 1000, errorBuffer_)) == nullptr)
        error("Can't open device " + interface + ".\n"
              + "Error message: " + errorBuffer_ + "\n");

    if(pcap_datalink(handle_) != DLT_EN10MB)
        error(interface + " doesn't provide Ethernet headers.");

    // Filter for RIP and RIPng
    filterSettings_ = "portrange 520-521"; //Set port range we want to sniff (520 RIP, 521 RIPng)

    // Compiling filter
    if(pcap_compile(handle_, &filter_, filterSettings_.c_str(), 0, net_) == -1)
        error("Can't parse filter " + filterSettings_);

    // Setting up filter
    if(pcap_setfilter(handle_, &filter_) == -1)
        error("Can't set up filter " + filterSettings_);
}

void Sniffer::sniffPackets()
{
    // Sniffing untill error or break via SIGTERM
    pcap_loop(handle_, -1, gotPacket, nullptr);
}

void Sniffer::gotPacket(u_char *args, const pcap_pkthdr *header, const u_char *packet)
{
    UNUSED(args);
    //Parsing packet
    cout << "Destination: ";
    for(int i = 0; i < 6; i++)
    {
        printf("%02x", packet[i]);
        cout <<  (i < 5 ? ":" : "\n");
    }
    cout << "Source: ";
    for(int i = 6; i < 12; i++)
    {
        printf("%02x", packet[i]);
        cout <<  (i < 11 ? ":" : "\n");
    }
    if(packet[12] == 0x08 && packet[13] == 0x00)
    {
        //IPv4
        cout << "IPv4 packet" << endl;
        cout << "IPv4 source: ";
        for(int i = 26; i < 30; i++)
        {
            printf("%d", packet[i]);
            cout << (i < 29 ? "." : "\n");
        }
        cout << "IPv4 destination: ";
        for(int i = 30; i < 34; i++)
        {
            printf("%d", packet[i]);
            cout << (i < 33 ? "." : "\n");
        }

        cout << "Source port: ";
        cout << (packet[34] << 8 | packet[35]) << endl;
        cout << "Destination port: ";
        cout << (packet[36] << 8 | packet[37]) << endl;

        //RIP
        cout << "----RIP----" << endl;
        cout << "Command: ";
        if(packet[RIPv1_OFFSET] == 0x02)
            cout << "Response" << endl;
        else if(packet[RIPv1_OFFSET == 0x01])
            cout << "Request" << endl;
        else
        {
            cout << "Invalid command" << endl;
        }
        cout << "Version: ";
        if(packet[RIPv1_OFFSET+1] == 0x01)
        {
            //RIPv1
            cout << "RIPv1" << endl;
            int recordCount = (static_cast<int>(header->len) - RIPv1_OFFSET - 4)/20; // count of entries
            for(int no = 0; no < recordCount; no++)
            {
                int recordOffset = no*20;
                cout << "----Entry no." << no << "----\n";

                cout << "Address family: " << ((packet[RIPv1_OFFSET + 4+recordOffset] == 0x00 && packet[RIPv1_OFFSET + 5+recordOffset] == 0x02) ? "IP" : "Unknown") << "\n";

                cout << "IP address: ";
                for(int i = RIPv1_OFFSET + 8 + recordOffset; i < (RIPv1_OFFSET + 12 + recordOffset); i++)
                {
                    printf("%d", packet[i]);
                    cout <<  (i < (RIPv1_OFFSET + 11 + recordOffset) ? "." : "\n");
                }

                cout << "Metric: ";
                // Metric starts from 20 to 23
                if(packet[RIPv1_OFFSET+20 + recordOffset] != 0x00 || packet[RIPv1_OFFSET+21 + recordOffset] != 0x00 || packet[RIPv1_OFFSET+22 + recordOffset] != 0x00)
                    cout << "Invalid value" << endl;
                else if( packet[RIPv1_OFFSET+23 + recordOffset] == 0x10 )
                {
                    cout << "Unreachable (16)" << endl;
                }
                else
                {
                    printf("%d", packet[RIPv1_OFFSET+23 + recordOffset]);
                    cout << endl;
                }
            }
        }
        else
        {
            //RIPv2
            cout << "RIPv2" << endl;
            int recordCount = (static_cast<int>(header->len) - RIPv1_OFFSET -4)/20;
            for(int no = 0; no < recordCount; no++)
            {
                int recordOffset = no*20;
                cout << "----Entry no." << no << "----\n";

                if((packet[RIPv1_OFFSET + 4+recordOffset] == 0xff && packet[RIPv1_OFFSET + 5+recordOffset] == 0xff))
                {
                    cout << "Authentication type: ";
                    if(packet[RIPv1_OFFSET+6+recordOffset] == 0x00 && packet[RIPv1_OFFSET+7+recordOffset] == 0x02)
                        cout << "Simple password" << endl;
                    else
                        cout << "Invalid" << endl;

                    cout << "Authentication password: ";
                    for(int i = 0; i < 16; i++)
                    {
                        printf("%c", packet[RIPv1_OFFSET+8+i]);
                    }
                    cout << endl;

                }
                else
                {
                    cout << "Address family: " << ((packet[RIPv1_OFFSET + 4+recordOffset] == 0x00 && packet[RIPv1_OFFSET + 5+recordOffset] == 0x02) ? "IP" : "Unknown") << "\n";

                    cout << "Route tag: ";
                    printf("0x%02x%02x\n", packet[RIPv1_OFFSET + 6+recordOffset], packet[RIPv1_OFFSET + 7+recordOffset]);

                    cout << "IP address: ";
                    for(int i = RIPv1_OFFSET + 8 + recordOffset; i < (RIPv1_OFFSET + 12 + recordOffset); i++)
                    {
                        printf("%d", packet[i]);
                        cout <<  (i < (RIPv1_OFFSET + 11 + recordOffset) ? "." : "\n");
                    }

                    cout << "Netmask: ";
                    for(int i = (RIPv1_OFFSET+12 + recordOffset); i < (RIPv1_OFFSET+16 + recordOffset); i++)
                    {
                        printf("%d", packet[i]);
                        cout <<  (i < (RIPv1_OFFSET + 15 + recordOffset) ? "." : "\n");
                    }

                    cout << "Next hop: ";
                    for(int i = (RIPv1_OFFSET+16 + recordOffset); i < (RIPv1_OFFSET+20 + recordOffset); i++)
                    {
                        printf("%d", packet[i]);
                        cout <<  (i < (RIPv1_OFFSET+19 + recordOffset) ? "." : "\n");
                    }

                    // Metric starts from 20 to 23
                    cout << "Metric: ";
                    if(packet[RIPv1_OFFSET+20 + recordOffset] != 0x00 || packet[RIPv1_OFFSET+21 + recordOffset] != 0x00 || packet[RIPv1_OFFSET+22 + recordOffset] != 0x00)
                        cout << "Invalid value" << endl;
                    else if( packet[RIPv1_OFFSET+23 + recordOffset] == 0x10 )
                    {
                        cout << "Unreachable (16)" << endl;
                    }
                    else
                    {
                        printf("%d", packet[RIPv1_OFFSET+23 + recordOffset]);
                        cout << endl;
                    }
                }
            }
        }

    }
    else
    {
        //IPv6
        cout << "IPv6 packet" << endl;

        cout << "IPv6 source: ";
        for(int i = 22; i < 38; i++)
        {
            printf("%02x", packet[i]);
            cout << ((i % 2 == 1) ? ((i < 37) ? ":" : "\n") : "");
        }

        cout << "IPv6 destination: ";
        for(int i = 38; i < 54; i++)
        {
            printf("%02x", packet[i]);
            cout << ((i % 2 == 1) ? ((i < 53) ? ":" : "\n") : "");
        }

        cout << "Source port: ";
        cout << (packet[54] << 8 | packet[55]) << endl;
        cout << "Destination port: ";
        cout << (packet[56] << 8 | packet[57]) << endl;

        cout << "----RIPng----" << endl;
        cout << "Command: ";
        if(packet[RIPng_OFFSET] == 0x02)
            cout << "Response" << endl;
        else if (packet[RIPng_OFFSET] == 0x01)
            cout << "Request" << endl;
        else
            cout << "Undefined" << endl;
        printf("Version: %x\n", packet[RIPng_OFFSET+1]);
        int packetCount = (static_cast<int>(header->len) - RIPng_OFFSET - 4)/20;
        for(int no = 0; no < packetCount; no++)
        {
            int recordOffset = no*20;
            cout << "----Entry no." << no << "----\n";
            cout << "IPv6 prefix: ";
            for(int i = RIPng_OFFSET+4+recordOffset; i < RIPng_OFFSET+20+recordOffset; i++)
            {
                printf("%02x", packet[i]);
                cout << ((i % 2 == 1) ? ((i < RIPng_OFFSET+19+recordOffset) ? ":" : "\n") : "");
            }
            printf("Route Tag: 0x%x%x\n", packet[RIPng_OFFSET+20+recordOffset], packet[RIPng_OFFSET+21+recordOffset]);
            printf("Prefix Lenght: %d\n", packet[RIPng_OFFSET+22+recordOffset]);
            printf("Metric: %d\n", packet[RIPng_OFFSET+23+recordOffset]);
        }
    }
    cout << "--------------------------------------------------------------------" << endl;

}

void Sniffer::error(const string &str)
{
    cerr << str;
    error_ = true;
}
