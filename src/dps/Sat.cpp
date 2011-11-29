
#include <sstream>

#include "Sat.h"

using namespace std;

ostream& operator<<(ostream& out, const Conjunct& c)
{
	stringstream ss;
	string sign(c.inverted()? "-" : "");
	ss << c.id();

	out << sign << ss.str();
	return out;
}

ostream& operator<<(ostream& out, const Conjunction& c)
{
	for(int i = 0; i < Conjunction::Size; ++i) {
		out << c[i];
		out << " ";
	}
	return out;
}

ostream& operator<<(ostream& out, const SatProblem& p)
{
	for(ConjunctionIter i = p.begin(); i != p.end(); ++i) {
		out << **i << endl;
	}
	return out;
}
