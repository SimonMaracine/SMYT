from scapy.all import *


def main():
    packet = IP(dst="localhost")
    print(packet)

    send(packet)


if __name__ == "__main__":
    main()
