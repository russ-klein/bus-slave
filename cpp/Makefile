GCC = gcc

all: if_gen if_gen_ahb if_gen_axi

if_gen: if_gen.c
	$(GCC) -o if_gen if_gen.c

if_gen_ahb: if_gen_ahb.c
	$(GCC) -o if_gen_ahb if_gen_ahb.c

if_gen_axi: if_gen_axi.c
	$(GCC) -o if_gen_axi if_gen_axi.c

clean:
	rm -rf if_gen if_gen_ahb if_gen_axi
