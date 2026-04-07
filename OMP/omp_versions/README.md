# OpenMP Version Detector

Questo programma mostra:

quale compilatore stai usando
quale versione di OpenMP è supportata

## Compilazione
Linux
gcc omp_versions.c -fopenmp -o omp_versions

▶️ Esecuzione
./omp_versions

🧠 Come funziona (in breve)
Il programma usa macro del compilatore per identificarlo
Se OpenMP è attivo (-fopenmp), legge la macro _OPENMP
Confronta il valore con una tabella e stampa la versione
