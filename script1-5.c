#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VALUE 1000

int* read_data(const char *file_path, int *num_elements) {
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        perror("Cannot open file");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    *num_elements = ftell(file) / sizeof(int);
    rewind(file);

    int *data = (int*) malloc(*num_elements * sizeof(int));
    fread(data, sizeof(int), *num_elements, file);

    fclose(file);
    return data;
}

void write_data(const char *file_path, int *data, int num_elements) {
    FILE *file = fopen(file_path, "wb");
    if (!file) {
        perror("Cannot open file");
        exit(EXIT_FAILURE);
    }

    fwrite(data, sizeof(int), num_elements, file);
    fclose(file);
}

void count_sort(int *data, int num_elements) {
    int count[MAX_VALUE] = {0};

    for (int i = 0; i < num_elements; i++)
        count[data[i]]++;

    int idx = 0;
    for (int i = 0; i < MAX_VALUE; i++) {
        for (int j = 0; j < count[i]; j++) {
            data[idx++] = i;
        }
    }
}

int main(int argc, char *argv[]) {
    int rank, size;
    int *data = NULL;
    int num_elements;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        data = read_data("data_file.bin", &num_elements);
    }

    MPI_Bcast(&num_elements, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int chunk_size = num_elements / size;
    int remainder = num_elements % size;
    if (rank < remainder)
        chunk_size++;

    int *chunk = (int*) malloc(chunk_size * sizeof(int));

    if (rank == 0) {
        int start = 0;
        for (int i = 0; i < size; i++) {
            int current_chunk_size = num_elements / size;
            if (i < remainder)
                current_chunk_size++;

            if (i == 0) {
                memcpy(chunk, data, chunk_size * sizeof(int));
            } else {
                MPI_Send(data + start, current_chunk_size, MPI_INT, i, 0, MPI_COMM_WORLD);
            }

            start += current_chunk_size;
        }
    } else {
        MPI_Recv(chunk, chunk_size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    count_sort(chunk, chunk_size);

    if (rank == 0) {
        memcpy(data, chunk, chunk_size * sizeof(int));
        int start = chunk_size;
        for (int i = 1; i < size; i++) {
            int current_chunk_size = num_elements / size;
            if (i < remainder)
                current_chunk_size++;

            MPI_Recv(data + start, current_chunk_size, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            start += current_chunk_size;
        }
    } else {
        MPI_Send(chunk, chunk_size, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        count_sort(data, num_elements);

        write_data("data_file.bin", data, num_elements);

        free(data);
    }

    free(chunk);
    MPI_Finalize();

    return 0;
}