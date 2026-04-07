# OpenMP — Regioni Parallele [1]

Questa raccolta di esempi introduce i concetti base di OpenMP:

- regioni parallele
- variabili private/shared
- data scoping clauses

## Compilazione
gcc file.c -fopenmp -o prog
./prog

## 0️⃣ Stack e memoria

📄 00_stack_and_scope.c

👉 Cosa osservare:

ogni thread ha stack separato
variabili private vs shared
indirizzi di memoria diversi tra thread

💡 Esperimento:

cambia OMP_STACKSIZE
osserva come cambiano gli indirizzi

## 1️⃣ Primo parallel region (errore)

📄 01_simple_pr_wrong.c

👉 Cosa succede:

uso scorretto di variabili condivise
possibile comportamento non deterministico

## 2️⃣ Versione corretta

📄 02_simple_pr.c

👉 Fix:

variabili dichiarate private
comportamento corretto

##🔁 Clausole OpenMP
##3️⃣ firstprivate

📄 03_clauses__firstprivate.c

👉 Ogni thread:

riceve una copia inizializzata
poi lavora in modo indipendente

## 4️⃣ lastprivate

📄 03_clauses__lastprivate.c

👉 Alla fine del loop:

il valore dell’ultima iterazione viene copiato fuori

💡 Attenzione:

“ultima” = ordine logico del loop, NON thread
## 5️⃣ threadprivate

📄 03_clauses__threadprivate.c

👉 Variabili:

persistono tra regioni parallele
ogni thread ha la sua copia

💡 Importante:

memoria allocata per thread → va liberata in parallelo
## 6️⃣ copyin

📄 03_clauses__copyin.c

👉 All’inizio della regione:

copia il valore del master a tutti i thread

💡 Nota:

utile con threadprivate
## 7️⃣ copyprivate

📄 03_clauses__copyprivate.c

👉 Dentro single:

un thread inizializza
valore copiato a tutti gli altri

💡 Pattern tipico:

inizializzazione condivisa
