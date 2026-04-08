#!/bin/bash

# Esempio 1: job batch minimo.
# Obiettivo: vedere il ciclo base "scrivo script -> sbatch -> leggo output".
# Sequenza tipica:
# 1) `sbatch 01_hello_slurm_minimo.sh`
# 2) controllo stato con `squeue`
# 3) leggo i file .out/.err prodotti dal job

# Nome logico del job in coda.
#SBATCH --job-name=hello_slurm
# File stdout del job:
# - %x = nome job
# - %j = JobID assegnato da Slurm
#SBATCH --output=logs/slurm-%x-%j.out
# File stderr del job.
#SBATCH --error=logs/slurm-%x-%j.err
# Tempo massimo concesso.
#SBATCH --time=00:01:00

# Corpo del job: questi comandi girano sul nodo allocato.
echo "Ciao da Slurm"
echo "JobID: $SLURM_JOB_ID"
echo "Nodo: $(hostname)"
echo "Start: $(date)"
# Sleep solo per rendere visibile il job in `squeue` per qualche secondo.
sleep 5
echo "End:   $(date)"
