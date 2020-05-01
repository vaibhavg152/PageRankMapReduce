For compiling part a:
    make a
For compiling part b:
    make b
For compiling part c:
    make c

For running part a:
    ./mr-pr-cpp.o {filename}.txt {num_processors} -o {filename}-pr-cpp.txt
For running part b:
    mpirun -np {num_processors} ./mr-pr-mpi {filename}.txt
For running part c:
    mpirun -np {num_processors} mr-pr-mpi-base {filename}.txt
