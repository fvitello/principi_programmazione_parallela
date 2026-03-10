# Memory Mountain

Questa directory contiene il codice per generare una **memory mountain**, come descritta nel libro:

**Computer Systems: A Programmer's Perspective**

## Contenuto della directory

* **clock.{c,h}**
  Routine per utilizzare i timer a ciclo di CPU su architetture **x86** e **Alpha**.

* **fcyc2.{c,h}**
  Routine che stimano il numero di cicli di CPU richiesti da una funzione `f` che riceve due argomenti.

* **Makefile**
  Makefile utilizzato per compilare il programma *memory mountain*.

* **mountain.c**
  Programma che genera i dati della **memory mountain**.

---

# Come eseguire l’esperimento

### 1. Impostare il compilatore

Aprire il `Makefile` e impostare il compilatore desiderato nella parte iniziale del file.

---

### 2. Compilare il programma

Eseguire:

```bash
make
```

---

### 3. Eseguire il programma

Dopo la compilazione verrà creato un eseguibile chiamato:

```bash
mountain.$COMPILER
```

Eseguirlo con:

```bash
./mountain.$COMPILER
```

---

### 4. Salvare l’output

Reindirizzare l’output in un file chiamato:

```bash
mountain.dat
```

Ad esempio:

```bash
./mountain.$COMPILER > mountain.dat
```

---

### 5. Visualizzare i risultati con gnuplot

Aprire **gnuplot**:

```bash
gnuplot
```

e caricare lo script di plotting:

```gnuplot
load "plotmountain.gp"
```

Questo genererà il grafico della **memory mountain**.
