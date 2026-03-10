# Esperimento: cache e località di memoria nella trasposizione di matrice

## Obiettivo dell’esperimento

Questo esercizio mostra come le **prestazioni della memoria** dipendano dal **pattern di accesso ai dati**.

Useremo diverse implementazioni della **trasposizione di una matrice** per osservare l’impatto di:

* località spaziale
* stride di accesso
* gerarchia di cache

Misureremo:

* **tempo di esecuzione**
* **bandwidth effettiva della memoria**

---

# Programmi inclusi

L’esperimento utilizza due programmi.

## 1️⃣ Trasposizione con diversi pattern di accesso

```id="y9m4x7"
matrix_transpose.c
```

Questo codice implementa due versioni della trasposizione.

### Strided write

```c id="dndu2y"
tmatrix[acc + i] = matrix[offset + j];
```

* lettura contigua
* scrittura con stride = N

---

### Contiguous write

```c id="7snk9b"
tmatrix[offset + j] = matrix[acc + i];
```

* scrittura contigua
* lettura con stride

Il codice produce file di output utilizzabili per generare grafici.

---

## 2️⃣ Trasposizione con ordine dei loop fisso

```id="7ys4op"
matrix_transpose_swapped.c
```

Questo programma esegue una sola versione della trasposizione ma misura la **bandwidth in funzione della dimensione della matrice**.

Il kernel principale è:

```c id="hldj6r"
for (int i = 0; i < N; i++)
{
    int offset = i*N;
    for(int j = 0, acc = 0; j < N; j++, acc+=N)
        array_swap[offset + j] = array[acc + i];
}
```

Questa implementazione esegue:

* **lettura strided**
* **scrittura contigua**

Serve per osservare direttamente come la **dimensione del working set** influenza la banda di memoria.

---

# Gerarchia di cache

Nel codice sono indicati valori tipici:

```id="hzhtp3"
L1 cache ≈ 32 KB
L2 cache ≈ 256 KB
```

Il programma varia automaticamente la dimensione della matrice per osservare cosa succede quando il dataset:

```
L1 → L2 → DRAM
```

---

# Compilazione

Compilare i programmi con ottimizzazione.

```bash id="2r3n9y"
gcc -O3 matrix_transpose.c -o transpose
```

```bash id="ch8r1h"
gcc -O3 matrix_transpose_swapped.c -o transpose_swapped
```

Se si vogliono usare anche i **contatori hardware PAPI**:

```bash id="kecb42"
gcc -O3 -DUSE_PAPI matrix_transpose.c -lpapi -o transpose
```

---

# Esecuzione degli esperimenti

## Esperimento 1 — confronto dei pattern di accesso

Eseguire:

```bash id="bhq7r8"
./transpose 0
```

```bash id="7pxiyc"
./transpose 1
```

Questo produrrà i file:

```id="o10y2x"
transpose_strided_write.dat
transpose_contiguous_write.dat
```

Formato dei dati:

```id="vdr2rz"
N  bandwidth_MBps  time_s
```

---

## Esperimento 2 — banda in funzione della dimensione della matrice

Eseguire:

```bash id="g3h74p"
./transpose_swapped > transpose_swapped.dat
```

Il file prodotto conterrà righe del tipo:

```id="nyf4gn"
size: N bw: bandwidth
```

Questo esperimento mostra come la **bandwidth cambia al crescere del working set**.

---

# Generare i grafici

È fornito lo script:

```id="wdvclu"
plot_transpose.gp
```

Per generare i grafici eseguire:

```bash id="lsac1q"
gnuplot plot_transpose.gp
```

Verranno prodotti:

```id="qow5e6"
transpose_bandwidth.png
transpose_speedup.png
```

---

# Interpretazione dei risultati

## Grafico bandwidth

Mostra:

```
bandwidth vs dimensione della matrice
```

Osservazioni tipiche:

* banda molto alta quando i dati stanno in **L1**
* primo calo quando si supera **L1**
* ulteriore calo quando si supera **L2**
* banda minima quando si accede alla **DRAM**

---

## Grafico speedup

Mostra il rapporto tra le due implementazioni:

```
speedup = contiguous / strided
```

Interpretazione:

| valore | significato                       |
| ------ | --------------------------------- |
| > 1    | accessi contigui più efficienti   |
| ≈ 1    | prestazioni simili                |
| < 1    | pattern strided meno penalizzante |
