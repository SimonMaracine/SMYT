import socket

HOST = "192.168.1.250"
PORT = 4898


def main():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        try:
            s.connect((HOST, PORT))
            print(f"Connected to {HOST} on {PORT}")
        except ConnectionRefusedError as err:
            print(err)


if __name__ == "__main__":
    main()
