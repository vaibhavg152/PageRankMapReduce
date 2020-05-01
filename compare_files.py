import sys

file1 = sys.argv[1]
file2 = sys.argv[2]

lines1 = open(file1).readlines()
lines2 = open(file2).readlines()

assert len(lines1)==len(lines2), 'unequal number of lines. {} and {}'.format(len(lines1),len(lines2))

error = 0
for idx, line in enumerate(lines1[:-1]):
	node = float(line.split()[0])
	importance = float(line.split()[2])
	assert float(lines2[idx].split()[0]) == node, 'different node for line number {}. should be {}'.format(idx,node)
	error += abs(importance-float(lines2[idx].split()[2]))

print('check complete. error = ',error,'average error = ', error/(-1+len(lines1)))
