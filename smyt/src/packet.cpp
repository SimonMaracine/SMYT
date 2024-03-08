#include "packet.hpp"

#include <pcap/pcap.h>

#include "capture.hpp"

/*
    timeval https://www.gnu.org/software/libc/manual/html_node/Time-Types.html
*/

namespace packet {
    void process_packet(unsigned char* user, const struct pcap_pkthdr* meta, const unsigned char* data) {
        // TODO process

        const auto callback {reinterpret_cast<capture::PacketCallback>(user)};
        callback(static_cast<long>(meta->ts.tv_sec), meta->caplen, nullptr, nullptr, nullptr);
    }
}
