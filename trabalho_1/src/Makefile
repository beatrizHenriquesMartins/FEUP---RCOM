projeto: dataLink.o
	gcc sender.c dataLink.c AppLink.c -Wall -w -o sender
	gcc receiver.c dataLink.c AppLink.c -Wall -w -o receiver

debug: dataLink.o
	gcc sender.c dataLink.c AppLink.c -Wall -w -o sender -g
	gcc receiver.c dataLink.c AppLink.c -Wall -w -o receiver -g

clean :
	rm -f sender receiver
