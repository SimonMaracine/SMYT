# SMYT

## Network Traffic Monitoring Program

SMYT is security program for the `Linux` platform that monitors network traffic, detects and reports possible
`DoS` attacks. It's almost an `IDS` program.

Too many `SYN` packets in a short amount of time are usually a sign of port scanning, or SYN scanning and they
could indicate the beginning of DoS attacks. The purpose of this program is then to detect such events and report
them to the user, so that they know to quickly take some action.

*SMYT* is an acronym and it means *SMYT Monitors Your Traffic*. Not the best name. But
[naming things](https://martinfowler.com/bliki/TwoHardThings.html) is hard.

## How Does SMYT Work

SMYT makes use of the `libpcap` library to capture `TCP` packets as they come and go through the operating system.
It then checks the flags to see which ones are `SYN` (Synchronize) packets and saves their source IP address for
potentially later use. There are three configurable parameteres that can decide if a particular situation is a
SYN scan or not:

- Warning threshold - number of SYN packets that if exceeded, triggers a warning log
- Panic theshold - number of SYN packets that if exceeded, triggers an alert log, meaning a SYN scan, and puts SMYT
  in a panic state
- Process interval - number of seconds between processing the stored packets

In panic mode, SMYT also takes action by blocking the source addresses from which the SYN packets come. It interacts
with the firewall through `iptables`.

## Service and CLI

SMYT comes in two forms: as a command line program and as a `systemd` service. The former is rather for testing
purposes and the latter is meant for actual use.

For managing the SMYT service there is a graphical user interface application written in `Python`. It is not
required, but it can be used to start/stop the service, see its status, see the logs and edit the configuration
file.

## Dependencies

- pcap
- systemd
- [json](https://github.com/nlohmann/json)
- [cargs](https://github.com/likle/cargs)

### Install

- dnf install libpcap-devel.x86_64
- dnf install systemd-devel.x86_64
- apt install libpcap-dev
- apt install libsystemd-dev
