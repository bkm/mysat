#include <iostream>

#include "Sat.h"

using namespace std;
using namespace Sat;

int main(int argc, char* argv[])
{
	if(argc != 2) { 
		cerr << "Need a file to solve" << endl;
		return 0;
	}
	char const* file = argv[1];
	Sat::Sat *s = new Sat::Sat;
	s->read(file);

	vector<Var> v;
	bool sat = s->solve(v);
	if(!sat) {
		cout << "Not sat" << endl;
		return 0;
	}

	cout << "Sat" << endl;
	cout << "Solution : " << endl;
	for(int i=0; i<v.size(); ++i) {
		cout << v[i] << "  ";
	}
	cout << endl;

	return 0;
}
