@echo "start test server ..."
kvdb-server.exe --host=127.0.0.1 --port=8090 --mode=echo --test=qps --packet-size=64 --thread-num=1 --echo=0
pause
