#!/bin/bash

# Primo esempio di script batch per Slurm.
# Questo file viene passato a `sbatch job_hello.sh`.
# Le righe che iniziano con #SBATCH sono direttive per lo scheduler.

# Nome leggibile del job (utile in `squeue` e nei log).
#SBATCH --job-name=hello

# File di output standard (stdout).
# `%j` viene sostituito automaticamente con lo JobID assegnato da Slurm.
#SBATCH --output=logs/hello_%j.out

# File di errore standard (stderr), separato da stdout.
#SBATCH --error=logs/hello_%j.err

# Tempo massimo concesso al job (formato HH:MM:SS).
# Se il job supera questo limite, Slurm lo termina.
#SBATCH --time=00:01:00

# Numero di task/processi richiesti.
# Qui usiamo 1 task per eseguire un semplice programma seriale.
#SBATCH --ntasks=1

# Comando da eseguire sul nodo allocato.
# In questo esempio lanciamo l'eseguibile compilato in `build/hello`.
./build/hello
