For compiling part a (with mapreduce c++ library):
    make a
        
For compiling part b (with own implementation of relevant mapreduce functions):
    make b
    
For compiling part c (with mapreduce MPI library):
    make c



For running part a:

    ./mr-pr-cpp.o {filename}.txt {num_processors} -o {filename}-pr-cpp.txt


For running part b:

    mpirun -np {num_processors} ./mr-pr-mpi {filename}.txt


For running part c:

    mpirun -np {num_processors} mr-pr-mpi-base {filename}.txt
