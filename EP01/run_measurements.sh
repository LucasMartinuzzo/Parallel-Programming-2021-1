#! /bin/bash

TEST_ITERATIONS=10
SIZE_ITERATIONS=10
INITIAL_SIZE=16
NUMBER_OF_THREADS=1

SIZE=$INITIAL_SIZE

NAMES=('mandelbrot_seq','mandelbrot_omp', 'mandelbrot_pth')


#rm -r results
make
mkdir -p results

for NAME in ${NAMES[@]}; do
    mkdir -p results/$NAME
	for ((i=1; i<=$SIZE_ITERATIONS; i++)); do
		for ((j=1; j<=$TEST_ITERATIONS; j++)); do
			./$NAME -2.5 1.5 -2.0 2.0 $SIZE >> full.log 2>&1
			./$NAME -0.8 -0.7 0.05 0.15 $SIZE >> seahorse.log 2>&1
			./$NAME 0.175 0.375 -0.1 0.1 $SIZE >> elephant.log 2>&1
			./$NAME -0.188 -0.012 0.554 0.754 $SIZE >> triple_spiral.log 2>&1
		done
		SIZE=$(($SIZE * 2))
	done
	mv *.log results/$NAME
	rm output.ppm
done
