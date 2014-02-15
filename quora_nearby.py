

topic_lst = []
quest_lst = []
query_lst = []


def read_data ():
	T, Q, N = [ int(u) for u in raw_input().strip().split() ]
	# Topic
	for t in range(T):
		item = raw_input().strip().split()
		topic_lst.append([ int(item[0]), float(item[1]), float(item[2]) ])

	# Question 
	for q in range(Q):
		item = raw_input().strip().split()
		quest_lst.append([ int(item[0]) ] + [ int(item[i+2]) for i in range(int(item[1])) ])

	# Query
	for n in range(N):
		item = raw_input().strip().split()
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
		if len(question) == 1:
			continue
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
	print '\n'.join(result)


if __name__ == "__main__":
	read_data ()
	solve_prob ()