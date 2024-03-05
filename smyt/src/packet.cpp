#include "packet.hpp"

#include <iostream>
#include <ctime>

#include <pcap/pcap.h>

/*
    timeval https://www.gnu.org/software/libc/manual/html_node/Time-Types.html
*/

namespace packet {
    void process_packet(unsigned char* user, const struct pcap_pkthdr* meta, const unsigned char* data) {
        std::cout << "Packet: timestamp " << meta->ts.tv_sec << ", lentgth " << meta->caplen << '\n';
    }
}
