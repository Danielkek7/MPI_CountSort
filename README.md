# MPI_CountSort
This code is an implementation of the parallel Count Sort algorithm using the Message Passing Interface (MPI) to distribute, sort, and gather data chunks across multiple processes. We divide the dataset into smaller chunks, distribute these chunks to separate processes, have each process sort its chunk, and then gather the sorted chunks back to the master process which performs a final sort.

"main()" function:
The main function initializes the MPI environment and identifies the rank (unique ID) and size (total number of processes) for each process. The master process (rank 0) reads data from a binary file and broadcasts the number of total elements to all other processes. Based on the total number of elements and the number of processes, each process calculates its chunk size. The master process then distributes the chunks of data to each process, which in turn sorts its own chunk using the count sort algorithm. After sorting, all processes send their sorted chunks back to the master process. The master process then performs a final count sort on the aggregated data and writes the sorted data back to the binary file.

"read_data(const char *file_path, int *num_elements)" function:
This function reads integers from a binary file. It calculates the number of integers by dividing the file size by the size of an integer. It allocates memory for these integers and reads them into an array, then returns this array and updates the "num_elements" variable with the total count.

"write_data(const char *file_path, int *data, int num_elements)" function:
This function writes the provided integer array back into a binary file. 

"count_sort(int *data, int num_elements)" function:
This function implements the Count Sort algorithm. It first initializes a count array, where each index represents an integer value and its value represents the number of occurrences of that integer in the data. The function then counts the occurrences of each integer in the provided data. It then constructs the sorted array by filling in the values based on their counts.
