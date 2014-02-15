import time
import os
import random
from subprocess import Popen, PIPE

topic_lst = []
quest_lst = []
query_lst = []


def generate_data (inputfile, T, Q, N):
	f = file(inputfile, 'wb');
	f.write(' '.join([str(T), str(Q), str(N)]) + '\n')
	LOWER, UPPER = 0.0, 1000000.0
	# Topic
	for i in range(T):
		(x, y) = (round(random.uniform(LOWER, UPPER), 2), \
			round(random.uniform(LOWER, UPPER), 2))
		f.write(' '.join([str(i), str(x), str(y)]) + '\n')

	# Question
	for i in range(Q):
		relatetopic = range(T)
		random.shuffle(relatetopic)
		f.write(str(i) + ' 10 ' + ' '.join([ str(u) for u in relatetopic[:10] ]) + '\n');

	# Query
	for i in range(N):
		(qx, qy) = (round(random.uniform(LOWER, UPPER), 2), \
			round(random.uniform(LOWER, UPPER), 2))
		tp = 't' if i % 2 == 0 else 'q'  
		f.write(tp + ' 100 ' + ' '.join([str(qx), str(qy)]) + '\n')
	f.close()

def read_data (inputfile):
	f = file(inputfile, 'r')
	lines = f.readlines()
	f.close()

	T, Q, N = [ int(u) for u in lines[0].strip().split() ]
	# Topic
	for t in range(T):
		item = lines[1+t].strip().split()
		topic_lst.append([ int(item[0]), float(item[1]), float(item[2]) ])

	# Question 
	for q in range(Q):
		item = lines[1+T+q].strip().split()
		quest_lst.append([ int(item[0]) ] + [ int(item[i+2]) for i in range(int(item[1])) ])

	# Query
	for n in range(N):
		item = lines[1+T+Q+n].strip().split()
		query_lst.append([ str(item[0]), int(item[1]), float(item[2]), float(item[3]) ])


def dist (x1, y1, x2, y2):
	return ((x1-x2) ** 2) + ((y1-y2) ** 2)


def nearby_topic (query):
	K, qx, qy = query
	val = []
	for topic in topic_lst:
		val.append((topic[0], dist(qx, qy, topic[1], topic[2])))
	val = sorted(val, key=lambda x:(x[1], -x[0]))
	return ' '.join([ str(u[0]) for u in val[:K] ])


def nearby_question (query):
	K, qx, qy = query
	dval = {}
	for topic in topic_lst:
		dval[topic[0]] = dist(qx, qy, topic[1], topic[2])

	val = []
	for question in quest_lst:
		val.append((question[0], min([ dval[u] for u in question[1:] ])))
	val = sorted(val, key=lambda x:(x[1], -x[0]))
	return ' '.join([ str(u[0]) for u in val[:K] ])


def solve_prob ():
	result = []
	for q in query_lst:
		tp, query = q[0], q[1:]
		if q[0] == 't':
			result.append( nearby_topic( query ) )
		else:
			result.append( nearby_question( query ) )
	return '\n'.join(result)


def compare_result (sol, res):
	sol = sol.split('\n')
	res = res.split('\n')
	assert (len(sol) == len(res))
	for i in range(len(sol)):
		if sol[i] != res[i]:
			print sol[i]
			print res[i]
			print "Fail"
			return
	print "Pass"


if __name__ == "__main__":
	tmpFileName = "tmp.data"
	generate_data (tmpFileName, 10000, 1000, 100)
	
	start = time.time()
	read_data (tmpFileName)
	sol = solve_prob().strip()
	end = time.time()
	print end - start

	start = time.time()
	process = Popen(os.getcwd() + os.sep + 'a.out < ' + tmpFileName, shell=True, stdout=PIPE)
	(stdout, stderr) = process.communicate()
	ans = stdout.strip()
	end = time.time()
	print end - start

	compare_result (sol, ans)
	
	os.remove(tmpFileName)



