@echo "start test clients ..."
# kvdb-cli.exe --host=0.0.0.0 --port=8090 --mode=echo --test=qps --packet-size=64 --thread-num=1
kvdb-cli.exe --host=0.0.0.0 --port=8090 --mode=echo --test=pingpong --packet-size=64 --thread-num=1
pause
