detect: detect.o
	gcc -Wall -o detect detect.c

detect.o: detect.c detect.h
	gcc -c detect.c -Wall

clean:
	rm -f *.o detect