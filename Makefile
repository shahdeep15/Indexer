CCFLAGS = -Wall -g -o

alldir: lib exec rundir

allfile: lib exec runfile

allsameio: lib exec runsameio

alllong: lib exec runlong

lib: sorted-list.c sorted-listmain.c tokenizer.c
	gcc -c sorted-list.c
	gcc -c sorted-listmain.c
	gcc -c tokenizer.c
	ar rv libi.a sorted-list.o sorted-listmain.o tokenizer.o

exec:
	gcc $(CCFLAGS) index index.c ./libi.a

rundir: 
	./index output.txt testdir

runfile: 
	./index output.txt test.txt

runsameio: 
	./index sameio.txt sameio.txt

runlong: 
	./index output.txt long.txt

clean:
	rm -rf *.o
	rm -rf *.a

