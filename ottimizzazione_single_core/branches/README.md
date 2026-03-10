# Esperimento: ottimizzazione dei branch e branch prediction

## Obiettivo dell’esperimento

Questo laboratorio dimostra l’impatto dei **branch condizionali** sulle prestazioni delle CPU moderne.

I processori moderni usano **branch prediction** per mantenere la pipeline piena.
Quando un branch è **predicibile**, l'esecuzione è veloce.
Quando un branch è **imprevedibile**, la pipeline viene svuotata causando **branch misprediction** e perdita di prestazioni.

In questo esercizio analizzeremo:

* l'effetto dei branch nei loop
* l'effetto dei dati **predicibili vs casuali**
* tecniche per **ridurre o eliminare i branch**

I programmi usati sono:

```
branch_prediction.c
crosssort.c
loop.c
```

---

# Concetto chiave: branch prediction

Consideriamo il codice:

```c
if (x > threshold)
    sum += x;
```

Se il risultato della condizione cambia frequentemente, il **branch predictor** della CPU può sbagliare spesso.

Una **misprediction** comporta:

* flush della pipeline
* perdita di molti cicli di CPU.

---

# Es. 1 — branch condizionali all'interno dei cicli

File:

```
loop.c
```

Questo programma confronta diverse implementazioni di un **branch complesso all’interno di un loop** .

Tre approcci:

1️⃣ **if-forest**

Molti branch annidati:

```c
if(...)
   if(...)
      if(...)
```

Questo crea molti possibili percorsi.

---

2️⃣ **function pointer**

Il branch viene eseguito **una sola volta prima del loop**.

Il loop diventa:

```c
func(&result, i);
```

Riducendo i branch nel loop.

---

3️⃣ **switch**

La scelta del caso viene calcolata prima del loop.

Questo riduce la complessità dei branch nel corpo del loop.

---

# Compilazione

```bash
gcc -O3 loop.c -o loop
```

---

# Esecuzione

```
./loop 0
./loop 1
./loop 2
```

dove

| mode | implementazione  |
| ---- | ---------------- |
| 0    | if forest        |
| 1    | function pointer |
| 2    | switch           |

---

# Cosa osservare

Confrontare i tempi.

Generalmente:

| versione         | prestazioni     |
| ---------------- | --------------- |
| if forest        | più lenta       |
| switch           | migliore        |
| function pointer | spesso migliore |

Questo mostra l'effetto della **riduzione dei branch nel loop**.

---

# Es. 2 — flussi di dati imprevedibili

File:

```
branch_prediction.c
```

Questo programma confronta diverse strategie per evitare o ridurre i branch nei loop .

## Versione base

```c
if (data[i] > PIVOT)
    sum += data[i];
```

Qui il branch dipende dai dati.

---

## Versione branchless

Con compilazione:

```
-DBESMART
```

viene usata una versione senza branch:

```c
unsigned int t = (data[i] - PIVOT - 1) >> 31;
sum += ~t & data[i];
```

Questo usa operazioni bitwise invece del branch.

---

## Versione con operatore ternario

```
-DBESMART2
```

```c
sum += (data[i] > PIVOT ? data[i] : 0);
```

Spesso il compilatore genera **CMOV** invece di un branch.

---

## Effetto del sorting

Compilando con

```
-DWOW
```

il programma ordina i dati prima del loop.

Questo rende il branch **molto predicibile**.

---

# Compilazione

Versione base:

```bash
gcc -O3 branch_prediction.c -o branch
```

Versione branchless:

```bash
gcc -O3 -DBESMART branch_prediction.c -o branch_smart
```

Versione ternary:

```bash
gcc -O3 -DBESMART2 branch_prediction.c -o branch_smart2
```

Versione con sorting:

```bash
gcc -O3 -DWOW branch_prediction.c -o branch_sorted
```

---

# Esecuzione

```bash
./branch
./branch_smart
./branch_smart2
./branch_sorted
```

Confrontare i tempi.

---

# Cosa osservare

Tipicamente:

| versione    | comportamento         |
| ----------- | --------------------- |
| branch base | lenta con dati random |
| branchless  | più stabile           |
| sorted      | molto veloce          |

Questo dimostra quanto il **branch predictor dipenda dalla distribuzione dei dati**.


---

# Programma 3 — ordinamento di due array

File:

```
crosssort.c
```

Questo programma mostra una tecnica per eliminare un branch nello scambio di due valori .

Versione standard:

```c
if (B[i] > A[i])
    swap(A[i], B[i]);
```

Versioni smart:

```
-DBESMART
-DBESMART2
-DBESMART3
```

usano operazioni aritmetiche o bitwise per evitare il branch.

---

# Compilazione

Versione base:

```bash
gcc -O3 crosssort.c -o crosssort -lm
```

Versione branchless:

```bash
gcc -O3 -DBESMART crosssort.c -o crosssort_smart -lm
```

Attivazione librerie PAPI:

```bash
gcc -O3 crosssort.c -o crosssort.PAPI -DUSE_PAPI -lpapi -lm
```
---

# Esecuzione

```
./crosssort
./crosssort_smart
```

---

# Cosa osservare

Il programma può anche misurare con **PAPI**:

* branches per elemento
* mispredicted branches
* cycles per element

Questo permette di correlare direttamente:

```
branch misprediction
↓
pipeline stall
↓
prestazioni
```

---

# Risultato atteso

Gli esperimenti mostrano che:

* i branch nei loop sono costosi
* i branch imprevedibili sono molto costosi
* eliminare branch può migliorare significativamente le prestazioni
