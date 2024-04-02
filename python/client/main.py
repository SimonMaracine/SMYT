import socket

HOST = "192.168.1.250"
PORT = 4898


def main():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))


if __name__ == "__main__":
    main()
