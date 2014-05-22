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

	Topic (int tt, double xx, double yy) : tid(tt) {
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
		if (abs(dist - e.dist) >= 0.001) return dist < e.dist;
		return id > e.id;
	}
};


class TopicQueue
{
public:
	priority_queue<Entry> q;
	int K;

	TopicQueue (int kk) : K (kk) {}

	void updateTopic (const Entry& e) {
		if (q.size() < K) {
			q.push(e);
		} else if (e < q.top()) {
			q.pop();
			q.push(e);
		}
	}

	double radius () {
		assert (!q.empty());
		return q.top().dist;
	}

	int needed () {
		return K - q.size();
	}

	void printTopic () {
		rprint ();
		cout << endl;
	}

private:
	void rprint () {
		if (q.empty()) return;
		int val = q.top().id;
		q.pop();
		if (q.size() >= 1) {
			rprint ();
			cout << " " << val;
		}
		else cout << val;
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
			else  cout << vec[i].id << " ";
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
Topic arr[10050];
map<int, vector<int> > tqTable;


class KDNode
{
private:
	int partition(int left, int right, int dim)
	{
		int l = left, r = right;
		double vp = arr[l + rand() % (r - l + 1)].data[dim];
		do {
			while (arr[l].data[dim] < vp) l++;
			while (arr[r].data[dim] > vp) r--;
			if (l <= r) {
				Topic tmp = arr[r];
				arr[r] = arr[l];
				arr[l] = tmp;
				l++;
				r--;
			}
		} while (l <= r);
		return r;
	}

public:
	int depth;
	int low, high;
	double splitPoint;
	KDNode *left, *right;

	KDNode (int dd, int ll, int hh): depth (dd), low(ll), high(hh), splitPoint(-1.0), left(NULL), right(NULL)
	{
		if (low >= high)
			return;

		int mid = partition(low, high, depth % 2);
		this->splitPoint = arr[mid].data[depth % 2];
		this->left = new KDNode(depth+1, low, mid);
		this->right = new KDNode(depth+1, mid+1, high);
	}

	~KDNode ()
	{
		delete left;
		delete right;
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
	if (index->splitPoint < 0) {
		for (int tt = index->low; tt <= index->high; ++tt) {
			double D = dist(x, y, arr[tt].data[0], arr[tt].data[1]);
			Entry e (D, arr[tt].tid);
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
			double D = dist(x, y, arr[tt].data[0], arr[tt].data[1]);
			Entry e (D, arr[tt].tid);
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
	if (index->splitPoint < 0) {
		for (int tt = index->low; tt <= index->high; ++tt) {
			double D = dist(x, y, arr[tt].data[0], arr[tt].data[1]);
			vector<int> qid = tqTable[arr[tt].tid];
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
	
	int tid, qid, qn;
	double xx, yy;

	// Read topics
	for (int t = 0; t < T; ++t)
	{
		cin >> tid >> xx >> yy;
		Topic tpc (tid, xx, yy);
		arr[t] = tpc;
	}

	// Build index
	KDNode *root = new KDNode(0, 0, T-1);

	// Read questions
	for (int q = 0; q < Q; ++q)
	{	
		cin >> qid >> qn;
		for (int t = 0; t < qn; ++t)
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

	delete root;
	
	return 0;
}
