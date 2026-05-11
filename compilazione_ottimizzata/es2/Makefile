# =============================================================
# Makefile — Lab 2: Memory Aliasing e restrict
# =============================================================

CC  = gcc
SRC = alias.c

.PHONY: all run asm vec nostrict clean

all: alias alias_nostrict

alias:          $(SRC) ; $(CC) -O2 -o $@ $<
alias_nostrict: $(SRC) ; $(CC) -O2 -fno-strict-aliasing -o $@ $<
alias_O3:       $(SRC) ; $(CC) -O3 -o $@ $<

run: alias alias_nostrict alias_O3
	@echo "=== -O2 (con strict aliasing) ==="; ./alias
	@echo ""
	@echo "=== -O2 -fno-strict-aliasing ===";  ./alias_nostrict
	@echo ""
	@echo "=== -O3 ===";                        ./alias_O3

# genera assembly e mostra differenza vettorizzazione
asm: $(SRC)
	$(CC) -O2 -S -fverbose-asm -o alias.s $<
	@echo ""
	@echo "=== Istruzioni scalari in scale() ==="
	@awk '/^scale:/{p=1} /^scale_r:/{p=0} p' alias.s | \
	    grep -E "mov|mul|add" | head -15
	@echo ""
	@echo "=== Istruzioni SIMD in scale_r() ==="
	@awk '/^scale_r:/{p=1} /^scale_tmp:/{p=0} p' alias.s | \
	    grep -E "ymm|xmm|vmov|vmul" | head -15

# mostra quali loop vengono vettorizzati
vec: $(SRC)
	$(CC) -O2 -fopt-info-vec -o alias_vec $< 2>&1

clean:
	rm -f alias alias_nostrict alias_O3 alias_vec alias.s
