@echo "start kvdb server ..."
kvdb-server.exe --host=127.0.0.1 --port=8077 --mode=echo --test=qps --packet-size=64 --thread-num=0
pause
