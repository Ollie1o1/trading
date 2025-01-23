lab2_output: lab2A.o lab2Main.o
	gcc -Wall -std=c99 lab2A.o lab2Main.o -o lab2_output

lab2A.o: lab2A.c lab2A.h
	gcc -Wall -std=c99 -c lab2A.c

lab2Main.o: lab2Main.c lab2A.h
	gcc -Wall -std=c99 -c lab2Main.c

clean:
	rm -f *.o lab2_output