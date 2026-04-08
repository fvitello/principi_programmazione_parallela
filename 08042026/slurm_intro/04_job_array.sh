#!/bin/bash

# Esempio 4: job array.
# Obiettivo: lanciare molti task indipendenti con una sola submit.
# Utile per workload "embarrassingly parallel" (parametri/input indipendenti).

#SBATCH --job-name=array_demo
# %A = JobID dell'array, %a = indice del task nell'array.
#SBATCH --output=logs/slurm-%x-%A_%a.out
#SBATCH --error=logs/slurm-%x-%A_%a.err
#SBATCH --time=00:01:00
# Crea 5 task: ID = 1,2,3,4,5
#SBATCH --array=1-5

# Ogni task dell'array riceve un indice diverso.
id="$SLURM_ARRAY_TASK_ID"

echo "ArrayJobID=$SLURM_ARRAY_JOB_ID TaskID=$id Nodo=$(hostname)"
# Simuliamo lavoro diverso per ogni task.
sleep "$id"
echo "Task $id completato dopo $id secondi"
