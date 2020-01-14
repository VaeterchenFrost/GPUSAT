#ifndef GPUSAT_GRAPHOUTPUT_H
#define GPUSAT_GRAPHOUTPUT_H


#include <string>
#include <types.h>
#include <map>

namespace gpusat {

	class Graphoutput {

	public:
		static const std::string dualedge;
		static const std::string incidenceedge;
		static const std::string primaledge;

		static const int baseIdSol = 1'000'000;
		static const int baseIdJoin = 4 * baseIdSol; // avoid factor 2 that would collide with adding two ids.

		Graphoutput(std::string filename) {
			setFile(filename);
		}

		void nodeBag(unsigned int id, std::string solution);
		void nodeJoin(unsigned int id1, unsigned int id2, std::string solution);

		void graphEdgeSet(treedecType* dec);
		void graphStart(treedecType* dec);
		void graphEnd();

		std::string getFilename() {
			return graphfile;
		}

		bool isEnabled() {
			return outputEnabled;
		}

		void neo4jSat(satformulaType* satFormula);
		void neo4jTD(treedecType* treeDec);

	private:
		bool outputEnabled = false;
		std::map<int, int> joinmap;
		std::map<int, std::vector<cl_long>> variablesmap;

		unsigned int countJoin = baseIdJoin;
		unsigned int countSol = baseIdSol;
		std::string graphfile;
		std::string satFile = "cypherSatFormula.txt";
		std::string tdFile = "cypherTreedec.txt";

		void graphEdge(unsigned int source, unsigned int target);
		void graphNode(unsigned int id, std::string label);
		void graphSolutionNode(unsigned int id, std::string label, std::string solution);

		void graphout(std::string string, bool append = true);
		void setFile(std::string filename);
	};
}

#endif // !GPUSAT_GRAPHOUTPUT_H
