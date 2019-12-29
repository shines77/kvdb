@echo "start test clients ..."
for /l %%i in (1,1,10) do start /min "kvdb-cli" kvdb-cli.exe --host=192.168.88.102 --host=127.0.0.1:8077 --username=root --password=user1234 --database=topics
