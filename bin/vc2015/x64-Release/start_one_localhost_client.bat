@echo "start test clients ..."
kvdb-cli.exe --host=192.l68.2.231 --port=8090 --mode=echo --test=qps --packet-size=64 --thread-num=1
pause
