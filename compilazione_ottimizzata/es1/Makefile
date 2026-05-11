# =============================================================
# Makefile — Lab 1: Confronto livelli -O
#
# Uso:
#   make          → compila tutte le varianti
#   make run      → esegue tutte le varianti e confronta i tempi
#   make asm      → genera i file .s annotati per Lab 1B
#   make simd     → cerca istruzioni SIMD nei file assembly
#   make clean    → rimuove tutti i file generati
# =============================================================

CC      = gcc
# Su Mac con clang, cambia in: CC = clang
# (i flag -O, -S, -fverbose-asm funzionano ugualmente)

SRC     = matrix.c
BINS    = matrix_O0 matrix_O1 matrix_O2 matrix_O3 matrix_Ofast
ASM     = matrix_O0.s matrix_O2.s matrix_O3.s

.PHONY: all run asm simd clean

all: $(BINS)

matrix_O0:    $(SRC) ; $(CC) -O0          -o $@ $<
matrix_O1:    $(SRC) ; $(CC) -O1          -o $@ $<
matrix_O2:    $(SRC) ; $(CC) -O2          -o $@ $<
matrix_O3:    $(SRC) ; $(CC) -O3          -o $@ $<
matrix_Ofast: $(SRC) ; $(CC) -Ofast       -o $@ $<

# ── esegue le varianti nell'ordine e stampa i tempi ──────────
run: all
	@echo "=== -O0 ===";  time ./matrix_O0
	@echo "=== -O1 ===";  time ./matrix_O1
	@echo "=== -O2 ===";  time ./matrix_O2
	@echo "=== -O3 ===";  time ./matrix_O3
	@echo "=== -Ofast ==="; time ./matrix_Ofast

# ── genera assembly annotato (Lab 1B) ────────────────────────
asm: $(ASM)

matrix_O0.s: $(SRC)
	$(CC) -O0 -S -fverbose-asm -o $@ $<

matrix_O2.s: $(SRC)
	$(CC) -O2 -S -fverbose-asm -o $@ $<
	@echo ">>> istruzioni totali -O2:"; wc -l $@

matrix_O3.s: $(SRC)
	$(CC) -O3 -S -fverbose-asm -o $@ $<
	@echo ">>> istruzioni totali -O3:"; wc -l $@

# ── cerca istruzioni SIMD nei file assembly ──────────────────
simd: matrix_O2.s matrix_O3.s
	@echo "=== istruzioni SIMD in -O2 ==="
	@grep -E "vmovsd|vmovupd|vfmadd|vmulpd|vaddpd|ymm|xmm" matrix_O2.s || echo "(nessuna trovata)"
	@echo ""
	@echo "=== istruzioni SIMD in -O3 ==="
	@grep -E "vmovsd|vmovupd|vfmadd|vmulpd|vaddpd|ymm|xmm" matrix_O3.s || echo "(nessuna trovata)"
	@echo ""
	@echo "=== confronto densità SIMD (ymm) ==="
	@grep -c ymm matrix_O2.s 2>/dev/null && grep -c ymm matrix_O3.s 2>/dev/null || true

# ── vettorizzazione: informa il compilatore di cosa ottimizza ─
vec: $(SRC)
	$(CC) -O2 -fopt-info-vec -o matrix_vec $< 2>&1 | grep -i "vectorized\|loop"

clean:
	rm -f $(BINS) $(ASM) matrix_vec
