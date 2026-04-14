#!/bin/bash

# Esempio MPI multi-nodo con OpenMPI:
# 4 processi totali distribuiti su 2 nodi (2 processi per nodo).

#SBATCH --job-name=hello_mpi_2nodes_ompi
#SBATCH --output=logs/hello_mpi_2nodes_ompi_%j.out
#SBATCH --error=logs/hello_mpi_2nodes_ompi_%j.err
#SBATCH --time=00:01:00
#SBATCH --nodes=2
#SBATCH --ntasks=4
#SBATCH --ntasks-per-node=2

module purge
module load mpi/openmpi

# Mostra i nodi assegnati (utile per verificare che siano 2).
scontrol show hostname $SLURM_NODELIST

# Con questa build OpenMPI usiamo mpirun nell'allocazione Slurm.
# Nota: il binario deve essere compilato con OpenMPI (mpicc sotto mpi/openmpi).
mpirun -np "${SLURM_NTASKS}" ./build/hello_mpi
