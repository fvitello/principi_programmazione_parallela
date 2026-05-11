# =============================================================
# Makefile — Lab 3: Godbolt e Assembly
# =============================================================

CC  = gcc
SRC = dot.c

.PHONY: all run asm godbolt clean

all: dot_O2 dot_O3 dot_Ofast

dot_O2:   $(SRC) ; $(CC) -O2        -o $@ $< -lm
dot_O3:   $(SRC) ; $(CC) -O3        -o $@ $< -lm
dot_Ofast:$(SRC) ; $(CC) -Ofast     -o $@ $< -lm

run: all
	@echo "=== -O2   ==="; ./dot_O2
	@echo "=== -O3   ==="; ./dot_O3
	@echo "=== -Ofast==="; ./dot_Ofast

# assembly per analisi locale (alternativa a Godbolt)
asm: $(SRC)
	$(CC) -O2 -S -fverbose-asm -masm=intel -o dot_O2.s $<
	$(CC) -O3 -S -fverbose-asm -masm=intel -o dot_O3.s $<
	@echo "=== FMA in -O3 (vfmadd) ==="
	@grep "fmadd" dot_O3.s || echo "(nessuna istruzione FMA)"
	@echo "=== SIMD in -O2 ==="
	@grep -c ymm dot_O2.s && echo "ymm trovate in -O2" || echo "nessuna ymm in -O2"
	@echo "=== SIMD in -O3 ==="
	@grep -c ymm dot_O3.s && echo "ymm trovate in -O3" || echo "nessuna ymm in -O3"

# suggerimento per incollare su Godbolt
godbolt:
	@echo ""
	@echo "Apri: https://godbolt.org"
	@echo "Incolla la funzione dot_r() da dot.c"
	@echo "Prova questi flag:"
	@echo "  -O2"
	@echo "  -O3"
	@echo "  -O2 (senza restrict nel sorgente)"
	@echo "  -Ofast"
	@echo ""

clean:
	rm -f dot_O2 dot_O3 dot_Ofast dot_O2.s dot_O3.s
