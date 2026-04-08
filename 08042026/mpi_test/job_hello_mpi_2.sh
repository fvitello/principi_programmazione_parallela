#!/bin/bash

# Esempio MPI multi-nodo:
# 4 processi totali distribuiti su 2 nodi (2 processi per nodo).

#SBATCH --job-name=hello_mpi_2nodes
#SBATCH --output=logs/hello_mpi_2nodes_%j.out
#SBATCH --error=logs/hello_mpi_2nodes_%j.err
#SBATCH --time=00:01:00

# Numero di nodi richiesti.
#SBATCH --nodes=2

# Numero totale di rank MPI del job.
#SBATCH --ntasks=4

# Distribuzione: 2 rank su ciascun nodo.
#SBATCH --ntasks-per-node=2

# Ambiente pulito e caricamento stack MPI coerente con srun.
module purge
module load mpi/mpich

# Mostra i nodi assegnati (utile per verificare la distribuzione).
scontrol show hostname $SLURM_NODELIST

# Avvia 4 rank MPI distribuiti secondo le direttive sopra.
srun ./build/hello_mpi
