import time
import serial
port='COM18'
for baud in (115200,9600,57600):
    try:
        ser=serial.Serial(port,baud,timeout=0.2)
    except Exception as e:
        print(f'open {port}@{baud} fail: {e}')
        continue
    start=time.time(); total=0
    while time.time()-start<4:
        d=ser.read(512)
        total += len(d)
    ser.close()
    print(f'{port}@{baud}: {total} bytes in 4s')
