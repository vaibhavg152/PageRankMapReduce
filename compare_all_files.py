import sys, glob
from pathlib import Path

filenames = glob.glob('test/*-base.txt')

for filename in filenames:
	file1 = filename.split('-mpi')[0] + '-j.txt'

	for types in ['-cpp','-mpi','-mpi-base']:
		file2 = filename.split('-mpi')[0] + types + '.txt'
		print(file2)

		lines1 = open(file1).readlines()
		lines2 = open(file2).readlines()

		assert len(lines1)==len(lines2), 'unequal number of lines. {} and {}'.format(len(lines1),len(lines2))

		error = 0
		for idx, line in enumerate(lines1[:-1]):
			node = float(line.split()[0])
			importance = float(line.split()[2])
			assert float(lines2[idx].split()[0]) == node, 'different node for line number {}. should be {}'.format(idx,node)
			error += abs(importance-float(lines2[idx].split()[2]))
		avg_error = error/(-1+len(lines1))
		if avg_error > 0.001:
			print('check complete. error = ',error,'average error = ', avg_error)
			print('some problem here')
