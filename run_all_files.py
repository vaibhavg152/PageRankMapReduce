import glob, json, os
# import matplotlib.pyplot as plt

num_processors = 4

# timings = {'pr-cpp': {},\
# 'pr-mpi' : {},\
# 'pr-mpi-base' : {},\
# }
timings = json.load(open('timings_mr-pr-mpi-base.json'))

filenames = open('test/all-tests.txt').readlines()[1:-1]


print('running part b')
compile_command = 'mpic++ mr-pr-mpi.cpp -o mr-pr-mpi.o'

os.system(compile_command)
for filename in filenames:
	filename = 'test/' + filename.strip()
	print(filename)
	run_command = './mr-pr-mpi.o {}.txt {} -o {}-pr-cpp.txt'.format(filename,num_processors,filename)
	stream = os.popen(run_command)
	output = stream.read().split()
	timings['pr-mpi'][filename.strip()] = (float(output[0]))


print('running part a')
compile_command = 'g++ mr-pr-cpp.cpp /usr/lib/x86_64-linux-gnu/libboost_system.a /usr/lib/x86_64-linux-gnu/libboost_iostreams.a /usr/lib/x86_64-linux-gnu/libboost_filesystem.a -pthread -o mr-pr-cpp.o -I src'

os.system(compile_command)
for filename in filenames:
	filename = 'test/' + filename.strip()
	print(filename)
	run_command = './mr-pr-cpp.o {}.txt {} -o {}-pr-cpp.txt'.format(filename,num_processors,filename)
	stream = os.popen(run_command)
	output = stream.read().split()
	timings['pr-cpp'][filename.strip()] = (float(output[0]))


#print('running part c')
#compile_command = 'mpic++ -c mr-pr-mpi-base.cpp -o mr-pr-mpi-base.o -I src\nmpic++ -g -O mr-pr-mpi-base.o src/libmrmpi_mpicc.a -o mr-pr-mpi-base'
#
#os.system(compile_command)
#for filename in reversed(filenames):
#	# if 'uniquely3colo' in filename:
#	# 	print('skipping',filename)
#	# 	continue
#	print(filename)
#	filename = 'test/' + filename.strip()
#	run_command = 'mpirun -np {} mr-pr-mpi-base {}.txt'.format(num_processors,filename)
#	stream = os.popen(run_command)
#	# print(stream)
#	output = stream.read().split()
#	timings['pr-mpi-base'][filename.strip()] = (float(output[0]))

json.dump(timings,open('timings-all.json','w+'))
