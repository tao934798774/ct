WORK_DIR=$(shell pwd)
INCLUDE=-I$(WORK_DIR)/../mysql-lib/mysql-test/include

insert_cgi:insert_cgi.cpp comm.cpp
	g++ -o $@ $^ $(INCLUDE)
.PHONY:clean
clean:
	rm -f insert_cgi
