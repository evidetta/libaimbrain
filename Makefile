build:
	gcc -Wall aimbrain.c request.c -o libaimbrain -lcjson -lcrypto `curl-config --libs`
