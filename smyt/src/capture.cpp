#include "capture.hpp"

#include <iostream>
#include <string>

#include <pcap/pcap.h>

#include "error.hpp"

namespace capture {
    static pcap_t* g_handle {nullptr};

    void initialize() {
        char err_msg[PCAP_ERRBUF_SIZE];

        if (pcap_init(PCAP_CHAR_ENC_UTF_8, err_msg) == PCAP_ERROR) {
            throw error::PcapError("Could not initialize pcap: " + std::string(err_msg));
        }
    }

    void uninitialize() {

    }

    void start_session() {

    }

    void stop_session() {

    }

    void capture_loop() {

    }

    void break_loop() {
        if (g_handle != nullptr) {
            pcap_breakloop(g_handle);
        }
    }
}
