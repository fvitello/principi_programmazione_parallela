# Esperimento: ottimizzazione dei loop e località di memoria

## Obiettivo dell’esperimento

Questo laboratorio mostra come semplici modifiche alla struttura dei **loop annidati** possano migliorare drasticamente le prestazioni.

Utilizzeremo diverse implementazioni della **moltiplicazione di matrici** per osservare l'effetto di:

* riordinamento dei loop
* località spaziale della memoria
* blocking (loop tiling)
* riduzione dei cache miss

Il prodotto di matrici calcolato è:

```
C(n,o) = A(n,m) × B(m,o)
```

---

# Programmi inclusi

Questo laboratorio utilizza due programmi principali:

```
matmul_simple.c
matmul.c
```

---

# Programma 1 — semplice esempio di loop optimization

File:

```
matmul_simple.c
```

Questo programma confronta due versioni della moltiplicazione di matrici.

## Versione non ottimizzata

```id="code1"
for (i = 0; i < n; i++)
    for (j = 0; j < o; j++)
        for (k = 0; k < m; k++)
            c[i*o + j] += a[i*m + k] * b[k*o + j];
```

Qui l'accesso alla matrice `B` avviene con **stride grande**:

```
b[k*o + j]
```

Questo provoca molti **cache miss**.

---

## Versione ottimizzata

```id="code2"
for (i = 0; i < n; i++)
    for (k = 0; k < m; k++)
        for (j = 0; j < o; j++)
            c[i*o + j] += a[i*m + k] * b[k*o + j];
```

Il semplice **scambio dei loop interni** migliora l'accesso alla matrice `B`.

Gli elementi di `B` vengono ora attraversati **in modo contiguo**.

Questo riduce drasticamente i **cache miss**. 

---

# Compilazione

```bash
gcc -O3 matmul_simple.c -o matmul_simple
```

---

# Esecuzione

Sintassi:

```bash
./matmul_simple version n m o
```

dove

```
version = 0  versione non ottimizzata
version = 1  versione ottimizzata
```

Esempio:

```bash
./matmul_simple 0 1000 1000 1000
./matmul_simple 1 1000 1000 1000
```

Confrontare i tempi di esecuzione.

---

# Programma 2 — ottimizzazione avanzata con blocking

File:

```
matmul.c
```

Questo programma implementa tre versioni della moltiplicazione.

---

## Versione 0 — naive

Implementazione diretta della definizione matematica:

```id="code3"
C[i][k] += A[i][j] * B[j][k]
```

Gli accessi alla matrice `B` sono **strided**, causando molti miss di cache. 

---

## Versione 1 — loop reordered

I loop interni sono scambiati per migliorare la località:

```id="code4"
for i
  for j
    for k
```

Questo rende l'accesso alla matrice `B` **contiguo in memoria**.

Riduce significativamente i **cache miss L1**.

---

## Versione 2 — blocking (tiling)

Questa versione divide la matrice in **blocchi più piccoli**.

Idea principale:

```
operare su sotto-matrici che entrano nella cache
```

Questo migliora:

* riuso dei dati
* località temporale
* utilizzo della cache.

---

# Compilazione

```bash
gcc -O3 matmul.c -o matmul
```

Versione con contatori hardware:

```bash
gcc -O3 -DUSE_PAPI matmul.c -lpapi -o matmul
```

Il file `mypapi.h` gestisce i contatori hardware per:

* istruzioni
* cicli CPU
* cache miss L1 

---

# Esecuzione

Sintassi:

```bash
./matmul version n m o [blockA blockB blockC]
```

dove

```
version = 0  naive
version = 1  loop reordered
version = 2  blocked
```

Esempio:

```bash
./matmul 0 1024 1024 1024
./matmul 1 1024 1024 1024
./matmul 2 1024 1024 1024 128 128 128
```

---

# Cosa osservare

Durante l’esperimento confrontare:

### tempo di esecuzione

La versione naive dovrebbe essere la più lenta.

---

### località di memoria

Il riordinamento dei loop migliora l'accesso alla matrice `B`.

---

### blocking

Il blocking riduce drasticamente i cache miss per matrici grandi.

---

# Risultato atteso

Tipicamente:

| versione        | prestazioni      |
| --------------- | ---------------- |
| naive           | molto lenta      |
| reordered loops | molto più veloce |
| blocked         | la più veloce    |

---

# Cosa dimostra questo esperimento

Le prestazioni dei programmi numerici dipendono fortemente da:

```
organizzazione dei loop
↓
pattern di accesso alla memoria
↓
cache behavior
```

Piccoli cambiamenti nella struttura dei loop possono produrre **speedup molto grandi**.
