# Principi della Programmazione Parallela – Codici del Corso

Questo repository contiene esempi di codice, esercizi e piccoli benchmark utilizzati nel corso **Principi della Programmazione Parallela**.

Il materiale ha lo scopo di supportare lezioni teoriche e attività di laboratorio fornendo implementazioni semplici e mirate di concetti fondamentali dell’**High Performance Computing (HPC)** e della **programmazione parallela**.

I programmi sono progettati per essere **compatti, leggibili e didattici**, piuttosto che soluzioni complete o ottimizzate per uso produttivo.

---

## Argomenti del Corso

Gli esempi presenti nel repository illustrano alcuni dei principali concetti trattati nel corso:

- introduzione alle prestazioni e allo scaling
- gerarchie di memoria e località dei dati
- tecniche di ottimizzazione del codice
- parallelismo a memoria condivisa
- parallelismo a memoria distribuita
- modelli di programmazione ibridi
- misurazione e analisi delle prestazioni

Le implementazioni sono volutamente essenziali per permettere agli studenti di concentrarsi sui concetti principali.

---

## Tecnologie Utilizzate

Gli esempi utilizzano strumenti e modelli di programmazione comunemente presenti negli ambienti HPC:

- **C / C++**
- **OpenMP** (parallelismo a memoria condivisa)
- **MPI** (parallelismo a memoria distribuita)
- **Slurm** (esempi di gestione dei job su cluster)
- strumenti di sviluppo standard Unix (`gcc`, `clang`, `make`, ecc.)

I programmi sono pensati per essere compilati con compilatori standard disponibili su sistemi Linux e cluster HPC.

---

## Requisiti

Per compilare ed eseguire gli esempi sono generalmente necessari:

- un ambiente **Linux**
- un compilatore **C/C++** (ad esempio `gcc` o `clang`)
- supporto **OpenMP**
- un’implementazione **MPI** (ad esempio OpenMPI o MPICH)
- accesso a un sistema di gestione dei job come **Slurm** per gli esempi su cluster

Molti programmi possono comunque essere eseguiti anche su una macchina locale.
