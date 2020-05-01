import matplotlib.pyplot as plt
import json

timings = json.load(open('timings-all.json'))

for part, vals in timings.items():
	x = sorted([f.split('/')[1] for f in vals.keys()])
	y = [float(vals['test/'+f]) for f in x]
	plt.scatter(x,y)
	plt.plot(x,y)
	plt.xlabel('filenames')
	plt.ylabel('time taken (in seconds)')
	plt.title('number of processes = 4')
	plt.savefig('graph_{}.svg'.format(part))
	plt.show()
#plt.legend(list(timings.keys()))
#plt.savefig('graph_all.svg')
#plt.show()
