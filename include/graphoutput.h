#ifndef GPUSAT_GRAPHOUTPUT_H
#define GPUSAT_GRAPHOUTPUT_H

// INCLUDES
#include <string>
#include <types.h>

namespace gpusat {


	class Graphoutput {

	public:
		Graphoutput(std::string filename) { setFile(filename); }

		inline void graphEdge(unsigned int source, unsigned int target);
		inline void graphNode(unsigned int id, std::string label, std::string solution);
		inline void graphEdgeSet(treedecType* dec);
		inline void graphStart();
		inline void graphEnd();
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
