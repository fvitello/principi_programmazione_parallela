

#include <mpi.h>
#include <stdio.h>
#include <string.h>

#define MAXLEN 64

int main(int argc, char **argv)
{
    int  rank, size;
    char msg[MAXLEN];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("[rank 0] sono il root, ci sono %d processi\n", size);

        /* Ricevere in ordine da rank 1, 2, ..., size-1.
         * src=k forza l'ordine: non accettiamo "chi arriva prima". */
        for (int k = 1; k < size; k++) {
            MPI_Recv(msg, MAXLEN, MPI_CHAR, k, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("%s\n", msg);
        }
    } else {
        /* Costruire il messaggio e inviarlo al processo 0 */
        /* Costruisce la stringa da inviare nel buffer msg.
        * snprintf: come sprintf ma sicuro — non supera mai MAXLEN byte,
        * evitando buffer overflow se la stringa formattata fosse troppo lunga. */
        snprintf(msg, MAXLEN, "[rank %d] sono un worker", rank);
        //strlen(msg)+1: inviamo anche il terminatore '\0
        MPI_Send(msg, strlen(msg) +1 , MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
