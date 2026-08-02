import socket as s

svr = s.socket(s.AF_INET, s.SOCK_STREAM)

svr.bind(("127.0.0.1", 10255))
svr.listen()

while True:
    cli, addr = svr.accept()
    connected = True
    while connected:
        try:
            _ = cli.send(b"0 0.000 >")
            print(cli.recv(128).decode().rstrip())
        except:
            connected = False