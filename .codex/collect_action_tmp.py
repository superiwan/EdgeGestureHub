import argparse
import csv
import datetime as dt
import os
import signal
import sys
import time

import serial
from serial.tools import list_ports

AUTO_KEYWORDS=("stlink","daplink","cmsis-dap","mbed serial","usb serial","ch340","ch34","cp210","silicon labs","ftdi","pl2303")


def pick_auto_port():
    ports=list(list_ports.comports())
    if not ports:
        return None
    scored=[]
    for p in ports:
        text=f"{p.device} {p.description} {p.hwid}".lower()
        score=sum(1 for k in AUTO_KEYWORDS if k in text)
        scored.append((score,p.device))
    scored.sort(key=lambda x:x[0], reverse=True)
    if scored[0][0] <= 0:
        return None
    return scored[0][1]


def is_csv7(line):
    parts=line.strip().split(',')
    if len(parts) != 7:
        return False
    try:
        for i,p in enumerate(parts):
            if i < 6:
                float(p)
            else:
                int(float(p))
        return True
    except Exception:
        return False


def main():
    ap=argparse.ArgumentParser()
    ap.add_argument('--port', required=True)
    ap.add_argument('--baud', type=int, required=True)
    ap.add_argument('--seconds', type=int, required=True)
    ap.add_argument('--csv', required=True)
    args=ap.parse_args()

    port=args.port
    if port.lower()=='auto':
        port=pick_auto_port()
        if not port:
            print('[collect] auto port detection failed')
            return 2

    os.makedirs(os.path.dirname(args.csv), exist_ok=True)
    deadline=time.time()+args.seconds
    rows=0

    with serial.Serial(port, args.baud, timeout=0.2) as ser, open(args.csv,'w',newline='',encoding='utf-8') as f:
        w=csv.writer(f)
        w.writerow(['ax','ay','az','gx','gy','gz','cls'])
        print(f"[collect] connected {port}@{args.baud}")
        buf=''
        while time.time() < deadline:
            data=ser.read(512)
            if not data:
                continue
            buf += data.decode('utf-8', errors='ignore')
            while '\n' in buf:
                line, buf = buf.split('\n',1)
                line=line.strip('\r').strip()
                if is_csv7(line):
                    w.writerow(line.split(','))
                    rows += 1
        print(f"[collect] done, rows={rows}")

    return 0


if __name__=='__main__':
    raise SystemExit(main())
