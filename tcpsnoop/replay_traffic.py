#!/usr/bin/env python3
"""
TCP Traffic Replayer

This script reads a previously recorded TCP message payload and sends it as 
a new message to a target host and port, making it appear as if the original 
sender has sent a fresh message.

Usage:
    python replay_traffic.py <target_host> <target_port> <input_file>

Example:
    python replay_traffic.py 127.0.0.1 6000 captured_data.bin

Arguments:
    target_host The IP address of the target application.
    target_port The TCP port to send the replayed message.
    input_file  The file containing the recorded message payload.

How It Works:
    - Establishes a fresh TCP connection to the target application.
    - Reads the recorded payload from the input file.
    - Sends the data over the connection as if it were a new message.
    - Closes the connection after all data is sent.
"""

import socket
import argparse

def replay_traffic(target_host, target_port, input_file):
    """Replay saved TCP messages as new messages to target_host:target_port."""
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((target_host, target_port))
    
    print(f"Connected to {target_host}:{target_port}")

    with open(input_file, "rb") as f:
        for chunk in iter(lambda: f.read(4096), b""):  # Read in 4KB chunks
            client_socket.sendall(chunk)  # Send as fresh message

    print("Replayed all captured messages.")
    client_socket.close()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="TCP Traffic Replayer")
    parser.add_argument("host", type=str, help="Target host (e.g., 127.0.0.1)")
    parser.add_argument("port", type=int, help="Target port number")
    parser.add_argument("input", type=str, help="File containing captured traffic")

    args = parser.parse_args()
    replay_traffic(args.host, args.port, args.input)
