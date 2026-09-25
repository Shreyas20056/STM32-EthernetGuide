import socket
import time

# ==================== SETTINGS ====================
STM32_IP = "192.168.1.10"     # STM32 Static IP (must match CubeMX)
STM32_PORT = 7                # Port used in tcp_echoserver.c
# ==================================================

def test_tcp_communication():
    try:
        # Create TCP socket
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client.settimeout(5)  # 5 seconds timeout

        print(f"Connecting to STM32 ({STM32_IP}:{STM32_PORT})...")
        client.connect((STM32_IP, STM32_PORT))
        print("Connected successfully!\n")

        # List of test messages
        messages = [
            "Hello STM32",
            "This is a TCP test from Python",
            "1234567890",
            "Echo test successful?"
        ]

        for msg in messages:
            print(f"Sending  → {msg}")
            client.send(msg.encode('utf-8'))

            # Receive echo from STM32
            response = client.recv(1024)
            print(f"Received ← {response.decode('utf-8')}\n")

            time.sleep(0.5)

        print("All messages sent and received successfully!")
        client.close()

    except ConnectionRefusedError:
        print("Error: Connection refused.")
        print("→ Is the STM32 powered on and running the TCP server?")
    except socket.timeout:
        print("Error: Connection timed out.")
        print("→ Check IP address, Ethernet cable, and network settings.")
    except Exception as e:
        print(f"Error occurred: {e}")

if __name__ == "__main__":
    test_tcp_communication()