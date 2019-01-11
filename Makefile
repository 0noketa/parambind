CC = cc -O0 -DFOR_I386_CDECL
MODS = src/parambinder1.o src/parambinder2.o
LD = ar q

parambinder.a: $(MODS)
	$(LD) parambinder.a $(MODS)

clean:
	rm -f src/*.o
	rm -f *.a
