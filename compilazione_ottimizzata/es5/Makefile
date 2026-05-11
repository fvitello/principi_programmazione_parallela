# =============================================================
# Makefile — Lab 5: gperftools
#
# Funziona su Linux e macOS (con `brew install gperftools`)
# =============================================================

CC      = gcc
SRC     = gperftools_target.c
BIN     = gperftools_target

# ── Rileva il sistema operativo ──────────────────────────────
UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
    # macOS: usa brew per trovare gperftools
    GPERFTOOLS_PREFIX := $(shell brew --prefix gperftools 2>/dev/null)
    ifeq ($(GPERFTOOLS_PREFIX),)
        $(error "gperftools non trovato. Installa con: brew install gperftools")
    endif
    CFLAGS  = -O2 -g -I$(GPERFTOOLS_PREFIX)/include
    LDFLAGS = -L$(GPERFTOOLS_PREFIX)/lib -lprofiler -lm
    LIBPROFILER = $(GPERFTOOLS_PREFIX)/lib/libprofiler.dylib
    PRELOAD_VAR = DYLD_INSERT_LIBRARIES
else
    # Linux
    CFLAGS  = -O2 -g
    LDFLAGS = -lprofiler -lm
    LIBPROFILER = /usr/lib/x86_64-linux-gnu/libprofiler.so
    PRELOAD_VAR = LD_PRELOAD
endif

.PHONY: all run pprof-text pprof-pdf pprof-svg pprof-web \
        preload install-mac install-linux clean

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "Compilato. Esegui: make run"

# ── esecuzione e generazione profili ─────────────────────────
run: $(BIN)
	./$(BIN)
	@echo ""
	@echo "Profili generati: matmul.prof e full.prof"
	@echo "Analizza con:     make pprof-text"

# ── analisi con pprof ─────────────────────────────────────────
pprof-text: matmul.prof full.prof
	@echo "=== matmul.prof (solo kernel critico) ==="
	pprof --text ./$(BIN) matmul.prof
	@echo ""
	@echo "=== full.prof (programma completo) ==="
	pprof --text ./$(BIN) full.prof

pprof-pdf: matmul.prof
	pprof --pdf ./$(BIN) matmul.prof > matmul.pdf
	@echo "Grafo salvato in matmul.pdf"

pprof-svg: matmul.prof
	pprof --svg ./$(BIN) matmul.prof > matmul.svg
	@echo "Grafo salvato in matmul.svg — aprilo con: open matmul.svg"

pprof-web: matmul.prof
	pprof --web ./$(BIN) matmul.prof

# ── Lab 5A: LD_PRELOAD (zero modifiche al sorgente) ──────────
# Prima compila senza -lprofiler:
ldpreload-bin: $(SRC)
	$(CC) -O2 -g -o $(BIN)_nolib $< -lm

preload: ldpreload-bin
	CPUPROFILE=ldpreload.prof \
	$(PRELOAD_VAR)=$(LIBPROFILER) \
	./$(BIN)_nolib
	@echo "Profilo LD_PRELOAD salvato in ldpreload.prof"
	pprof --text ./$(BIN)_nolib ldpreload.prof

# ── installazione ─────────────────────────────────────────────
install-mac:
	brew install gperftools graphviz
	@echo "Installazione completata."

install-linux:
	sudo apt-get install -y google-perftools libgoogle-perftools-dev graphviz
	@echo "Installazione completata."

clean:
	rm -f $(BIN) $(BIN)_nolib matmul.prof full.prof ldpreload.prof \
	      matmul.pdf matmul.svg
