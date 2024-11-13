import serial

# Set the USB port name (update this to match your device's actual port)
USB_PORT = "/dev/tty.usbmodem1301"  # For Linux/macOS; on Windows, it might be "COM3" or similar


def receive_image():
    print(f"Connecting to {USB_PORT}...")
    ser = serial.Serial(USB_PORT, timeout=10)  # Set the appropriate baud rate
    print("Connection established.")

    with open("received_photo.jpeg", "wb") as f:
        print("Receiving data...")
        while True:
            data = ser.read(1024)  # Adjust buffer size as needed
            if not data:
                print("No more data. Transfer complete.")
                break
            print("Received data:", data)
            f.write(data)
            print(f"Received {len(data)} bytes")
        print("Image saved to received_photo.jpeg")
    ser.close()
    print("Serial connection closed.")


if __name__ == "__main__":
    receive_image()
