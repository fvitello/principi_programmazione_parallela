# Lab 1 - Introduzione a Slurm, OpenMP e MPI

Materiale didattico per il primo laboratorio su cluster Slurm.

## Struttura

- `slurm_intro/`
  esempi base Slurm (batch, risorse, stdout/stderr, job array, dependency)
- `hello_test/`
  primi esempi C seriale e OpenMP
- `mpi_test/`
  primi esempi MPI con script Slurm per MPICH/OpenMPI

## Esecuzione rapida

### 1) Intro Slurm

```bash
cd slurm_intro
sbatch 01_hello_slurm_minimo.sh
sbatch 02_risorse_slurm.sh
sbatch 03_stdout_stderr.sh
sbatch 04_job_array.sh
./07_submit_dependency.sh
```

### 2) Hello OpenMP

```bash
cd hello_test
module purge
module load gcc/13
gcc -O2 -fopenmp hello_omp.c -o build/hello_omp
sbatch job_hello_omp.sh
```

### 3) Hello MPI (MPICH + srun)

```bash
cd mpi_test
module purge
module load mpi/mpich
mpicc hello_mpi.c -o build/hello_mpi
sbatch job_hello_mpi.sh
```

### 4) Hello MPI (OpenMPI + mpirun)

```bash
cd mpi_test
module purge
module load mpi/openmpi
mpicc hello_mpi.c -o build/hello_mpi
sbatch job_hello_mpi_openmpi.sh
```

## Nota pratica

Compila con lo stesso stack usato a runtime:

- MPICH: `module load mpi/mpich` + `mpicc` + `srun`
- OpenMPI: `module load mpi/openmpi` + `mpicc` + `mpirun`
