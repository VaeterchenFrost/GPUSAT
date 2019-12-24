#include <graphoutput.h>
#include <iostream>
#include <fstream>
#include <types.h>

namespace gpusat {

	/// <summary>
	/// Create one edge between two nodes.
	/// </summary>
	/// <param name="source">The source id.</param>
	/// <param name="target">The target id.</param>
	void Graphoutput::graphEdge(unsigned int source, unsigned int target)
	{
		graphout(
			"edge\n[\n"
			"source " + std::to_string(source) + "\n"
			"target " + std::to_string(target) + "\n"
			"]\n"
		);
	}

	/// <summary>
	/// Creates a node with one label.
	/// </summary>
	/// <param name="id">The identifier.</param>
	/// <param name="label">The label.</param>
	void Graphoutput::graphNode(unsigned int id, std::string label)
	{
		graphout(
			"node\n"
			"[\n id " + std::to_string(id) + "\n"
			"label \"" + label + "\"\n"
			"LabelGraphics\n[ text \""
			+ label +
			" \"]\n"
			"]\n"
		);
	}

	/// <summary>
	/// Creates a node with one label and a solution label.
	/// </summary>
	/// <param name="id">The identifier.</param>
	/// <param name="label">The corresponding label.</param>
	/// <param name="solution">The solution to create a label.</param>
	void Graphoutput::graphNode(unsigned int id, std::string label, std::string solution)
	{
		graphout(
			"node\n"
			"[\n id " + std::to_string(id) + "\n"
			"label \"" + label + "\"\n"
			"LabelGraphics\n[ text \""
			+ label +
			" \"]\n"
			"LabelGraphics\n[ text \""
			+ solution +
			" \"]\n"
			"]\n"
		);
	}

	/// <summary>
	/// Create one node with the corresponding solution in a connected extra bag.
	/// </summary>
	/// <param name="id">The identifier of the bag.</param>
	/// <param name="solution">The solution in string form.</param>
	void Graphoutput::nodeBag(unsigned int id, std::string solution)
	{
		std::string label = "bag " + std::to_string(id);
		graphNode(id, label);
		graphNode(++countSol, label, solution);
		graphEdge(id, countSol);
	}


	/// <summary>
	/// Creates a node with the solution that joins two bags.
	/// Two edges are added to connect the solution.
	/// </summary>
	/// <param name="id1">The id1.</param>
	/// <param name="id2">The id2.</param>
	/// <param name="solution">The solution.</param>
	/// TODO Edit XML Comment Template for nodeJoin
	void Graphoutput::nodeJoin(unsigned int id1, unsigned int id2, std::string solution)
	{
		graphNode(++countJoin, "Join " + std::to_string(countJoin - baseIdJoin), solution);
		graphEdge(id1, countJoin);
		graphEdge(id2, countJoin);
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

	void Graphoutput::setFile(std::string filename)
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