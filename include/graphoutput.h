#ifndef GPUSAT_GRAPHOUTPUT_H
#define GPUSAT_GRAPHOUTPUT_H

// INCLUDES
#include <string>
#include <types.h>

namespace gpusat {

	class Graphoutput {
	public:
		inline static void graphEdge(std::string filename, unsigned int source, unsigned int target);
		inline static void graphNode(std::string filename, unsigned int id, std::string label, std::string solution);
		inline static void graphEdgeSet(std::string filename, treedecType* dec);
	private:
		inline static void graphout(std::string filename, std::string string);

	};
}

#endif // !GPUSAT_GRAPHOUTPUT_H
