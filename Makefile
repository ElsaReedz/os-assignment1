# To run, enter
# make all

all: dph prod cons mycall

dph: dph.c
	gcc dph.c -lrt -lpthread -o dph

prod: prod.c
	gcc prod.c -lm -lrt -lpthread -o prod

cons: prod.c
	gcc cons.c -lm -lrt -lpthread -o cons

mycall: mycall.c
	gcc mycall.c -o mycall