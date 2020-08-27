import requests
import time
import sys

def stop():
    response = requests.get('http://192.168.10.12/S/')
    print("stop")    # レスポンスのHTMLを文字列で取得

def go():
    response = requests.get('http://192.168.10.12/G/')
    print("go")    # レスポンスのHTMLを文字列で取得

def back():
    response = requests.get('http://192.168.10.12/B/')
    print("back")    # レスポンスのHTMLを文字列で取得

def turnR():
    response = requests.get('http://192.168.10.12/TR/')
    print("turn right")

def turnL():
    response = requests.get('http://192.168.10.12/TL/')
    print("turn left")

def end():
    response = requests.get('http://192.168.10.12/E/')
    print("END")

while True:
    try:
        stop()
        time.sleep(1)
        go()
        time.sleep(1)
        stop()
        time.sleep(2)
        back()
        time.sleep(1)
        stop()
        time.sleep(2)
        turnR()
        time.sleep(2)
        turnL()
        time.sleep(2)
        stop()

    except KeyboardInterrupt:
        stop()
        print("end")
        sys.exit(0)
        break