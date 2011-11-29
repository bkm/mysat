
#ifndef _DIMACS_H_
#define _DIMACS_H_

#include <fstream>

class SatProblem;
class Conjunction;
class Conjunct;

using std::ifstream;

class DimacsFile
{
	public :
		DimacsFile(char const* fileName):
			m_fileName(fileName),
			m_inputStream(fileName) { }

		void read();

		SatProblem const* getProblem() const { return m_satProblem; }

	private :

		Conjunction const* readConjunction();

		SatProblem* m_satProblem;

		char const *m_fileName;
		ifstream m_inputStream;
};

#endif // _DIMACS_H_

