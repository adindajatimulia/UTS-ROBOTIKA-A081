
import serial
import serial.tools.list_ports
import time

BAUD_RATE = 115200


def pilih_port():
    ports = list(serial.tools.list_ports.comports())

    print("\n=== PORT ESP32 YANG TERDETEKSI ===")

    if not ports:
        print("Tidak ada port serial terdeteksi.")
        print("Periksa kabel USB dan driver ESP32.")
        return None

    for p in ports:
        print(f"{p.device} - {p.description}")

    port = input("\nMasukkan port ESP32 (contoh COM5): ")
    return port.strip().upper()


def main():
    port = pilih_port()

    if port is None:
        return

    try:
        esp32 = serial.Serial(
            port=port,
            baudrate=BAUD_RATE,
            timeout=1,
            write_timeout=2
        )

        # ESP32 mungkin restart ketika port dibuka
        time.sleep(2)
        esp32.reset_input_buffer()

        print("\nESP32 berhasil terhubung!")

    except serial.SerialException as e:
        print("\nGagal terhubung:", e)
        print("Tutup Serial Monitor Arduino IDE.")
        return

    try:
        while True:
            print("\n===== KONTROL MOTOR iMCLab =====")
            print("1. Motor ON")
            print("2. Motor OFF")
            print("3. Kecepatan naik")
            print("4. Kecepatan turun")
            print("5. Atur PWM manual")
            print("0. Keluar")

            pilihan = input("Pilih menu: ").strip()

            if pilihan == "1":
                perintah = "1"

            elif pilihan == "2":
                perintah = "0"

            elif pilihan == "3":
                perintah = "+"

            elif pilihan == "4":
                perintah = "-"

            elif pilihan == "5":
                nilai = input("Masukkan PWM 0-255: ").strip()

                if not nilai.isdigit():
                    print("Masukkan angka saja.")
                    continue

                pwm = int(nilai)

                if not 0 <= pwm <= 255:
                    print("PWM harus antara 0 dan 255.")
                    continue

                perintah = f"PWM:{pwm}"

            elif pilihan == "0":
                esp32.write(b"0\n")
                esp32.flush()
                print("Perintah OFF dikirim.")
                break

            else:
                print("Pilihan tidak tersedia.")
                continue

            # Kirim perintah Python ke ESP32
            esp32.write((perintah + "\n").encode("utf-8"))
            esp32.flush()

            print("Perintah terkirim:", perintah)

            # Tampilkan balasan Arduino jika tersedia
            time.sleep(0.1)

            while esp32.in_waiting:
                balasan = esp32.readline().decode(
                    "utf-8", errors="ignore"
                ).strip()

                if balasan:
                    print("Balasan ESP32:", balasan)

    except KeyboardInterrupt:
        print("\nProgram dihentikan.")

    except serial.SerialException as e:
        print("Koneksi serial terputus:", e)

    finally:
        if esp32.is_open:
            esp32.close()
        print("Koneksi serial ditutup.")


if __name__ == "__main__":
    main()
