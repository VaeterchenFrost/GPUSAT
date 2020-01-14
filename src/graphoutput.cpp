#include <graphoutput.h>
#include <iostream>
#include <fstream>
#include <types.h>
#include <sstream>
#include <gpusatutils.h>

namespace gpusat {

	/// <summary>
	/// Write out one edge between two nodes.
	/// </summary>
	/// <param name="source">The source id.</param>
	/// <param name="target">The target id.</param>
	void Graphoutput::graphEdge(unsigned int source, unsigned int target)
	{
		if (!isEnabled()) return;
		graphout(
			"edge\n[\n"
			"source " + std::to_string(source) + "\n"
			"target " + std::to_string(target) + "\n"
			"]\n"
		);
	}

	/// <summary>
	/// Writes out a node with one label.
	/// </summary>
	/// <param name="id">The identifier.</param>
	/// <param name="label">The label.</param>
	void Graphoutput::graphNode(unsigned int id, std::string label)
	{
		if (!isEnabled()) return;
		// find the original variables for this bag:
		std::ostringstream variables;
		auto search = variablesmap.find(id);
		if (search != variablesmap.end()) {
			std::vector<cl_long> vars = search->second;
			variables << "[";
			for (cl_long var : vars)
			{
				variables << " " << var;
			}
			variables << " ]";
		}
		else {
			std::cerr << "No value for key " << id << "in variablesmap!\n";
		}
		graphout(
			"node\n"
			"[\n id " + std::to_string(id) + "\n"
			"label \"" + label + "\"\n"
			"graphics\n[\n"
			"x 0\n"
			"y 0\n"
			"w 15\n"
			"h 15\n"
			"type \"ellipse\"\n]\n"
			"LabelGraphics\n[ text \""
			+ label +
			"\"\n"
			"anchor \"w\"\n"
			"]\n"
			"LabelGraphics\n[ text \""
			+ variables.str() +
			"\"\n"
			"anchor \"e\"\n"
			"]\n"
			"]\n"
		);
	}

	/// <summary>
	/// Writes out a node with one label and a solution label.
	/// </summary>
	/// <param name="id">The identifier.</param>
	/// <param name="label">The corresponding label.</param>
	/// <param name="solution">The solution to create a label.</param>
	void Graphoutput::graphSolutionNode(unsigned int id, std::string label, std::string solution)
	{
		if (!isEnabled()) return;
		graphout(
			"node\n"
			"[\n id " + std::to_string(id) + "\n"
			"label \"" + label + "\"\n"
			"graphics\n[\n"
			"x 0\n"
			"y 0\n"
			"type \"star8\"\n]\n"
			"LabelGraphics\n[ text \""
			+ label +
			"\"\nanchor \"e\"\n]\n"
			"LabelGraphics\n[ text \""
			+ solution +
			"\"\nmodel \"sandwich\"\nanchor \"s\"\n]\n"
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
		if (!isEnabled()) return;
		std::string label = "bag " + std::to_string(id);
		graphNode(id, label);
		graphSolutionNode(++countSol, label, solution);
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
		if (!isEnabled()) return;
		graphSolutionNode(++countJoin, "Join " + std::to_string(id1) + "~" + std::to_string(id2), solution);
		graphEdge(id1, countJoin);
		graphEdge(id2, countJoin);
		joinmap.emplace(id1, countJoin);
		joinmap.emplace(id2, countJoin);
	}


	/// <summary>
	/// Addes the edges from the tree-decomposition into the graph.
	/// </summary>
	/// <remarks>Any outgoing edges from id '0' are not added.</remarks>
	/// <param name="dec">Pointer to the tree-decomposition containing used edges.</param>
	void Graphoutput::graphEdgeSet(treedecType* dec)
	{
		if (!isEnabled()) return;
		for (auto b : dec->bags) {
			for (auto e : b.edges) {
				// no backward from 0	
				if (e->id != 0) {
					// replace joined nodes with the join result:
					auto search = joinmap.find(e->id);
					if (search != joinmap.end()) {
						graphEdge(search->second, b.id);
					}
					else {
						graphEdge(e->id, b.id);
					}

				}
			}
		}
	}

	void Graphoutput::graphStart(treedecType* dec)
	{
		if (!isEnabled()) return;
		// collect variables for each bag for later use
		for (auto b : dec->bags) {
			variablesmap.emplace(b.id, b.variables);
		}
		// overwrite existing content with the opening line
		Graphoutput::graphout("graph\n[\n", false);
	}

	void Graphoutput::graphEnd()
	{
		if (!isEnabled()) return;
		Graphoutput::graphout("\n]\n");
	}

	/// <summary>
	/// Output Cypherquery to create the nodes from the SAT formula
	/// </summary>
	/// <param name="satFormula">The sat formula.</param>
	void Graphoutput::neo4jSat(satformulaType* satFormula)
	{
		if (!isEnabled()) return;
		const std::string
			primaledge = "SHARECLAUSE",
			incidenceedge = "CONTAINS",
			dualedge = "SHAREVAR";

		std::ofstream file(satFile);
		if (file.is_open()) {
			std::stringstream stream;
			stream << "CREATE \n";
			stream << "(:SatFormula{"
				<< "numVars:" << satFormula->numVars
				<< ",numClauses:" << satFormula->clauses.size()
				<< ",unsat:" << satFormula->unsat
				<< ",facts:\"" << satFormula->facts << "\"})";

			// stream << "\n// ========== INCIDENCE GRAPH ===========\n";

			// Variables
			for (int i = satFormula->numVars; i > 0; i--) {
				stream << ",(v" << i << ":Variable {id:" << i << "})";
			}
			// Clauses
			int clauseid = -1;
			for (auto clause : satFormula->clauses) {
				++clauseid;
				stream << "\n,(c" << clauseid << ":Clause {id:" << clauseid << ",content:\"" << clause << "\"})";
				// Edges
				for (auto var : clause) {
					stream << ",(" << "c" << clauseid << ")-[:" + incidenceedge + "]->(v" << std::abs(var) << ")";
				}
			}

			stream << ";"
				"\n\n// ====== DUAL GRAPH QUERY ==========="
				"\nMATCH (cl1:Clause)-[]-(:Variable)-[]-(cl2:Clause)  WHERE cl1.id<>cl2.id MERGE (cl1)-[:" 
				+ dualedge + "]-(cl2);"
				"\n\n// ====== PRIMAL GRAPH QUERY ========="
				"\nMATCH (va1:Variable)<-[]-(:Clause)-[]->(va2:Variable) WHERE va1.id<>va2.id MERGE (va1)-[:"
				+ primaledge + "]-(va2);";

			stream << "\n";
			file << stream.str();
			file.close();
		}
		else { std::cerr << "Failed to open file : " << satFile << " with " << errno << std::endl; }
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