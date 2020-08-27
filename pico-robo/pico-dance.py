import requests
import time
import sys

def go():
    response = requests.get('http://192.168.10.14/go/')
    print(response.text)    # レスポンスのHTMLを文字列で取得

def back():
    response = requests.get('http://192.168.10.14/back/')
    print(response.text)    # レスポンスのHTMLを文字列で取得

def right():
    response = requests.get('http://192.168.10.14/right/')
    print(response.text)    # レスポンスのHTMLを文字列で取得

def left():
    response = requests.get('http://192.168.10.14/left/')
    print(response.text)    # レスポンスのHTMLを文字列で取得

def turn1():
    response = requests.get('http://192.168.10.14/f1/')
    print(response.text)    # レスポンスのHTMLを文字列で取得

def turn2():
    response = requests.get('http://192.168.10.14/f2/')
    print(response.text)    # レスポンスのHTMLを文字列で取得

def stop():
    response = requests.get('http://192.168.10.14/stop/')
    print(response.text)    # レスポンスのHTMLを文字列で取得

def f3():
    response = requests.get('http://192.168.10.14/f3/')
    print(response.text)    # レスポンスのHTMLを文字列で取得

while True:
    try:
        f3()
        time.sleep(2)
        go()
        time.sleep(3)
        stop()
        time.sleep(1)
        back()
        time.sleep(3)
        turn1()
        time.sleep(2)
        turn2()
        time.sleep(2)
        stop()
        time.sleep(1)
        right()
        time.sleep(4)
        stop()
        time.sleep(1)
        left()
        time.sleep(4)
        stop()
        print("END")

    except KeyboardInterrupt:
        stop()
        print("end")
        sys.exit(0)
        break