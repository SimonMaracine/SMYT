import socketserver

PORT = 4898


class Handler(socketserver.BaseRequestHandler):
    def handle(self):
        print(f"{self.client_address} connected")


def main():
    try:
        with socketserver.TCPServer(("", PORT), Handler) as server:
            print(f"Serving on port {PORT}")
            server.serve_forever()
    except KeyboardInterrupt:
        print()


if __name__ == "__main__":
    main()
