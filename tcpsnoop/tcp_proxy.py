#!/usr/bin/env python3
"""
TCP Proxy with Recording & Playback (Multithreaded)

- Listens on port 5557 for the client application.
- Connects to the actual server on port 5556.
- Runs a separate thread for control commands on port 6616.
- Commands:
    - 'start-recording' → Records server-to-client messages.
    - 'stop-recording' → Stops recording and saves data.
    - 'playback' → Replays recorded messages to the client.

Usage:
    python tcp_proxy.py
"""

import socket
import threading
import select

PROXY_PORT = 5557
SERVER_HOST = "127.0.0.1"
SERVER_PORT = 5556
CONTROL_PORT = 6616
RECORD_FILE = "recorded_data.bin"

recording = False
recorded_data = []
lock = threading.Lock()  # Ensures thread safety for shared variables


def forward_data(src_socket, dest_socket, record=False):
    """Transfers data from src_socket to dest_socket, optionally recording it."""
    try:
        data = src_socket.recv(4096)
        if not data:
            return False  # Connection closed

        dest_socket.sendall(data)

        if record:
            with lock:  # Prevent race conditions
                recorded_data.append(data)

    except (socket.error, ConnectionResetError):
        return False  # Connection error

    return True


def handle_client(client_socket, server_socket):
    """Handles bidirectional communication between client and server."""
    global recorded_data

    sockets = [client_socket, server_socket]

    try:
        while True:
            readable, _, _ = select.select(sockets, [], [])
            
            for sock in readable:
                if sock is client_socket:
                    if not forward_data(client_socket, server_socket, record=False):
                        return  # Client disconnected
                elif sock is server_socket:
                    if not forward_data(server_socket, client_socket, record=recording):
                        return  # Server disconnected

    except (socket.error, ConnectionResetError):
        pass  # Handle client/server disconnection

    client_socket.close()
    server_socket.close()


def control_handler():
    """Runs in a separate thread to listen for control commands on port 6616."""
    global recording, recorded_data

    control_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    control_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    control_socket.bind(("0.0.0.0", CONTROL_PORT))
    control_socket.listen(1)
    print(f"Control interface listening on port {CONTROL_PORT}")

    while True:
        conn, _ = control_socket.accept()

        try:
            command = conn.recv(1024).decode().strip().lower()
            if not command:
                continue  # Ignore empty input

            if command == "start-recording":
                with lock:
                    recording = True
                    recorded_data = []
                conn.sendall(b"Recording started.\n")

            elif command == "stop-recording":
                with lock:
                    recording = False
                try:
                    with open(RECORD_FILE, "wb") as f:
                        with lock:
                            for data in recorded_data:
                                f.write(data)
                    conn.sendall(b"Recording stopped and saved.\n")
                except Exception as e:
                    conn.sendall(f"Error saving file: {e}\n".encode())

            elif command == "playback":
                with lock:
                    for data in recorded_data:
                        conn.sendall(data)
                conn.sendall(b"Playback complete.\n")

            else:
                conn.sendall(b"Unknown command.\n")

        except (socket.error, ConnectionResetError):
            pass  # Handle disconnection

        conn.close()


def start_proxy():
    """Starts the TCP proxy between client and server."""
    proxy_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    proxy_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    proxy_socket.bind(("0.0.0.0", PROXY_PORT))
    proxy_socket.listen(5)
    print(f"TCP Proxy listening on port {PROXY_PORT}")

    # Start control listener in a separate thread
    threading.Thread(target=control_handler, daemon=True).start()

    while True:
        client_socket, _ = proxy_socket.accept()
        
        # Connect to the actual server
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.connect((SERVER_HOST, SERVER_PORT))

        # Start bidirectional traffic handling in a separate thread
        threading.Thread(target=handle_client, args=(client_socket, server_socket), daemon=True).start()


if __name__ == "__main__":
    start_proxy()
