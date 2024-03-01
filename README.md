# SMYT

## Network Traffic Monitoring Program

SMYT is security program that monitors network traffic and detects and reports incomplete `TCP handshakes`.

Many incomplete TCP handshakes in a short amount of time are usually a sign of port scanning or even the beginning
of DOS attacks, because software tools that scan for open ports (like `Nmap`) don't usually complete their handshakes.
The purpose of this program is then to detect such events and report them to the user, so that they know to quickly
take action.

Of course, there exist cases when incomplete TCP handshakes happen by accident (eg. the server or the client
crashed in the meantime). These events are detected by the program and they *could* become false positives.

*SMYT* is an acronym and it means *SMYT Monitors Your Traffic*. Not the best name... Yes,
[naming things](https://martinfowler.com/bliki/TwoHardThings.html) is hard.

## How Does SMYT Do Its Job

<!-- dnf install libpcap-devel.x86_64 -->
