# Makefile

CC		= gcc
RM		= rm


.SUFFIXES: .c .o
.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<

OBJS =	gen.o

gen: $(OBJS)
	$(CC) $(OBJS) ../libudis86/libudis86.a -o gen

tests: test16 test32 test64 testjmp ovrrun randraw

test16: gen
	yasm -f bin -o test16.bin test16.asm
	echo "[bits 16]" > test16.out
	./gen -16 < test16.bin >> test16.out
	diff test16.asm test16.out | more
test32: gen
	yasm -f bin -o test32.bin test32.asm
	echo "[bits 32]" > test32.out
	./gen -32 < test32.bin >> test32.out
	diff test32.asm test32.out | more
test64: gen
	yasm -f bin -o test64.bin test64.asm
	echo "[bits 64]" > test64.out
	./gen -64 < test64.bin >> test64.out
	diff test64.asm test64.out | more
testjmp: gen
	yasm -f bin -o testjmp.bin testjmp.asm
	udcli -64 < testjmp.bin > testjmp.out
	diff testjmp.ref testjmp.out

ovrrun: bufovrrun.c
	$(CC) bufovrrun.c -o ovrrun ../libudis86/libudis86.a
	./ovrrun

randraw: randtest.raw
	./gen -16 < randtest.raw > randtest16.out
	diff randtest16.out randtest16.ref
	./gen -32 < randtest.raw > randtest32.out
	diff randtest32.out randtest32.ref
	./gen -64 < randtest.raw > randtest64.out
	diff randtest64.out randtest64.ref

clean:
	$(RM) -f core ./*.o ./gen *~ *.bin *.out ovrrun
