#include "capture.hpp"

#include <cassert>

#include <pcap/pcap.h>

#include "error.hpp"
#include "packet.hpp"

/*
    main https://www.tcpdump.org/manpages/libpcap-1.10.4/pcap.3pcap.html
    loop https://www.tcpdump.org/manpages/libpcap-1.10.4/pcap_loop.3pcap.html
    break https://www.tcpdump.org/manpages/libpcap-1.10.4/pcap_breakloop.3pcap.html
    link layer https://www.tcpdump.org/linktypes.html
    error msg https://www.tcpdump.org/manpages/libpcap-1.10.4/pcap_statustostr.3pcap.html
*/

namespace capture {
    static pcap_t* g_handle {nullptr};

    static void start_capture_session(const std::string& device) {
        char err_msg[PCAP_ERRBUF_SIZE];

        pcap_t* handle {pcap_create(device.c_str(), err_msg)};

        if (handle == nullptr) {
            throw error::PcapError("Could not create device handle `" + device + "`: " + err_msg);
        }

        g_handle = handle;

        if (pcap_set_snaplen(handle, 65535) == PCAP_ERROR_ACTIVATED) {  // TODO
            throw error::PcapError("Could not set snaplen\n");
        }

        if (pcap_set_promisc(handle, 1) == PCAP_ERROR_ACTIVATED) {
            throw error::PcapError("Could not set promisc\n");
        }

        if (pcap_set_timeout(handle, 1000) == PCAP_ERROR_ACTIVATED) {
            throw error::PcapError("Could not set timeout\n");
        }

        if (pcap_set_buffer_size(handle, 4096) == PCAP_ERROR_ACTIVATED) {
            throw error::PcapError("Could not set buffer size\n");
        }

        {
            const int result {pcap_activate(handle)};

            if (result > 0) {
                throw error::PcapError("Warning activating device `" + device + "`: " + pcap_statustostr(result));
            } if (result < 0) {
                throw error::PcapError("Could not activate device `" + device + "`: " + pcap_statustostr(result));
            }
        }

        {
            const int result {pcap_datalink(handle)};  // FIXME

            if (result != DLT_EN10MB) {
                throw error::PcapError("Error datalink");
            }
        }
    }

    void initialize() {
        char err_msg[PCAP_ERRBUF_SIZE];

        if (pcap_init(PCAP_CHAR_ENC_UTF_8, err_msg) == PCAP_ERROR) {
            throw error::PcapError("Could not initialize pcap: " + std::string(err_msg));
        }
    }

    void uninitialize() {

    }

    void start_session(const std::string& device) {
        if (device.empty()) {
            // TODO automatically pick one
        }

        try {
            start_capture_session(device);
        } catch (const error::PcapError&) {
            stop_session();
            throw;
        }
    }

    void stop_session() {
        pcap_close(g_handle);
        g_handle = nullptr;
    }

    void capture_loop() {
        const int result {pcap_loop(g_handle, -1, packet::process_packet, nullptr)};

        if (result >= 0) {
            assert(false);
        } else if (result == PCAP_ERROR_BREAK) {
            return;
        } else {
            throw error::PcapError("An error occurred while capturing: " + std::string(pcap_geterr(g_handle)));
        }
    }

    void break_loop() {
        if (g_handle != nullptr) {
            pcap_breakloop(g_handle);
        }
    }

    const char* get_library_version() {
        return pcap_lib_version();
    }
}
