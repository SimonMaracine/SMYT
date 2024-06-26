#include "capture.hpp"

#include <cassert>
#include <utility>
#include <type_traits>
#include <ostream>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#if SMYT_LOG_PACKETS
    #include <sstream>
    #include <iomanip>
#endif

#include <pcap/pcap.h>

#include "error.hpp"
#include "packet.hpp"
#include "logging.hpp"
#include "helpers.hpp"

#define BLOCKED_LIST_FILE_PATH "/var/log/smyt/blocked"

/*
    main https://www.tcpdump.org/manpages/libpcap-1.10.4/pcap.3pcap.html
    loop https://www.tcpdump.org/manpages/libpcap-1.10.4/pcap_loop.3pcap.html
    break https://www.tcpdump.org/manpages/libpcap-1.10.4/pcap_breakloop.3pcap.html
    link layer https://www.tcpdump.org/linktypes.html
    error msg https://www.tcpdump.org/manpages/libpcap-1.10.4/pcap_statustostr.3pcap.html
    packet timeout https://www.tcpdump.org/manpages/libpcap-1.10.4/pcap_set_timeout.3pcap.html
    buffer size https://www.tcpdump.org/manpages/libpcap-1.10.4/pcap_set_buffer_size.3pcap.html
    devices https://www.tcpdump.org/manpages/libpcap-1.10.4/pcap_findalldevs.3pcap.html
    compile https://www.tcpdump.org/manpages/libpcap-1.10.4/pcap_compile.3pcap.html
    filter https://www.tcpdump.org/manpages/libpcap-1.10.4/pcap_setfilter.3pcap.html
    filter https://www.tcpdump.org/manpages/libpcap-1.10.4/pcap-filter.7.html
    direction https://www.tcpdump.org/manpages/libpcap-1.10.4/pcap_setdirection.3pcap.html

    ethernet https://en.wikipedia.org/wiki/Ethernet_frame
    tcp min size https://superuser.com/questions/243008/whats-the-minimum-size-of-a-tcp-packet

    ntop https://linux.die.net/man/3/inet_ntop

    condition variable https://en.cppreference.com/w/cpp/thread/condition_variable
    system https://en.cppreference.com/w/cpp/utility/program/system
*/

namespace capture {
    namespace internal {
        static pcap_t* g_handle {nullptr};

        static constexpr int SNAPLEN {64};
        static constexpr int BUFFER_SIZE {8192};
        static constexpr int TIMEOUT {700};

        // 8192 / 64 = 128 packets in buffer

        static const char* FILTER {"tcp[tcpflags] & tcp-syn != 0"};

        static void create_capture_session(const std::string& device) {
            char err_msg[PCAP_ERRBUF_SIZE];

            pcap_t* handle {pcap_create(device.c_str(), err_msg)};

            if (handle == nullptr) {
                throw error::PcapError("Could not create device handle `" + device + "`: " + err_msg);
            }

            g_handle = handle;

            if (pcap_set_promisc(handle, 1) == PCAP_ERROR_ACTIVATED) {
                throw error::PcapError("Could not set promisc");
            }

            // Enable buffering
            if (pcap_set_immediate_mode(handle, 0) == PCAP_ERROR_ACTIVATED) {
                throw error::PcapError("Could not set immediate mode");
            }

            // Set maximum snapshot length, as we only care about TCP
            if (pcap_set_snaplen(handle, SNAPLEN) == PCAP_ERROR_ACTIVATED) {
                throw error::PcapError("Could not set snaplen");
            }

            // Set the buffer size of the packets
            if (pcap_set_buffer_size(handle, BUFFER_SIZE) == PCAP_ERROR_ACTIVATED) {
                throw error::PcapError("Could not set buffer size");
            }

            // Process packets periodically in bursts
            if (pcap_set_timeout(handle, TIMEOUT) == PCAP_ERROR_ACTIVATED) {
                throw error::PcapError("Could not set timeout");
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
                const int result {pcap_datalink(handle)};

                if (result != DLT_EN10MB) {
                    throw error::PcapError("Link-layer type is not Ethernet");
                }
            }

            {
                struct bpf_program filter;

                if (pcap_compile(handle, &filter, FILTER, 1, PCAP_NETMASK_UNKNOWN) < 0) {
                    throw error::PcapError("Could not compile filter program: " + std::string(pcap_geterr(handle)));
                }

#if 1
                if (pcap_setfilter(handle, &filter) < 0) {
                    pcap_freecode(&filter);
                    throw error::PcapError("Could not set filter: " + std::string(pcap_geterr(handle)));
                }
#endif

                pcap_freecode(&filter);
            }

            if (pcap_setdirection(handle, PCAP_D_IN) < 0) {
                throw error::PcapError("Could not set direction: " + std::string(pcap_geterr(handle)));
            }
        }

        static std::vector<Device> process_devices_and_get_default(const pcap_if_t* devs) {
            std::vector<Device> result;

            const pcap_if_t* dev {devs};

            while (dev != nullptr) {
                Device device;
                device.name = dev->name;

                if (dev->description != nullptr) {
                    device.description = dev->description;
                }

                result.push_back(std::move(device));

                dev = dev->next;
            }

            return result;
        }

        static void process_packet(
            long timestamp,
            const struct ip* ipv4,
            const struct tcphdr* tcp,
            SessionData& session_data
        ) {
            if (helpers::ntoh(tcp->syn) && !helpers::ntoh(tcp->ack)) {
                static_assert(std::is_same_v<std::uint32_t, in_addr_t>);

                SynPacket packet;
                packet.src_address = ipv4->ip_src.s_addr;
                packet.timestamp = timestamp;

                std::lock_guard<std::mutex> lock {session_data.mutex};
                session_data.state.syn_packets.push_back(packet);
            }
        }

        static std::vector<std::uint32_t> read_blocked_attackers(std::ostream* err_stream) {
            std::ifstream stream {BLOCKED_LIST_FILE_PATH};

            if (!stream.is_open()) {
                try {
                    logging::log("Error reading blocked addresses from file `" BLOCKED_LIST_FILE_PATH "`");
                } catch (const error::LogError& e) {
                    if (err_stream) {
                        *err_stream << smyt << e.what() << '\n';
                    }
                }

                return {};
            }

            std::vector<std::uint32_t> result;

            std::string line;

            while (std::getline(stream, line)) {
                const auto converted {helpers::pton(line.c_str())};

                if (converted) {
                    result.push_back(*converted);
                }
            }

            return result;
        }

        static std::vector<std::uint32_t> retrieve_blocked_attackers(std::ostream* err_stream) {
            const char* command {R"(iptables -L INPUT -n | awk 'NR > 2 && $1 == "DROP" && $2 == "0" { print $4 }' > )" BLOCKED_LIST_FILE_PATH};
            const int result {std::system(command)};

            if (WEXITSTATUS(result) != 0) {
                try {
                    logging::log("Error retrieving blocked addresses from iptables");
                } catch (const error::LogError& e) {
                    if (err_stream) {
                        *err_stream << smyt << e.what() << '\n';
                    }
                }

                return {};
            }

            return read_blocked_attackers(err_stream);
        }

        static void block_attacker_by_address(std::uint32_t ip_address, State& state, std::ostream* err_stream) {
            // iptables -C INPUT --protocol all --source ip_address --jump DROP
            // iptables -A INPUT --protocol all --source ip_address --jump DROP

            const auto address {helpers::ntop(&ip_address)};

            // Address is invalid, can't do anything about it
            if (!address) {
                return;
            }

            const std::string rule {"INPUT -p all -s " + *address + " -j DROP"};

            // Check firewall rule first
            {
                const std::string command {"iptables -C " + rule};
                const int result {std::system(command.c_str())};

                if (WEXITSTATUS(result) == 0) {
                    try {
                        logging::log("Firewall rule `" + rule + "` already exists");
                    } catch (const error::LogError& e) {
                        if (err_stream) {
                            *err_stream << smyt << e.what() << '\n';
                        }
                    }

                    return;
                }
            }

            // Add firewall rule
            {
                const std::string command {"iptables -A " + rule};
                const int result {std::system(command.c_str())};

                if (WEXITSTATUS(result) != 0) {
                    try {
                        logging::log("Error adding a new firewall rule: " + rule);
                    } catch (const error::LogError& e) {
                        if (err_stream) {
                            *err_stream << smyt << e.what() << '\n';
                        }
                    }

                    return;
                }

                try {
                    logging::log("Successfully added a new firewall rule: " + rule);
                } catch (const error::LogError& e) {
                    if (err_stream) {
                        *err_stream << smyt << e.what() << '\n';
                    }
                }

                state.blocked_addresses.push_back(ip_address);
            }
        }

        static void find_attacker_to_block(State& state, std::ostream* err_stream) {
            std::vector<std::uint32_t> addresses;

            // Gather addresses that are not already blocked
            for (const auto& packet : state.syn_packets) {
                if (std::find(state.blocked_addresses.cbegin(), state.blocked_addresses.cend(), packet.src_address) != state.blocked_addresses.cend()) {
                    continue;
                }

                addresses.push_back(packet.src_address);
            }

            std::uint32_t address {};
            std::size_t frequency {};

            // Find the most frequent address
            if (!helpers::most_common(addresses, address, frequency)) {
                return;
            }

            // Ignore, if that address didn't sent too many packets in the process interval
            if (frequency < 10u) {
                return;
            }

            // Address can't be already blocked, so block it
            block_attacker_by_address(address, state, err_stream);
        }

        static void process_data_so_far(State& state, const configuration::Config& config, std::ostream* err_stream) {
#if SMYT_PRINT_PROCESSING
            if (err_stream) {
                *err_stream << "Processing...\n";
            }
#endif

            // Packets since last process
            const auto packets_interval {state.syn_packets.size()};

            if (state.panic_mode) {
                state.syn_packet_count += packets_interval;

                if (packets_interval <= config.panic_threshold) {
                    state.panic_mode = false;
                    const auto total {std::exchange(state.syn_packet_count, 0u)};

                    try {
                        logging::log("SYN scan is over. " + std::to_string(total) + " SYN packets in total.");
                    } catch (const error::LogError& e) {
                        if (err_stream) {
                            *err_stream << smyt << e.what() << '\n';
                        }
                    }

                    goto purge_data;
                }

                find_attacker_to_block(state, err_stream);

                goto purge_data;
            }

            if (packets_interval > config.panic_threshold) {
                state.panic_mode = true;
                state.syn_packet_count += packets_interval;

                try {
                    logging::log("Alert! SYN scan detected! " + std::to_string(packets_interval) + " SYN packets so far.");
                } catch (const error::LogError& e) {
                    if (err_stream) {
                        *err_stream << smyt << e.what() << '\n';
                    }
                }

                find_attacker_to_block(state, err_stream);
            } else if (packets_interval > config.warning_threshold) {
                try {
                    logging::log("Warning! Too many SYN packets. " + std::to_string(packets_interval) + " in the last interval.");
                } catch (const error::LogError& e) {
                    if (err_stream) {
                        *err_stream << smyt << e.what() << '\n';
                    }
                }
            }

        purge_data:
            state.syn_packets.clear();
        }

        static void packet(
            long timestamp,
            [[maybe_unused]] std::size_t length,
            [[maybe_unused]] const struct ether_header* ether,
            const struct ip* ipv4,
            const struct tcphdr* tcp,
            SessionData& session_data
        ) {
#if SMYT_LOG_PACKETS
            std::ostringstream stream;

            stream.fill('0');

            if (ether == nullptr) {
                return;
            }

            stream << "Ether ";

            stream << std::hex
                << std::setw(2) << static_cast<unsigned int>(ether->ether_shost[0u])
                << ':'
                << std::setw(2) << static_cast<unsigned int>(ether->ether_shost[1u])
                << ':'
                << std::setw(2) << static_cast<unsigned int>(ether->ether_shost[2u])
                << ':'
                << std::setw(2) << static_cast<unsigned int>(ether->ether_shost[3u])
                << ':'
                << std::setw(2) << static_cast<unsigned int>(ether->ether_shost[4u])
                << ':'
                << std::setw(2) << static_cast<unsigned int>(ether->ether_shost[5u])
                << " -> "
                << std::setw(2) << static_cast<unsigned int>(ether->ether_dhost[0u])
                << ':'
                << std::setw(2) << static_cast<unsigned int>(ether->ether_dhost[1u])
                << ':'
                << std::setw(2) << static_cast<unsigned int>(ether->ether_dhost[2u])
                << ':'
                << std::setw(2) << static_cast<unsigned int>(ether->ether_dhost[3u])
                << ':'
                << std::setw(2) << static_cast<unsigned int>(ether->ether_dhost[4u])
                << ':'
                << std::setw(2) << static_cast<unsigned int>(ether->ether_dhost[5u]);

            if (ipv4 == nullptr) {
                return;
            }

            const auto src {helpers::ntop(&ipv4->ip_src).value_or("<invalid>")};
            const auto dst {helpers::ntop(&ipv4->ip_dst).value_or("<invalid>")};

            stream << std::dec << " IPv4 " << src << " -> " << dst;

            if (tcp == nullptr) {
                return;
            }

            stream << " TCP";

            try {
                logging::log(stream.str());
            } catch (const error::LogError& e) {
                if (session_data.err_stream) {
                    *session_data.err_stream << smyt << e.what() << '\n';
                }
            }
#endif

            if (tcp == nullptr) {
                return;
            }

            process_packet(timestamp, ipv4, tcp, session_data);
        }

        static void processing(SessionData& data) {
            while (true) {
                std::unique_lock<std::mutex> lock {data.mutex};
                data.cv.wait_for(lock, std::chrono::seconds(data.config.process_interval), [&data]() {
                    const auto now {std::chrono::seconds(std::time(nullptr)).count()};

                    return now - data.last_process >= data.config.process_interval || !data.processing;
                });

                if (!data.processing) {
                    break;
                }

                process_data_so_far(data.state, data.config, data.err_stream);
                data.last_process = std::chrono::seconds(std::time(nullptr)).count();
            }
        }
    }

    std::optional<Device> initialize() {
        char err_msg[PCAP_ERRBUF_SIZE];

        if (pcap_init(PCAP_CHAR_ENC_UTF_8, err_msg) < 0) {
            throw error::PcapError("Could not initialize pcap: " + std::string(err_msg));
        }

        pcap_if_t* devs {nullptr};

        if (pcap_findalldevs(&devs, err_msg) < 0) {
            throw error::PcapError("Could not retrieve devices list: " + std::string(err_msg));
        }

        if (devs == nullptr) {
            // No default device
            return std::nullopt;
        }

        // This can throw bad_alloc, but who cares; in fact I use strings everywhere
        const auto devices {internal::process_devices_and_get_default(devs)};

        pcap_freealldevs(devs);

        assert(!devices.empty());

        // The first one should be fine
        return devices.front();
    }

    void create_session(const std::string& device) {
        assert(!device.empty());

        try {
            internal::create_capture_session(device);
        } catch (const error::PcapError&) {
            destroy_session();
            throw;
        }
    }

    void destroy_session() {
        pcap_close(internal::g_handle);
        internal::g_handle = nullptr;
    }

    void capture_loop(const configuration::Config& config, std::ostream* err_stream) {
        SessionData data;
        data.config = config;
        data.err_stream = err_stream;
        data.callback = internal::packet;
        data.state.blocked_addresses = internal::retrieve_blocked_attackers(err_stream);
        data.thread = std::thread(internal::processing, std::ref(data));

        try {
            logging::log("Read " + std::to_string(data.state.blocked_addresses.size()) + " blocked IP address(es)");
        } catch (const error::LogError& e) {
            if (err_stream) {
                *err_stream << smyt << e.what() << '\n';
            }
        }

        const int result {
            pcap_loop(
                internal::g_handle,
                -1,
                packet::process_packet,
                reinterpret_cast<unsigned char*>(&data)
            )
        };

        // Signal processing thread to stop
        {
            std::lock_guard<std::mutex> lock {data.mutex};
            data.processing = false;
        }
        data.cv.notify_one();

        // Wait for the thread
        data.thread.join();

        if (result >= 0) {
            assert(false);
        } else if (result != PCAP_ERROR_BREAK) {
            throw error::PcapError("An error occurred while capturing: " + std::string(pcap_geterr(internal::g_handle)));
        }
    }

    void break_loop() {
        if (internal::g_handle != nullptr) {
            pcap_breakloop(internal::g_handle);
        }
    }

    const char* get_library_version() {
        return pcap_lib_version();
    }
}
