# notes

- Tried using the notification system in the DE to send notifications, but it didn't really work, because SMYT
  must run as root.
- Replaced notifications with logging. (Logging was on my TODO list anyway.) Log file resides in /var/log/smyt.
- Tried to send TCP packets from the same computer that runs SMYT, to see if SMYT detects the SYN. Didn't
  work, because those packets probably didn't go through the network. Not even Wireshark could capture them.
- Tried then to send those TCP packets from my laptop and SMYT did capture them.
- Wrote a tiny TCP server in Python (that does nothing) and also a TCP client to test the handshake detection.
- Deleted the code that detects TCP handshakes and replaced it with detecting SYN scans, i.e. too many SYN packets
  in a short amount of time
- Included nlohmann/json as a dependency for managing the configuration file
- Tried to make the configuration system as solid as possible
- Improved the code here and there, and fixed some bugs
