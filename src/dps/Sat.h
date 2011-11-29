
#ifndef _SAT_H_
#define _SAT_H_

#include <vector>
#include <ostream>

typedef int VarId;

using std::vector;
using std::ostream;

class Conjunct {
	public :
		Conjunct(): m_varId(0) { }
		Conjunct(VarId id, bool isInverted):
			m_varId(id),
			m_isInverted(isInverted) { }
		Conjunct(int val):
			m_varId(val < 0? -val : val),
			m_isInverted(val < 0) { }

		bool inverted() const { return m_isInverted; }
		VarId id() const { return m_varId; }

	private :
		VarId m_varId;
		bool m_isInverted;
};

class Conjunction {
	public :
		enum Size {
			Size = 3
		};

		Conjunction() { }

		Conjunct& operator[](int idx) { return m_conjuncts[idx]; }
		const Conjunct& operator[](int idx) const { return m_conjuncts[idx]; }

	private :
		Conjunct m_conjuncts[Size];
};

typedef vector<Conjunction const*>::const_iterator ConjunctionIter;

class SatProblem {
	public :
		SatProblem(int numVars, int numClauses):
			m_numVars(numVars),
			m_numClauses(numClauses),
			m_conjunctions(vector<Conjunction const*>()) { }

		void addConjunction(Conjunction const* c) { m_conjunctions.push_back(c); }

		ConjunctionIter begin() const { return m_conjunctions.begin(); }
		ConjunctionIter end() const { return m_conjunctions.end(); }

	private :
		int m_numVars;
		int m_numClauses;

		vector<Conjunction const*> m_conjunctions;
};

ostream& operator<<(ostream&, const Conjunct&);
ostream& operator<<(ostream&, const Conjunction&);
ostream& operator<<(ostream&, const SatProblem&);

#endif // _SAT_H_

