# Aliasing di memoria e ottimizzazione del compilatore

## Obiettivo dell’esperimento

Questo esperimento mostra come l’**aliasing dei puntatori** possa limitare le ottimizzazioni del compilatore.

---

# L’esempio

Il programma definisce due funzioni:

```c
void func1(int *a, int *b){
    *a += *b;
    *a += *b;
}

void func2(int *a, int *b){
    *a += 2 * *b;
}
```

A prima vista le due funzioni sembrano equivalenti, perché:

```
*a += *b
*a += *b
```

sembra equivalente a:

```
*a += 2 * *b
```

Tuttavia questo **non è sempre vero**.

---

# Aliasing vs non-aliasing

## Caso 1 — Nessun aliasing

```
a -> x
b -> y
```

Esempio:

```
x = 1
y = 1
```

Esecuzione:

```
func1:
x = 1 + 1 + 1 = 3

func2:
x = 1 + 2*1 = 3
```

Le due funzioni producono **lo stesso risultato**.

---

## Caso 2 — Aliasing

```
a -> x
b -> x
```

Esempio:

```
x = 1
```

Esecuzione:

```
func1:
x = 1 + 1 = 2
x = 2 + 2 = 4

func2:
x = 1 + 2*1 = 3
```

Ora i risultati sono **diversi**.

Questo significa che il compilatore **non può sostituire `func1` con `func2`** se esiste la possibilità che `a` e `b` aliasino.

---

# Eseguire l’esperimento

Compilare il programma:

```
gcc -O0 es_memory_aliasing.c -o es_memory_aliasing
```

Eseguire:

```
./es_memory_aliasing
```

Osservare i risultati nei casi:

```
non-alias
alias
```

Dovreste vedere che `func1` e `func2` si comportano diversamente quando c’è aliasing.

---

# Analizzare le ottimizzazioni del compilatore

Compilare generando l’assembly:

```
gcc -O3 -S es_memory_aliasing.c
```

Esaminare il file `.s` generato e confrontare il codice delle funzioni:

```
func1
func2
```

---

# Esperimento: usare `restrict`

Modificare la definizione della funzione:

```c
void func1(int *restrict a, int *restrict b)
```

La parola chiave `restrict` indica al compilatore che i due puntatori **non aliasano**.

Ricompilare:

```
gcc -O3 es_memory_aliasing.c
```

Poi analizzare di nuovo l’assembly.

Domande:

* Il compilatore applica più ottimizzazioni?
* Il codice generato è più simile a `func2`?

---

# Esperimento opzionale sulle prestazioni

Aumentare il numero di iterazioni nel programma:

```
const int iters = 100000000;
```

Compilare con ottimizzazione:

```
gcc -O3 es_memory_aliasing.c
```

Eseguire il programma e confrontare i tempi.