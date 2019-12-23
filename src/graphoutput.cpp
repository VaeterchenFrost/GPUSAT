#include <graphoutput.h>
#include <iostream>
#include <fstream>
#include <types.h>

namespace gpusat {

	void Graphoutput::graphEdge(unsigned int source, unsigned int target)
	{
		graphout(
			"edge\n[\n"
			"source " + std::to_string(source) + "\n"
			"target " + std::to_string(target) + "\n"
			"]\n"
		);
	}

	void Graphoutput::graphNode(unsigned int id, std::string label, std::string solution)
	{
		graphout(
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

	void Graphoutput::graphEdgeSet(treedecType* dec)
	{
		for (auto b : dec->bags) {
			for (auto e : b.edges) {
				graphEdge(e->id, b.id);
			}
		}
	}

	void Graphoutput::graphStart()
	{
		Graphoutput::graphout("graph\n[\n", false);
	}

	void Graphoutput::graphEnd()
	{
		Graphoutput::graphout("\n]\n");
	}

	void Graphoutput::graphout(std::string string, bool append)
	{
		if (!isEnabled()) return;

		std::ofstream stream(graphfile, append ? std::ios_base::app : std::ios_base::out);

		if (stream.is_open()) {
			stream << string;
			stream.close();
		}
		else { std::cerr << "Failed to open file : " << graphfile << " with " << errno << std::endl; }
	}

	bool Graphoutput::setFile(std::string filename)
	{
		if (filename != "") {
			outputEnabled = true;
		}
		else outputEnabled = false;
		graphfile = filename;
	}

	// currently deprecated
	/*void decompGraph(std::string filename, treedecType& decomp) {
		std::ofstream stream(filename);
		if (stream.is_open()) {
			stream << "graph\n[\n";
			/// nodes
			for (bagType b : decomp.bags) {
				stream << "node\n[\n" << " id " << b.id << "\n";
				stream << " label \"bag " << b.id << " var: " << b.variables << "\"\n]\n";
			}
			/// edges
			for (auto b : decomp.bags) {
				for (auto e : b.edges) {
					stream << "edge\n[\n source " << b.id << "\n"
						<< " target " << e->id << "\n]\n";
				}
			}
			stream << "]";
			stream.close();
		}
		else { std::cerr << "Failed to open file : " << filename << " with " << errno << std::endl; }
	}*/

}