@echo "start test clients ..."
rem kvdb-cli.exe mode=qps 127.0.0.1 8090 64
kvdb-cli.exe --host=127.0.0.1 --port=8090 --mode=echo --test=throughput --packet-size=64 --thread-num=1
pause
