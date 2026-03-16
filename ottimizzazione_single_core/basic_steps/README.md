# Esercizio: Regole di Base

Questo esercizio mostra come ottimizzare progressivamente un **loop computazionale**.

L'obiettivo è osservare come **piccole modifiche al codice** possano produrre miglioramenti significativi nelle prestazioni.

Il codice di partenza simula un semplice calcolo fisico: per ogni particella viene calcolata la distanza dal centro delle celle di una griglia e si accumula il contributo delle particelle entro una distanza massima. 

---

# Obiettivo dell’esercizio

Partendo dal codice iniziale:

```
loop.0.c
```

dovete applicare diverse tecniche di **ottimizzazione dei loop**.

Le ottimizzazioni sono presentate nelle slide e implementate nei file successivi.

---

# Versioni del codice

| File       | Slide | Descrizione                        |
| ---------- | ----- | ---------------------------------- |
| `loop.0.c` | #6    | versione iniziale (baseline)       |
| `loop.1.c` | #7    | prima ottimizzazione               |
| `loop.2.c` | #8    | ulteriore miglioramento            |
| `loop.3.c` | #9    | eliminazione di operazioni costose |
| `loop.4.c` | #11   | hoisting di espressioni            |
| `loop.5.c` | #14   | ulteriore miglioramento            |
| `loop.6.c` | #12   | versione finale                    |

---

# Ottimizzazioni introdotte

## (1) Evitare chiamate a funzioni costose

Nei primi passi dell’esercizio si eliminano chiamate a funzioni matematiche costose.
Queste funzioni sono molto più lente rispetto a semplici operazioni aritmetiche.

---

## (2) Hoisting delle espressioni

Il **loop hoisting** consiste nello spostare fuori dal loop le espressioni che non cambiano tra iterazioni.

---

## (3) Miglioramenti strutturali

In questa fase si applicano ulteriori ottimizzazioni, ad esempio:

* riduzione delle operazioni ripetute
* miglior utilizzo delle variabili temporanee
* riduzione delle conversioni di tipo

---

# Compilazione

Compilare ogni versione con:

```bash
gcc -std=c11 -o gcc/loop.n loop.n.c -lm
```

Esempio:

```bash
gcc -std=c11 -o gcc/loop.0 loop.0.c -lm
gcc -std=c11 -o gcc/loop.1 loop.1.c -lm
gcc -std=c11 -o gcc/loop.2 loop.2.c -lm
```

---

# Esecuzione

Il programma richiede:

```
Np  Ng  [iterations]
```

dove:

| parametro    | significato                      |
| ------------ | -------------------------------- |
| `Np`         | numero di particelle             |
| `Ng`         | dimensione della griglia         |
| `iterations` | numero di iterazioni (opzionale) |

Esempio:

```bash
./loop.0 1000 20
```

---

# Confronto delle prestazioni

Eseguire tutte le versioni e confrontare i tempi:

```
./loop.0 1000 20
./loop.1 1000 20
./loop.2 1000 20
./loop.3 1000 20
./loop.4 1000 20
./loop.5 1000 20
./loop.6 1000 20
```

Osservare:

* come diminuisce il tempo di esecuzione
* quale ottimizzazione ha l'impatto maggiore

---

# Ottimizzazione del compilatore

Dopo aver analizzato le versioni manuali, verificare cosa succede usando l'ottimizzazione del compilatore.

Compilare con:

```bash
gcc -std=c11 -O3 -march=native -o gcc/loop.n.O loop.n.c -lm
```

Esempio:

```bash
gcc -std=c11 -O3 -march=native -o gcc/loop.0.O loop.0.c -lm
```

Confrontare i risultati con la versione non ottimizzata.
