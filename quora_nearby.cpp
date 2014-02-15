#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <queue>
#include <set>

using namespace std;

class Topic
{
public:
	int tid;
	double data[2];
	Topic (int id, double xx, double yy) : tid(id) {
		data[0] = xx;
		data[1] = yy;
	}

	Topic () {}
};

class Entry
{
public:
	double dist;
	int id;
	Entry (double dd, int ii) : dist(dd), id(ii) {}

	bool operator<(const Entry &e) const {
		if (dist != e.dist) return dist < e.dist;
		return id > e.id;
	}
};


class TopicQueue
{
public:
	priority_queue<Entry> prq;
	int K;

	TopicQueue (int kk) : K (kk) {}

	void updateTopic (const Entry& e) {
		if (prq.size() < K) {
			prq.push(e);
		} else if (e < prq.top()) {
			prq.pop();
			prq.push(e);
		}
	}

	double radius () {
		assert (!prq.empty());
		return prq.top().dist;
	}


	void printTopic () {
		vector<Entry> vec;
		while (!prq.empty()) {
			vec.push_back(prq.top());
			prq.pop();
		}
		for (int i = vec.size()-1; i >= 0; --i)
		{	
			if (i == 0) cout << vec[i].id << endl;
			else cout << vec[i].id << " ";
		}
	}


	int needed () {
		return K - prq.size();
	}
};



class QuestionQueue {

private:
	void decreaseKey (const Entry& e) {
		for (int i = 0; i < vec.size(); ++i) {
			if (vec[i].id != e.id) continue;
			if (e.dist < vec[i].dist) {
				vec[i].dist = e.dist;
				make_heap(vec.begin(), vec.end());
			}
			break;
		}
	}

public:
	vector<Entry> vec;
	set<int> visited;
	int K;

	QuestionQueue (int kk) : K(kk) {}

	void updateQuestion (const Entry& e) {
		if (vec.size() < K) {
			if (visited.count(e.id) == 0) {
				vec.push_back(e);
				push_heap(vec.begin(), vec.end());
				visited.insert(e.id);
			}
			else decreaseKey (e);
		}
		else if (e < vec.front()) {
			if (visited.count(e.id) == 0) {
				int id = vec.front().id;
				pop_heap(vec.begin(), vec.end());
				vec.pop_back();
				visited.erase(id);

				vec.push_back(e);
				push_heap(vec.begin(), vec.end());
				visited.insert(e.id);
			}
			else decreaseKey (e);
		}
	}

	void printQuestion () {
		sort(vec.begin(), vec.end());
		for (int i = 0; i < vec.size(); ++i) {
			if (i == vec.size()-1)
				cout << vec[i].id << endl;
			else cout << vec[i].id << " ";
		}
	}

	double radius () {
		assert (!vec.empty());
		return vec.front().dist;
	}

	int needed () {
		return K - vec.size();
	}
};


int T, Q, N;
Topic allTopics[10050];
map<int, vector<int> > tqTable;


bool sortByX (const Topic& t1, const Topic& t2) {
	return t1.data[0] < t2.data[0];
}

bool sortByY (const Topic& t1, const Topic& t2) {
	return t1.data[1] < t2.data[1];
}


class KDNode
{
private:
	int splitByDim(const int& l, const int& u, const int& dim)
	{
		if (dim) sort(allTopics+l, allTopics+u+1, sortByY);
		else sort(allTopics+l, allTopics+u+1, sortByX);
		return l + (u-l) / 2;
	}

public:
	int depth;
	int low, high;

	double splitPoint;
	KDNode *left, *right;

	KDNode (int dd, int ll, int hh): depth (dd), low(ll), high(hh), 
		splitPoint(-1.0), left(NULL), right(NULL)
	{
		if (low >= high)
			return;

		int mid = splitByDim(low, high, depth%2);
		
		if (low == mid)
			return;

		this->splitPoint = allTopics[mid].data[depth%2];
		this->left = new KDNode(depth+1, low, mid);
		this->right = new KDNode(depth+1, mid+1, high);
	}
};


inline double dist(const double &x1, const double &y1, 
	const double &x2, const double &y2)
{
	return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}


void KNN (KDNode *index, const double &x, const double &y, TopicQueue &Q)
{	
	if (!index) return;

	/* Leaf node */
	if (index->splitPoint == -1) {
		for (int tt = index->low; tt <= index->high; ++tt) {
			double D = dist(x, y, allTopics[tt].data[0], allTopics[tt].data[1]);
			Entry e (D, allTopics[tt].tid);
			Q.updateTopic (e);
		}
		return;
	}

	int pivot = (index->depth % 2) ? y : x;
	KDNode *p1 = (pivot <= index->splitPoint) ? index->left : index->right;
	KDNode *p2 = (pivot <= index->splitPoint) ? index->right : index->left;

	/* Subtree */
	if ( !p1 || (p1->high - p1->low + 1) < Q.needed() ) {
		for (int tt = index->low; tt <= index->high; ++tt) {
			double D = dist(x, y, allTopics[tt].data[0], allTopics[tt].data[1]);
			Entry e (D, allTopics[tt].tid);
			Q.updateTopic (e);
		}
	}
	/* Recurse */
	else {
		KNN (p1, x, y, Q);
		if (abs(pivot-index->splitPoint) <= Q.radius() ) {
			KNN (p2, x, y, Q);
		}
	}
}


void KNNQuestion (KDNode *index, const double &x, const double &y, QuestionQueue &Q)
{
	if (!index) return;

	/* Leaf node */
	if (index->splitPoint == -1) {
		for (int tt = index->low; tt <= index->high; ++tt) {
			double D = dist(x, y, allTopics[tt].data[0], allTopics[tt].data[1]);
			vector<int> qid = tqTable[allTopics[tt].tid];
			for (int qq = 0; qq < qid.size(); ++qq)
			{
				Entry ques (D, qid[qq]);
				Q.updateQuestion (ques);
			}
		}
		return;
	}

	int pivot = (index->depth % 2) ? y : x;
	KDNode *p1 = (pivot <= index->splitPoint) ? index->left : index->right;
	KDNode *p2 = (pivot <= index->splitPoint) ? index->right : index->left;

	KNNQuestion (p1, x, y, Q);
	
	/* Recurse back */
	if (Q.needed() != 0 || abs(pivot-index->splitPoint) <= Q.radius()) {
		KNNQuestion (p2, x, y, Q);
	}
}



int main ()
{
	cin >> T >> Q >> N;
	
	// Read topics
	int tid;
	double xx, yy;
	for (int t = 0; t < T; ++t)
	{
		cin >> tid >> xx >> yy;
		Topic tpc (tid, xx, yy);
		allTopics[t] = tpc; 
	}

	// Build index
	KDNode *root = new KDNode(0, 0, T-1);

	// Read questions
	int qid, Qn;
	for (int q = 0; q < Q; ++q)
	{	
		cin >> qid >> Qn;
		for (int t = 0; t < Qn; ++t)
		{
			cin >> tid;
			tqTable[tid].push_back(qid);
		}
	}

	// Read queries
	char ch;
	double qx, qy;
	int topk;

	for (int n = 0; n < N; ++n)
	{
		cin >> ch >> topk >> qx >> qy;
		if (ch == 't') {
			TopicQueue Q (topk);
			KNN(root, qx, qy, Q);
			Q.printTopic ();
		}
		else {
			QuestionQueue Q (topk);
			KNNQuestion(root, qx, qy, Q);
			Q.printQuestion ();
		}
	}
	
	return 0;
}
