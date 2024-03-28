
wish: wish.o batch.o interactive.o exec.o
	gcc -g -o wish wish.o batch.o interactive.o exec.o

main.o: wish.c
	gcc -c -g wish.c

batch.o: batch.c
	gcc -c -g batch.c

interactive.o: interactive.c
	gcc -c -g interactive.c

exec.o: exec.c
	gcc -c -g exec.c

clean: 
	rm -f *.o wish out 
	clear
