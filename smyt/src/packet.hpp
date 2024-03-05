#pragma once

struct pcap_pkthdr;

namespace packet {
    void process_packet(unsigned char* user, const struct pcap_pkthdr* meta, const unsigned char* data);
}
