HEADERS=main.c net.c logging.h symmetry.h config.h

all: symmetry

symmetry: main.o net.o logging.o symmetry.o
	gcc -g main.o net.o logging.o symmetry.o -o symmetry

main.o: main.c ${HEADERS}
	gcc -g -c main.c

net.o: net.c ${HEADERS}
	gcc -g -c net.c

logging.o: logging.c ${HEADERS}
	gcc -g -c logging.c

symmetry.o: symmetry.c ${HEADERS}
	gcc -g -c symmetry.c

clean:
	rm -f *o symmetry
