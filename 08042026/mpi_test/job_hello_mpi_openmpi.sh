#!/bin/bash

# Variante alternativa:
# OpenMPI + mpirun (quando OpenMPI non ha supporto PMI/PMIx per srun).

#SBATCH --job-name=hello_mpi_openmpi
#SBATCH --output=logs/hello_mpi_openmpi_%j.out
#SBATCH --error=logs/hello_mpi_openmpi_%j.err
#SBATCH --time=00:01:00
#SBATCH --nodes=1
#SBATCH --ntasks=4

module purge
module load mpi/openmpi

# Mostra i nodi assegnati al job (utile per debug didattico).
scontrol show hostname $SLURM_NODELIST

# Con questa build OpenMPI si usa mpirun dentro l'allocazione Slurm.
# Nota: il binario deve essere compilato con OpenMPI (mpicc sotto mpi/openmpi).
mpirun -np "${SLURM_NTASKS}" ./build/hello_mpi
