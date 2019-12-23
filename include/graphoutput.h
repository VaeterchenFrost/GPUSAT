#ifndef GPUSAT_GRAPHOUTPUT_H
#define GPUSAT_GRAPHOUTPUT_H


#include <string>
#include <types.h>

namespace gpusat {

	class Graphoutput {

	public:

		static const int solIdBase = 1'000'000;
		static const int joinIdBase = 4 * solIdBase; // avoid factor 2 that would collide with adding two ids.

		Graphoutput(std::string filename) {
			setFile(filename);
		}

		void graphEdge(unsigned int source, unsigned int target);
		void graphNode(unsigned int id, std::string label);
		void graphNode(unsigned int id, std::string label, std::string solution);

		void nodeBag(unsigned int id, std::string solution);
		void nodeJoin(unsigned int id1, unsigned int id2, std::string solution);

		void graphEdgeSet(treedecType* dec);
		void graphStart();
		void graphEnd();

		std::string getFilename() {
			return graphfile;
		}

		bool isEnabled() {
			return outputEnabled;
		}

	private:
		bool outputEnabled = false;

		unsigned int countJoin = solIdBase;
		unsigned int countSol = joinIdBase;
		std::string graphfile;

		void graphout(std::string string, bool append = true);
		bool setFile(std::string filename);
	};
}

#endif // !GPUSAT_GRAPHOUTPUT_H
