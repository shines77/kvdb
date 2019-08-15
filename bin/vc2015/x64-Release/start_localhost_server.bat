@echo "start test server ..."
kvdb-cli.exe --host=0.0.0.0 --port=8090 --mode=echo --test=qps --packet-size=64 --thread-num=1
pause
