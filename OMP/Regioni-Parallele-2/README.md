# OpenMP — Regioni Parallele [2]

Questa raccolta di esempi introduce i concetti base di OpenMP:

- regioni parallele
- numero di thread
- ordine di esecuzione
- assegnazione del lavoro
- regioni parallele annidate

## Compilazione
gcc file.c -fopenmp -o prog
./prog

## 04 – Numero di thread
04a_num_of_threads.c
04b_num_of_threads.c

## 05 – Ordine di esecuzione
05a_order_of_threads.c
05b_order_of_threads.c
05c_order_of_threads.c
05_order_of_threads_wrong.c

L’ordine delle stampe cambia ad ogni esecuzione
Il comportamento non è deterministico
Il file *_wrong.c mostra un uso scorretto
## 06 – Assegnazione del lavoro
06_assign_work.c
06a_assign_work.c

Come il lavoro viene distribuito tra i thread
Se alcuni thread restano inattivi
Differenze tra assegnazione manuale e automatica
## 07 – Regioni parallele annidate
07_simple_nested_regions.c

Come vengono gestite le regioni parallele annidate
Numero effettivo di thread utilizzati