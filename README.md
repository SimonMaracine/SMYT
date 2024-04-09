# SMYT

## Network Traffic Monitoring Program

SMYT is security program for the `Linux` platform that monitors network traffic, detects and reports possible
`DOS` attacks.

Many incomplete `TCP handshakes` in a short amount of time are usually a sign of port scanning or even the
beginning of DOS attacks, because software tools that scan for open ports (like `Nmap`) don't usually complete
their handshakes. The purpose of this program is then to detect such events and report them to the user, so that
they know to quickly take some action.

Of course, there are cases when incomplete TCP handshakes occur by accident (eg. the server or the client
crashed in the meantime). These events are detected by the program and they *could* become false positives.

*SMYT* is an acronym and it means *SMYT Monitors Your Traffic*. Not the best name... But
[naming things](https://martinfowler.com/bliki/TwoHardThings.html) is hard.

## Initial Idea

The idea of this security program is to monitor network traffic, `TCP` packets more exactly, to figure out
when `SYN` scans are executed and report these events to the user. Later I decided to make SMYT also take
some action when these events occur.

<!-- TODO see if things change -->

## How Does SMYT Do Its Job

SMYT makes use of the `libpcap` library to capture IP packets as they come and go through the operating system.

<!-- dnf install libpcap-devel.x86_64 -->
<!-- dnf install systemd-devel.x86_64 -->
<!-- apt install libpcap-dev -->
