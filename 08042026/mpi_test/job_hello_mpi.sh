#!/bin/bash

# Variante consigliata nel cluster didattico:
# MPICH + srun (integrazione Slurm diretta)

# Nome leggibile del job in coda.
#SBATCH --job-name=hello_mpi_mpich

# File di output standard (stdout). %j = JobID Slurm.
#SBATCH --output=logs/hello_mpi_mpich_%j.out

# File di errore standard (stderr).
#SBATCH --error=logs/hello_mpi_mpich_%j.err

# Tempo massimo consentito al job.
#SBATCH --time=00:01:00

# Numero di nodi richiesti.
# In questo primo esempio usiamo un solo nodo.
#SBATCH --nodes=1

# Numero totale di task/rank MPI.
# Con 4 task avremo rank 0,1,2,3.
#SBATCH --ntasks=4

# Ambiente pulito: evita moduli caricati in precedenza.
module purge

# Carica lo stack MPI usato per compilare/eseguire il programma.
# Con MPICH e' corretto lanciare con srun.
module load mpi/mpich

# Mostra i nodi assegnati al job (utile per debug didattico).
scontrol show hostname $SLURM_NODELIST

# Avvia il programma MPI: Slurm crea i rank in base a --ntasks.
# Nota: il binario deve essere compilato con MPICH (mpicc sotto mpi/mpich).
srun ./build/hello_mpi
