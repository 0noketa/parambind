CC = cc -O0 -DFOR_I386_CDECL
MODS = src/parambind1.o src/parambind2.o
LD = ar q

parambind.a: $(MODS)
	$(LD) parambind.a $(MODS)

clean:
	rm -f src/*.o
	rm -f *.a
