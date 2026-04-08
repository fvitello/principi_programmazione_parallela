#!/bin/bash

# Primo esempio Slurm per programma OpenMP.
# Si invia con: sbatch job_hello_omp.sh

# Nome del job mostrato da Slurm.
#SBATCH --job-name=hello_omp

# File di output standard (stdout).
# `%j` viene sostituito con lo JobID.
#SBATCH --output=logs/hello_omp_%j.out

# File di errore standard (stderr).
#SBATCH --error=logs/hello_omp_%j.err

# Tempo massimo di esecuzione.
#SBATCH --time=00:01:00

# OpenMP in genere usa 1 task con piu thread.
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4

# Numero di thread OpenMP: in genere allineato alle CPU richieste a Slurm.
# OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK}

# Esegue il binario OpenMP compilato.
./build/hello_omp