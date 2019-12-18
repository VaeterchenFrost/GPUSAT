#include "..\include\graphoutput.h"
#include <iostream>
#include <fstream>

namespace gpusat {

	inline void Graphoutput::graphEdge(std::string filename, unsigned int source, unsigned int target)
	{
		graphout(filename,
			"edge\n[\n"
			"source " + std::to_string(source) + "\n"
			"target " + std::to_string(target) + "\n"
			"]\n"
		);
	}

	inline void Graphoutput::graphNode(std::string filename, unsigned int id, std::string label, std::string solution)
	{
		graphout(filename,
			"node\n"
			"[\n id " + std::to_string(id) + "\n"
			"label \"" + label + "\"\n"
			"LabelGraphics\n[ text \""
			+ label +
			"\"\n fontSize 12\n"
			" fontName \"Dialog\"\n"
			" model \"null\"\n"
			" ]\n"
			"LabelGraphics\n[ text \""
			+ solution +
			"\"\n fontSize 12\n"
			" fontName \"Dialog\"\n"
			" model \"null\"\n"
			" ]\n"
			"]\n"
		);
	}

	inline void Graphoutput::graphEdgeSet(std::string filename, treedecType* dec)
	{
		for (auto b : dec->bags) {
			for (auto e : b.edges) {
				graphEdge(filename, e->id, b.id);
			}
		}
	}

	inline void Graphoutput::graphout(std::string filename, std::string string)
	{
		std::ofstream stream(filename, std::ios_base::app);
		if (stream.is_open()) {
			stream << string;
			stream.close();
		}
		else { std::cerr << "Failed to open file : " << filename << " with " << errno << std::endl; }
	}
}