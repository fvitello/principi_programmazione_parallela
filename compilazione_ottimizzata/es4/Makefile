# =============================================================
# Makefile — Lab 4: perf stat e perf record
#
# NOTA: perf funziona solo su Linux.
# Su Mac vedi le istruzioni nel sorgente (Instruments/sample).
# =============================================================

CC     = gcc
SRC    = profiling_target.c
BIN    = profiling_target
FG_DIR = /tmp/flamegraph

.PHONY: all run stat stat-detail record report flamegraph setup-perf clean

all: $(BIN)

$(BIN): $(SRC)
	$(CC) -O2 -g -o $@ $< -lm
	@echo "Compilato con -O2 -g (simboli di debug inclusi)"

# ── esecuzione semplice ───────────────────────────────────────
run: $(BIN)
	./$(BIN)

# ── Lab 4A: perf stat ─────────────────────────────────────────
stat: $(BIN)
	perf stat ./$(BIN)

stat-detail: $(BIN)
	perf stat -r 3 \
	  -e cycles,instructions,cache-misses,\
	  L1-dcache-loads,L1-dcache-load-misses,LLC-load-misses \
	  ./$(BIN)

# ── Lab 4B: perf record + report ──────────────────────────────
record: $(BIN)
	perf record -g --call-graph dwarf -o perf.data ./$(BIN)
	@echo "Profilo salvato in perf.data"
	@echo "Esegui: make report"

report:
	perf report --stdio --no-children -i perf.data 2>/dev/null | head -40
	@echo ""
	@echo "Per l'interfaccia interattiva: perf report -i perf.data"

# ── Flame Graph ───────────────────────────────────────────────
flamegraph: perf.data
	@if [ ! -d $(FG_DIR) ]; then \
	    echo "Clono FlameGraph in $(FG_DIR)..."; \
	    git clone https://github.com/brendangregg/FlameGraph $(FG_DIR); \
	fi
	perf script -i perf.data \
	  | $(FG_DIR)/stackcollapse-perf.pl \
	  | $(FG_DIR)/flamegraph.pl > perf.svg
	@echo "Flame Graph salvato in perf.svg"
	@echo "Aprilo con: firefox perf.svg  oppure  xdg-open perf.svg"

# ── setup permessi (una volta sola, richiede root) ────────────
setup-perf:
	sudo bash -c 'echo -1 > /proc/sys/kernel/perf_event_paranoid'
	sudo bash -c 'echo 0  > /proc/sys/kernel/kptr_restrict'
	@echo "Permessi perf impostati per questa sessione."

clean:
	rm -f $(BIN) perf.data perf.svg
