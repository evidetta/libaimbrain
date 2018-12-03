build:
	gcc -Wall -g -Og -c request/request.c
	gcc -Wall -g -Og -c error/error.c
	gcc -Wall -g -Og -c aimbrain/aimbrain_context.c
	gcc -Wall -g -Og -c aimbrain/sessions.c
	gcc -Wall -g -Og -c main.c
	gcc main.o request.o error.o aimbrain_context.o sessions.o -o libaimbrain -lcjson -lcrypto `curl-config --libs`
clean:
	rm libaimbrain *.o
