all: mync ttt

ttt:ttt.o
	gcc ttt.o -o ttt

ttt.o:ttt.c
	gcc -c ttt.c -o ttt.o

mync: mync.o
	gcc --coverage mync.o -o mync

mync.o: mync.c
	gcc -c mync.c -o mync.o -fprofile-arcs -ftest-coverage


clean:
	rm -f *.o ttt mync *.gcno *.gcda *.gcov
