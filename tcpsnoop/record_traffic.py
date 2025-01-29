#!/usr/bin/env python3
"""
TCP Traffic Recorder

This script listens for incoming TCP connections on a specified port, captures 
the raw message payloads (excluding metadata like timestamps and headers), and 
writes them to a file for later replay.

Usage:
    python record_traffic.py <port> <output_file>

Example:
    python record_traffic.py 5000 captured_data.bin

Arguments:
    port        The TCP port to listen on.
    output_file The file where the recorded traffic will be saved.

How It Works:
    - Listens for TCP connections on the specified port.
    - Accepts a single client connection.
    - Reads incoming data and writes only the payload to the output file.
    - Stops recording when the sender closes the connection.
"""

import socket
import argparse

def record_traffic(listen_port, output_file):
    """Capture messages sent to listen_port and save the payloads."""
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(("127.0.0.1", listen_port))
    server_socket.listen(1)
    
    print(f"Listening for messages on port {listen_port}...")

    conn, addr = server_socket.accept()
    print(f"Connection received from {addr}")

    with open(output_file, "wb") as f:
        while True:
            data = conn.recv(4096)
            if not data:
                break  # Stop if the connection is closed
            f.write(data)  # Save only the raw payload

    print("Recording complete. Data saved.")
    conn.close()
    server_socket.close()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="TCP Traffic Recorder")
    parser.add_argument("port", type=int, help="Port number to listen on")
    parser.add_argument("output", type=str, help="File to save captured traffic")
    
    args = parser.parse_args()
    record_traffic(args.port, args.output)
