#!/bin/bash

# Esempio 2: introspezione risorse assegnate da Slurm.
# Obiettivo: leggere le variabili d'ambiente principali del job.

# Etichetta job.
#SBATCH --job-name=risorse_slurm
# Log standard/error.
#SBATCH --output=logs/slurm-%x-%j.out
#SBATCH --error=logs/slurm-%x-%j.err
# Limite tempo.
#SBATCH --time=00:01:00
# Risorse richieste:
# - 1 nodo
# - 1 task (1 processo)
# - 2 CPU assegnate a quel task
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=2

# Variabili d'ambiente settate automaticamente da Slurm.
echo "SLURM_JOB_ID=$SLURM_JOB_ID"
echo "SLURM_JOB_NAME=$SLURM_JOB_NAME"
echo "SLURM_JOB_NODELIST=$SLURM_JOB_NODELIST"
echo "SLURM_NNODES=$SLURM_NNODES"
echo "SLURM_NTASKS=$SLURM_NTASKS"
echo "SLURM_CPUS_PER_TASK=$SLURM_CPUS_PER_TASK"
echo "SLURM_SUBMIT_DIR=$SLURM_SUBMIT_DIR"
# Nodo reale su cui sta girando questo script.
echo "Hostname locale: $(hostname)"
