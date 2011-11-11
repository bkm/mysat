#ifndef SAT_H
#define SAT_H

#include <vector>
#include <string>
#include <ostream>
#include <stack>

#include <iostream>

using namespace std;

using std::vector;
using std::string;
using std::ostream;
using std::stack;

namespace Sat {

class Var {

	friend ostream& operator<<(ostream&, const Var&);

	public :
		Var():
			m_id(0), m_inverted(false) { }

		Var(int id, bool inverted):
			m_id(id),
			m_inverted(inverted) { }

		bool inverted() const { return m_inverted; }
		int id() const { return m_id; }

	private :
		int m_id;
		bool m_inverted;
};

typedef vector<Var>::const_iterator VarIter;

class Clause {
	public :
		~Clause()
		{
			cout << "woops" << endl; }
		Clause():
			m_conjuncts(vector<Var>()) { }

		void addVar(int id, bool inverted)
		{
			m_conjuncts.push_back(Var(id, inverted));
		}

		const Var& operator[](int index)
		{
			return m_conjuncts[index];
		}

		const Var& first() const
		{
			return m_conjuncts.at(0);
		}

		VarIter begin() const
		{
			return m_conjuncts.begin();
		}

		VarIter end() const
		{
			return m_conjuncts.end();
		}

	private :
		vector<Var> m_conjuncts;
};

typedef vector<Clause const*>::const_iterator ClausePtrIter;

struct Backtrack {
	int m_varId;
	vector<Clause const*> m_clauses;
	bool m_backtracked;

	ClausePtrIter begin() const { return m_clauses.begin(); }
	ClausePtrIter end() const { return m_clauses.end(); }
};

enum VarValue {
	True,
	False,
	Unset
};

class Sat {
	public :
		Sat():
			m_index(0) { }


		bool solve(vector<Var>&);
		void read(const char*);

		ClausePtrIter begin() const
		{
			return m_clauses.begin();
		}

		ClausePtrIter end() const
		{
			return m_clauses.end();
		}

	private :

		const Var& pick_var() const;
		bool simplify();
		bool backtrack();

		int m_nVars;
		int m_nClauses;

		Backtrack *m_backtracks;
		int m_index;

		VarValue *m_vars;

		vector<Clause const*> m_clauses;
};

ostream& operator<<(ostream&, const Sat&);
ostream& operator<<(ostream&, const Clause&);
ostream& operator<<(ostream&, const Var&);

}

#endif
