#!/bin/bash

# Esempio 5B: secondo step di workflow.
# Obiettivo: partire solo dopo il successo dello step1 (dependency afterok).
# Questo job rappresenta la fase di "post-processing".

#SBATCH --job-name=step2_post
#SBATCH --output=logs/slurm-%x-%j.out
#SBATCH --error=logs/slurm-%x-%j.err
#SBATCH --time=00:01:00

# Prende l'ultimo file prodotto da step1.
last_file="$(ls -1t results/pre_*.txt 2>/dev/null | head -n 1)"

# Se manca input, fallisce esplicitamente (utile in didattica).
if [ -z "$last_file" ]; then
  echo "Nessun file pre_*.txt trovato in results/" 1>&2
  exit 1
fi

echo "Leggo il file prodotto da step1: $last_file"
cat "$last_file"
echo "Post-processing completato su $(hostname)"
