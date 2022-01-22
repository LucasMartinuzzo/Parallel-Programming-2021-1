OUTPUT=mandelbrot

IMAGE=.ppm

CC=gcc
CC_OPT=-std=c11

MPI=mpicc

CC_OMP=-fopenmp
CC_PTH=-pthread

.PHONY: ep1
ep1: $(OUTPUT)_omp $(OUTPUT)_pth $(OUTPUT)_seq

$(OUTPUT)_omp: $(OUTPUT)_omp.c
	$(CC) -o $(OUTPUT)_omp $(CC_OPT) $(CC_OMP) $(OUTPUT)_omp.c -lm

$(OUTPUT)_pth: $(OUTPUT)_pth.c
	$(CC) -o $(OUTPUT)_pth $(CC_OPT) $(CC_PTH) $(OUTPUT)_pth.c -lm

$(OUTPUT)_seq: $(OUTPUT)_seq.c
	$(CC) -o $(OUTPUT)_seq $(CC_OPT) $(OUTPUT)_seq.c


.PHONY: ep2
ep2: $(OUTPUT)_omp_ep2 $(OUTPUT)_pth_ep2 $(OUTPUT)_seq_ep2 $(OUTPUT)_mpi $(OUTPUT)_mpi_pth $(OUTPUT)_mpi_omp

$(OUTPUT)_omp_ep2: $(OUTPUT)_omp_ep2.c
	$(CC) -o $(OUTPUT)_omp_ep2 $(CC_OPT) $(CC_OMP) $(OUTPUT)_omp_ep2.c -lm

$(OUTPUT)_pth_ep2: $(OUTPUT)_pth_ep2.c
	$(CC) -o $(OUTPUT)_pth_ep2 $(CC_OPT) $(CC_PTH) $(OUTPUT)_pth_ep2.c -lm

$(OUTPUT)_seq_ep2: $(OUTPUT)_seq_ep2.c
	$(CC) -o $(OUTPUT)_seq_ep2 $(CC_OPT) $(OUTPUT)_seq_ep2.c

$(OUTPUT)_mpi: $(OUTPUT)_mpi.c
	$(MPI) -o $(OUTPUT)_mpi $(OUTPUT)_mpi.c -lm

$(OUTPUT)_mpi_pth: $(OUTPUT)_mpi_pth.c
	$(MPI) -o $(OUTPUT)_mpi_pth $(CC_PTH) $(OUTPUT)_mpi_pth.c -lm

$(OUTPUT)_mpi_omp: $(OUTPUT)_mpi_omp.c
	$(MPI) -o $(OUTPUT)_mpi_omp $(CC_OMP) $(OUTPUT)_mpi_omp.c -lm

.PHONY: clean_ep1
clean_ep1:
	rm $(OUTPUT)_omp $(OUTPUT)_pth $(OUTPUT)_seq  *$(IMAGE)

.PHONY: clean_ep2
clean_ep2:
	rm $(OUTPUT)_omp_ep2 $(OUTPUT)_pth_ep2 $(OUTPUT)_seq_ep2  *$(IMAGE)
