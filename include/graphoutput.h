#ifndef GPUSAT_GRAPHOUTPUT_H
#define GPUSAT_GRAPHOUTPUT_H

// INCLUDES
#include <string>
#include <types.h>

namespace gpusat {

	class Graphoutput {

	public:
		Graphoutput(std::string filename) { setFile(filename); }

		void graphEdge(unsigned int source, unsigned int target);
		void graphNode(unsigned int id, std::string label, std::string solution);
		void graphEdgeSet(treedecType* dec);
		void graphStart();
		void graphEnd();
		std::string getFilename() {
			return graphfile;
		}

		bool isEnabled() { return outputEnabled; }
		
	private:
		bool outputEnabled = false;
		std::string graphfile;

		void graphout(std::string string, bool append = true);
		bool setFile(std::string filename);
	};
}

#endif // !GPUSAT_GRAPHOUTPUT_H
