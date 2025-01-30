#!/usr/bin/env python3
"""
TCP Proxy Control Client

This program sends control commands to the TCP proxy server at 127.0.0.1:6616.

Available Commands:
    --start-recording   Begin recording server-to-client messages.
    --stop-recording    Stop recording and save data to a file.
    --playback         Send the recorded messages back to the client.

Usage:
    python tcp_control.py --start-recording
    python tcp_control.py --stop-recording
    python tcp_control.py --playback
"""

import socket
import sys

HOST = "127.0.0.1"
PORT = 6616

def send_command(command):
    """Connects to the TCP proxy control interface and sends a command."""
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as control_socket:
            control_socket.connect((HOST, PORT))
            control_socket.sendall(command.encode() + b"\n")

            response = control_socket.recv(1024).decode().strip()
            print(f"Response: {response}")

    except (socket.error, ConnectionRefusedError):
        print("Error: Unable to connect to the control interface.")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python tcp_control.py --start-recording | --stop-recording | --playback")
        sys.exit(1)

    command_map = {
        "--start-recording": "start-recording",
        "--stop-recording": "stop-recording",
        "--playback": "playback"
    }

    user_command = sys.argv[1]

    if user_command in command_map:
        send_command(command_map[user_command])
    else:
        print("Invalid command. Use --start-recording, --stop-recording, or --playback.")
        sys.exit(1)
