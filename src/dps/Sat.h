
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

		Conjunct& at(int idx) { return m_conjuncts[idx]; }
		const Conjunct& at(int idx) const { return m_conjuncts[idx]; }

	private :
		Conjunct m_conjuncts[Size];
};

typedef vector<Conjunction const*>::const_iterator ConjunctionIter;

struct Backtrack {
	int m_varId;
	vector<Conjunction const*> m_clauses;
	bool m_backtracked;

	ConjunctionIter begin() const { return m_clauses.begin(); }
	ConjunctionIter end() const { return m_clauses.end(); }
};

enum ConjunctVal {
	True,
	False,
	Unset
};

class SatProblem {
	public :
		SatProblem(int numVars, int numClauses);
		~SatProblem();

		void addConjunction(Conjunction const* c) { m_conjunctions.push_back(c); }

		bool solve(vector<Conjunct>&);

		ConjunctionIter begin() const { return m_conjunctions.begin(); }
		ConjunctionIter end() const { return m_conjunctions.end(); }

	private :
		const Conjunct& pick_var() const;
		bool simplify();
		bool backtrack();

		int m_numVars;
		int m_numClauses;

		int m_index;
		ConjunctVal *m_values;
		Backtrack *m_backtracks;

		vector<Conjunction const*> m_conjunctions;
};

ostream& operator<<(ostream&, const Conjunct&);
ostream& operator<<(ostream&, const Conjunction&);
ostream& operator<<(ostream&, const SatProblem&);

#endif // _SAT_H_

