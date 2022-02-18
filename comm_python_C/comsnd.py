from socket import socket, AF_INET, SOCK_STREAM
import time

HOST        = 'localhost'
PORT        = 51000

def com_send():

    while True:
        try:
            # 通信の確立
            sock = socket(AF_INET, SOCK_STREAM)
            sock.connect((HOST, PORT))

            # メッセージ送信 （以下では 0〜9までを１秒おきに送信）
            for i in range(10):
                # data を文字列に変換する
                mess = str(i);
                print ('send message: ' + mess)
                # encode で文字列型をデータ型に変換
                sock.send(mess.encode())

                time.sleep(1)

            # 通信の終了
            sock.close()
            break

        except:
            print ('retry: ')


