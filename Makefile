all: dph prod cons

dph: dph.c
	gcc dph.c -lrt -lpthread -o dph

prod: prod.c
	gcc prod.c -lm -lrt -lpthread -o prod

cons: prod.c
	gcc cons.c -lm -lrt -lpthread -o cons

