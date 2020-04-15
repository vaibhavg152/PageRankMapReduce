import glob, json, os
# import matplotlib.pyplot as plt

num_processors = 4

timings = {'pr-cpp': {},\
'pr-mpi' : {},\
'pr-mpi-base' : {},\
}

filenames = open('test/all-tests.txt').readlines()[1:-1]

print('running part a')
compile_command = 'g++ parallel_a.cpp /usr/lib/x86_64-linux-gnu/libboost_system.a /usr/lib/x86_64-linux-gnu/libboost_iostreams.a /usr/lib/x86_64-linux-gnu/libboost_filesystem.a -pthread -o mr-pr-cpp.o'

os.system(compile_command)
for filename in filenames:
	filename = 'test/' + filename.strip()
	print(filename)
	run_command = './mr-pr-cpp.o {}.txt {} -o {}-pr-cpp.txt'.format(filename,num_processors,filename)
	stream = os.popen(run_command)
	output = stream.read().split()
	timings['pr-cpp'][filename.strip()] = (float(output[0]))


print('running part b')
compile_command = ''

os.system(compile_command)
for filename in filenames:
	filename = 'test/' + filename.strip()
	run_command = ''
	stream = os.popen(run_command)
	output = stream.read().split()
	timings['pr-mpi'][filename.strip()] = (float(output[0]))


print('running part c')
compile_command = ''

os.system(compile_command)
for filename in filenames:
	filename = 'test/' + filename.strip()
	run_command = ''
	stream = os.popen(run_command)
	output = stream.read().split()
	timings['pr-mpi-base'][filename.strip()] = (float(output[0]))

#json.dump(timings,open('timings.json','w+'))
