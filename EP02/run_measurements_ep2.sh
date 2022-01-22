#! /bin/bash

TEST_ITERATIONS=15
SIZE=4096
THREAD_ITERATIONS=4
THREAD=1

NAMES=('mandelbrot_seq_ep2' 'mandelbrot_omp_ep2' 'mandelbrot_pth_ep2')
NAMES_MPI=('mandelbrot_mpi_pth' 'mandelbrot_mpi_omp')
ONLY_MPI=('mandelbrot_mpi')

rm -r results
make ep2
mkdir -p results

for NAME in ${NAMES[@]}; do
	echo $NAME
    mkdir -p results/$NAME
	for ((j=1; j<=$TEST_ITERATIONS; j++)); do
		./$NAME -0.188 -0.012 0.554 0.754 $SIZE >> triple_spiral.log 2>&1
	done
	mv triple_spiral.log results/$NAME
done
rm output*.ppm

echo $ONLY_MPI
mkdir -p results/$ONLY_MPI
for ((j=1; j<=$TEST_ITERATIONS; j++)); do
	mpirun -np 1 ./$NAME >> triple_spiral.log 2>&1
	mpirun -np 2 ./$ONLY_MPI >> triple_spiral.log 2>&1
	mpirun -np 4 ./$ONLY_MPI >> triple_spiral.log 2>&1
	mpirun -np 8 ./$ONLY_MPI >> triple_spiral.log 2>&1
	mpirun -np 16 ./$ONLY_MPI >> triple_spiral.log 2>&1
	mpirun -np 32 ./$ONLY_MPI >> triple_spiral.log 2>&1
done
mv triple_spiral.log results/$ONLY_MPI
rm output.ppm

for NAME in ${NAMES_MPI[@]}; do
	echo $NAME
	THREAD=1
    mkdir -p results/$NAME
	for ((i=1; i<=$THREAD_ITERATIONS; i++)); do
		for ((j=1; j<=$TEST_ITERATIONS; j++)); do
			mpirun -np 1 ./$NAME i >> triple_spiral.log 2>&1
			mpirun -np 2 ./$NAME $THREAD  >> triple_spiral.log 2>&1
			mpirun -np 4 ./$NAME $THREAD  >> triple_spiral.log 2>&1
			mpirun -np 8 ./$NAME $THREAD  >> triple_spiral.log 2>&1
			mpirun -np 16 ./$NAME $THREAD  >> triple_spiral.log 2>&1
			mpirun -np 32 ./$NAME $THREAD  >> triple_spiral.log 2>&1
		done
		THREAD=$(($THREAD * 2))
	done
	mv triple_spiral.log results/$NAME
	rm output.ppm
done