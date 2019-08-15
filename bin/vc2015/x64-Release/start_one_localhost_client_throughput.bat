@echo "start test clients ..."
kvdb-cli.exe --host=192.168.2.231 --port=8090 --mode=echo --test=throughput --packet-size=64 --thread-num=1
pause
