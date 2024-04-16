#include "packet.hpp"

#include <cstring>

#include <pcap/pcap.h>

#include "capture.hpp"
#include "helpers.hpp"

/*
    timeval https://www.gnu.org/software/libc/manual/html_node/Time-Types.html

    ethernet https://en.wikipedia.org/wiki/Ethernet_frame
    ethertype https://en.wikipedia.org/wiki/EtherType
    ipv4 https://en.wikipedia.org/wiki/Internet_Protocol_version_4
    ip protocols https://en.wikipedia.org/wiki/List_of_IP_protocol_numbers
    endianess https://linux.die.net/man/3/ntohs
*/

namespace packet {
    void process_packet(unsigned char* user, const struct pcap_pkthdr* meta, const unsigned char* data) {
        capture::SessionData* session_data {reinterpret_cast<capture::SessionData*>(user)};

        struct ether_header ether;
        std::memcpy(&ether, data, sizeof(ether));

        // Check Ethernet II
        if (helpers::ntoh(ether.ether_type) < 1536u) {
            goto only_ether;
        }

        // Check EtherType IPv4
        if (helpers::ntoh(ether.ether_type) != 0x0800u) {
            goto only_ether;
        }

        struct ip ipv4;
        std::memcpy(&ipv4, data + sizeof(ether), sizeof(ipv4));

        if (ipv4.ip_p != 6u) {
            goto only_ipv4;
        }

        struct tcphdr tcp;
        std::memcpy(&tcp, data + sizeof(ether) + sizeof(ipv4), sizeof(tcp));

        session_data->callback(static_cast<long>(meta->ts.tv_sec), meta->caplen, &ether, &ipv4, &tcp, *session_data);
        return;

    only_ipv4:
        session_data->callback(static_cast<long>(meta->ts.tv_sec), meta->caplen, &ether, &ipv4, nullptr, *session_data);
        return;

    only_ether:
        session_data->callback(static_cast<long>(meta->ts.tv_sec), meta->caplen, &ether, nullptr, nullptr, *session_data);
    }
}
