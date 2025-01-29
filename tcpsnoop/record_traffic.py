#!/usr/bin/env python3
"""
TCP Traffic Sniffer (Passive)

This script captures TCP packets sent between a client and a server on a given 
port without interfering with their communication. It extracts the payloads 
and saves them to a file.

Usage:
    sudo python tcp_record.py <port> <output_file>

Example:
    sudo python tcp_record.py 5556 recorded_data.bin

Requires:
    - Python's `scapy` module (`pip install scapy`).
    - Root privileges to sniff packets.
"""

from scapy.all import sniff, TCP, Raw
import sys

def packet_callback(packet, output_file):
    """Callback function to process each captured packet."""
    if packet.haslayer(Raw):  # Check if the packet has a payload
        data = packet[Raw].load
        with open(output_file, "ab") as f:
            f.write(data)  # Append payload to file

def record_traffic(port, output_file):
    """Capture TCP traffic on the specified port."""
    print(f"Sniffing TCP traffic on port {port}... (Press Ctrl+C to stop)")

    try:
        sniff(
            filter=f"tcp port {port}",
            prn=lambda pkt: packet_callback(pkt, output_file),
            store=False
        )
    except KeyboardInterrupt:
        print("\nRecording stopped.")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: sudo python tcp_record.py <port> <output_file>")
        sys.exit(1)

    port = sys.argv[1]
    output_file = sys.argv[2]

    record_traffic(port, output_file)
