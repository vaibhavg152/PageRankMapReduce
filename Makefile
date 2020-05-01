a:
	g++ mr-pr-cpp.cpp /usr/lib/x86_64-linux-gnu/libboost_system.a /usr/lib/x86_64-linux-gnu/libboost_iostreams.a /usr/lib/x86_64-linux-gnu/libboost_filesystem.a -pthread -o mr-pr-cpp.o -I a_src
b:
	mpic++ mr-pr-mpi.cpp -o mr-pr-mpi.o
c:
	mpic++ -c mr-pr-mpi-base.cpp -o mr-pr-mpi-base.o -I mpi_src
	mpic++ -g -O mr-pr-mpi-base.o mpi_src/libmrmpi_mpicc.a -o mr-pr-mpi-base
