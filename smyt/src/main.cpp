// #include <csignal>
// #include <iostream>

// #include <pcap/pcap.h>

// static pcap_t* handle {nullptr};

// static void signal_handler(int) {
//     pcap_breakloop(handle);
// }

// static void packet(unsigned char*, const struct pcap_pkthdr* header, const unsigned char* packet) {
//     std::cout << "Packet\n";
// }

// int main(int argc, char** argv) {
//     if (std::signal(SIGINT, signal_handler) == SIG_ERR) {
//         std::cerr << "Error signal\n";
//         return 1;
//     }

//     if (argc < 2) {
//         std::cerr << "Error arguments\n";
//         return 1;
//     }

//     char err_msg[PCAP_ERRBUF_SIZE];

//     if (pcap_init(PCAP_CHAR_ENC_UTF_8, err_msg) == PCAP_ERROR) {
//         std::cerr << "Error init\n";
//         return 1;
//     }

//     handle = pcap_create(argv[2u], err_msg);

//     if (handle == nullptr) {
//         std::cerr << "Error create\n";
//         return 1;
//     }

//     if (pcap_set_snaplen(handle, 65535) == PCAP_ERROR_ACTIVATED) {
//         std::cerr << "Error snaplen\n";
//         return 1;
//     }

//     if (pcap_set_promisc(handle, 1) == PCAP_ERROR_ACTIVATED) {
//         std::cerr << "Error promisc\n";
//         return 1;
//     }

//     if (pcap_set_timeout(handle, 1000) == PCAP_ERROR_ACTIVATED) {
//         std::cerr << "Error timeout\n";
//         return 1;
//     }

//     if (pcap_set_buffer_size(handle, 4096) == PCAP_ERROR_ACTIVATED) {
//         std::cerr << "Error buffer size\n";
//         return 1;
//     }

//     if (pcap_activate(handle) < 0) {
//         std::cerr << "Error activate\n";
//         return 1;
//     }

//     // const int headers_type {pcap_datalink(handle)};

//     // if (headers_type != DLT_EN10MB) {
//     //     std::cerr << "Error datalink\n";
//     //     return 1;
//     // }

//     const int result {pcap_loop(handle, 20, packet, nullptr)};

//     if (result < 0 && result != PCAP_ERROR_BREAK) {
//         std::cerr << "Error loop\n";
//         return 1;
//     }

//     pcap_close(handle);

//     return 0;
// }

#include <iostream>
#include <csignal>

#include "capture.hpp"
#include "error.hpp"

static void signal_handler(int) {
    capture::break_loop();
}

int main(int argc, char** argv) {
    if (std::signal(SIGINT, signal_handler) == SIG_ERR) {
        std::cerr << "Could not setup interrupt handler\n";
        return 1;
    }

    try {
        capture::initialize();
    } catch (const error::PcapError& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }

    capture::start_session();
    capture::capture_loop();
    capture::stop_session();

    capture::uninitialize();

    return 0;
}
