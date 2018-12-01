build:
	gcc -Wall -c request/request.c
	gcc -Wall -c aimbrain/aimbrain_context.c
	gcc -Wall -c aimbrain/sessions.c
	gcc -Wall -c main.c
	gcc main.o request.o aimbrain_context.o sessions.o -o libaimbrain -lcjson -lcrypto `curl-config --libs`
clean:
	rm libaimbrain *.o
