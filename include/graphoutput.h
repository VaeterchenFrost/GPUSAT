#ifndef GPUSAT_GRAPHOUTPUT_H
#define GPUSAT_GRAPHOUTPUT_H


#include <string>
#include <types.h>
#include <map>
#include <utility>

namespace gpusat {

	class Graphoutput {

	public:

		static const int baseIdSol = 1'000'000;
		static const int baseIdJoin = 4 * baseIdSol; // avoid factor 2 that would collide with adding two ids.

		Graphoutput(std::string filename) {
			setFile(filename);
		}

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
		std::map<int, int> joinmap;
		unsigned int countJoin = baseIdJoin;
		unsigned int countSol = baseIdSol;
		std::string graphfile;

		void graphEdge(unsigned int source, unsigned int target);
		void graphNode(unsigned int id, std::string label);
		void graphNode(unsigned int id, std::string label, std::string solution);

		void graphout(std::string string, bool append = true);
		void setFile(std::string filename);
	};
}

#endif // !GPUSAT_GRAPHOUTPUT_H
