#!/bin/bash

# invia step1 e poi step2 con vincolo afterok:<jobid_step1>.

# Modalita' "strict":
# -e  esce al primo comando che fallisce
# -u  errore se una variabile non e' definita
# pipefail  in una pipeline fallisce se fallisce uno qualunque dei comandi
set -euo pipefail

# --parsable restituisce solo il JobID, comodo da salvare in variabile.
job1="$(sbatch --parsable 05_step1_preprocess.sh)"
echo "Step1 inviato con JobID=$job1"

# Step2 parte solo se Step1 termina con stato OK.
job2="$(sbatch --parsable --dependency=afterok:${job1} 06_step2_postprocess.sh)"
echo "Step2 inviato con JobID=$job2 (dipende da afterok:$job1)"
