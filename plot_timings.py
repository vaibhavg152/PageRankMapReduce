import matplotlib.pyplot as plt
import json

timings = json.load(open('timings-all.json'))

for part, vals in timings.items():
	x = sorted([f.split('/')[1] for f in vals.keys()])
	y = [float(vals['test/'+f]) for f in x]
	plt.scatter(x,y)
	plt.plot(x,y)
plt.legend(list(timings.keys()))
plt.xlabel('filenames')
plt.ylabel('time taken (in seconds)')
plt.savefig('graph-all.svg')
plt.title('number of processes = 4')
plt.show()
