@echo "start test clients ..."
kvdb-cli.exe --host=127.0.0.1 --port=8090 --mode=http --test=qps --packet-size=64 --thread-num=1
pause