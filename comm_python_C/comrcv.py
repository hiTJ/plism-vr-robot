from socket import socket, AF_INET, SOCK_STREAM

HOST        = 'localhost'
PORT        = 51000
MAX_MESSAGE = 1024

def com_receive():

    #mess[MAX_MESSAGE]

    # 通信の確立
    sock = socket(AF_INET, SOCK_STREAM)
    sock.bind    ((HOST, PORT))
    sock.listen  (5)
    print ('receiver ready ')

    conn, addr = sock.accept()

    # メッセージ受信ループ
    while True:
        try:
            # decode()  バイト型 -> 文字列型に変換
            mess       = conn.recv(MAX_MESSAGE).decode()
            print ('receive message: ' + mess)

            # mess サイズがゼロになったら終了
            #print (len(mess))
            if len(mess) == 0:
                break

        except:
            print ('Error:' + mess)

    conn.close()
    # 通信の終了
    sock.close()
    print ('end of receiver')

