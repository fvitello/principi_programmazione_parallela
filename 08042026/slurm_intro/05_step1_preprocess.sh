#!/bin/bash

# Esempio 5A: primo step di workflow.
# Obiettivo: produrre un file che verra' letto dallo step successivo.
# Questo job rappresenta una fase di "pre-processing".

#SBATCH --job-name=step1_pre
#SBATCH --output=logs/slurm-%x-%j.out
#SBATCH --error=logs/slurm-%x-%j.err
#SBATCH --time=00:01:00

sleep 15 # Simuliamo un lavoro che richiede tempo.
# Crea la cartella output se non esiste.
mkdir -p results
# Nome file che include il JobID per evitare collisioni tra run diversi.
outfile="results/pre_${SLURM_JOB_ID}.txt"

# Produzione di un artefatto semplice da passare allo step2.
echo "Output generato da step1, job $SLURM_JOB_ID, nodo $(hostname)" > "$outfile"
echo "Creato file: $outfile"
