# Esperimento: pipelining della CPU e loop unrolling

## Obiettivo dell’esperimento

Questo esercizio mostra come il **pipelining delle CPU moderne** possa essere sfruttato migliorando la struttura dei loop.

Analizzeremo diverse implementazioni di una **riduzione (scan/sum) di un array**, osservando come cambiano le prestazioni quando si:

* elimina la **dipendenza tra iterazioni**
* introduce **loop unrolling**
* usa **più accumulatori**
* sfrutta **vectorizzazione**

L’esperimento dimostra come queste tecniche permettano al processore di **riempire meglio la pipeline e aumentare l’Instruction Level Parallelism (ILP)**.


# Compilazione

Compilare senza e con ottimizzazione:

```bash
gcc -O3 reduction.c -o reduction
```

Per usare anche i contatori hardware PAPI:

```bash
gcc -O3 -DUSE_PAPI reduction.c -lpapi -o reduction
```

---

# Esecuzione dell’esperimento

Sintassi:

```bash
./reduction [scan_type] [N]
```

dove:

```
scan_type = versione del kernel [0=Versione semplice; 1=Loop unrolling 2×1; 2=Loop unrolling con due accumulatori; 3=Loop unrolling 4×4; 4=Vectorized reduction; 5=Unrolling avanzato]
N         = dimensione dell'array [es N=1000000]
```

---

# Output del programma

Il programma stampa:

```
timing for scan is ...
```

e, se PAPI è attivo:

```
IPC
time-per-element
cycles-per-element
L1miss-per-element
```

Questi valori permettono di osservare:

* quanto la pipeline è sfruttata
* quante istruzioni vengono eseguite per ciclo
* l'impatto della memoria

---

# Cosa osservare

Durante l’esperimento confrontare:

### tempo per elemento

```
time-per-element
```

Dovrebbe diminuire passando dalle versioni semplici a quelle con più parallelismo.

---

### cycles per element

```
cycles-per-element
```

Valori più bassi indicano migliore sfruttamento della CPU.

---

### IPC (instructions per cycle)

```
IPC
```

Un valore più alto indica che la CPU riesce a eseguire più istruzioni contemporaneamente.

---

# Risultato atteso

Tipicamente si osserva:

| versione   | comportamento               |
| ---------- | --------------------------- |
| simple     | più lenta                   |
| unroll2x1  | leggermente meglio          |
| unroll2x2  | miglioramento significativo |
| unroll4x4  | molto veloce                |
| vectorized | tra le migliori             |

