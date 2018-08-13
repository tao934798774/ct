.PHONY:all
all:server client server_fork

server:server.c
	gcc $^ -o $@

client:client.c
	gcc $^ -o $@

server_fork:server_fork.c
	gcc $^ -o $@

.PHONY:clean
clean:
	rm server client server_fork
