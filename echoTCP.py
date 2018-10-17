import socket

TCP_IP = 'xxx.xxx.xxx.xxx'
TCP_PORT = 5005
BUFFER_SIZE = 256  # Normally 1024, but we want fast response


s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((TCP_IP, TCP_PORT))
s.listen(1)

while True:
    counter = 0
    conn, addr = s.accept()
    print 'Connection address:', addr
    while True:
        data = conn.recv(BUFFER_SIZE)
        if not data: break
        print "received data:", data
        echo_message = "PONG "+ str(counter) + "\0"
        conn.send(echo_message )  # echo message
        print "echoed message: ", echo_message
        counter = counter + 1
    conn.close()