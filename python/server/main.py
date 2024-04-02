import socketserver

PORT = 4898


def main():
    try:
        with socketserver.TCPServer(("", 4898), socketserver.BaseRequestHandler) as server:
            print(f"Serving at port {PORT}")
            server.serve_forever()
    except KeyboardInterrupt:
        print()


if __name__ == "__main__":
    main()
