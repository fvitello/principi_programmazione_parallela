#!/bin/bash

# Esempio 3: separazione stdout/stderr.
# Obiettivo: capire perche' conviene avere file .out e .err distinti.
# Nota didattica: errori e warning su stderr evitano confusione nei log normali.

#SBATCH --job-name=stdout_stderr
#SBATCH --output=logs/slurm-%x-%j.out
#SBATCH --error=logs/slurm-%x-%j.err
#SBATCH --time=00:01:00

# stdout: output "normale" del programma.
echo "Messaggio normale su stdout (va nel file .out)"
# stderr: output di errore/diagnostica.
echo "Messaggio di warning simulato su stderr (va nel file .err)" 1>&2
echo "Altra riga su stdout"
