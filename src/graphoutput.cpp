#include <fstream>
#include <gpusatutils.h>
#include <graphoutput.h>
#include <iostream>
#include <sstream>
#include <types.h>

namespace gpusat {

/// <summary>
/// Writes one edge between two nodes.
/// </summary>
/// <param name="source">The source id.</param>
/// <param name="target">The target id.</param>
void Graphoutput::graphEdge(unsigned int source, unsigned int target) {
    if (!isEnabled())
        return;
    graphout(
        "edge\n[\n"
        "source " +
        std::to_string(source) + "\n"
                                 "target " +
        std::to_string(target) + "\n"
                                 "]\n");
}

/// <summary>
/// Writes a node with one label.
/// </summary>
/// <param name="id">The identifier.</param>
/// <param name="label">The label.</param>
void Graphoutput::graphNode(unsigned int id, std::string label) {
    if (!isEnabled())
        return;
    // find the original variables for this bag:
    std::ostringstream variables;
    auto search = variablesmap.find(id);
    if (search != variablesmap.end()) {
        std::vector<cl_long> vars = search->second;
        variables << "[";
        for (cl_long var : vars) {
            variables << " " << var;
        }
        variables << " ]";
    } else {
        std::cerr << "No value for key " << id << "in variablesmap!\n";
    }
    graphout(
        "node\n"
        "[\n id " +
        std::to_string(id) + "\n"
                             "label \"" +
        label + "\"\n"
                "graphics\n[\n"
                "x 0\n"
                "y 0\n"
                "w 15\n"
                "h 15\n"
                "type \"ellipse\"\n]\n"
                "LabelGraphics\n[ text \"" +
        label +
        "\"\n"
        "anchor \"w\"\n"
        "]\n"
        "LabelGraphics\n[ text \"" +
        variables.str() +
        "\"\n"
        "anchor \"e\"\n"
        "]\n"
        "]\n");
}

/// <summary>
/// Writes a node with one label and a solution label.
/// </summary>
/// <param name="id">The identifier.</param>
/// <param name="label">The corresponding label.</param>
/// <param name="solution">The solution to create a label.</param>
void Graphoutput::graphSolutionNode(unsigned int id, std::string label, std::string solution) {
    if (!isEnabled())
        return;
    graphout(
        "node\n"
        "[\n id " +
        std::to_string(id) + "\n"
                             "label \"" +
        label + "\"\n"
                "graphics\n[\n"
                "x 0\n"
                "y 0\n"
                "type \"star8\"\n]\n"
                "LabelGraphics\n[ text \"" +
        label +
        "\"\nanchor \"e\"\n]\n"
        "LabelGraphics\n[ text \"" +
        solution +
        "\"\nmodel \"sandwich\"\nanchor \"s\"\n]\n"
        "]\n");
}

/// <summary>
/// Creates one node with the corresponding solution in a connected extra bag.
/// </summary>
/// <param name="id">The identifier of the bag.</param>
/// <param name="solution">The solution in string form.</param>
void Graphoutput::nodeBag(unsigned int id, std::string solution) {
    if (!isEnabled())
        return;
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
void Graphoutput::nodeJoin(unsigned int id1, unsigned int id2, std::string solution) {
    if (!isEnabled())
        return;
    graphSolutionNode(++countJoin, "Join " + std::to_string(id1) + "~" + std::to_string(id2), solution);
    graphEdge(id1, countJoin);
    graphEdge(id2, countJoin);
    joinmap.emplace(id1, countJoin);
    joinmap.emplace(id2, countJoin);
}

/// <summary>
/// Adds the edges from the tree-decomposition into the graph.
/// </summary>
/// <remarks>Any outgoing edges from id '0' are not added.</remarks>
/// <param name="dec">Pointer to the tree-decomposition containing used edges.</param>
void Graphoutput::graphEdgeSet(treedecType *dec) {
    if (!isEnabled())
        return;
    for (auto b : dec->bags) {
        for (auto e : b.edges) {
            // no backward from 0
            if (e->id != 0) {
                // replace joined nodes with the join result:
                auto search = joinmap.find(e->id);
                if (search != joinmap.end()) {
                    graphEdge(search->second, b.id);
                } else {
                    graphEdge(e->id, b.id);
                }
            }
        }
    }
}

/// <summary>
/// If enabled: OVERWRITE existing content with the start of a dot-graph.
/// </summary>
/// <param name="dec">To collect variables for each bag for later use.</param>
void Graphoutput::graphStart(treedecType *dec) {
    if (!isEnabled())
        return;
    // collect variables for each bag for later use
    for (auto b : dec->bags) {
        variablesmap.emplace(b.id, b.variables);
    }
    // overwrite existing content with the start of the graph
    graphout("graph\n[\n", false);
}

/// <summary>
/// If enabled: Add the closing bracket(s) to the output.
/// </summary>
void Graphoutput::graphEnd() {
    if (!isEnabled())
        return;
    graphout("\n]\n");
}

/// <summary>
/// Output Cypherquery to create the nodes from the SAT formula
/// </summary>
/// <param name="satFormula">The sat formula.</param>
void Graphoutput::neo4jSat(satformulaType *satFormula) {
    if (!isEnabled())
        return;

    std::ofstream file(satFile);
    if (file.is_open()) {
        std::stringstream stream;

        stream << "MERGE \n";
        stream << "(:SatFormula{"
               << "numVars:" << satFormula->numVars
               << ",numClauses:" << satFormula->clauses.size()
               << ",unsat:" << satFormula->unsat
               << ",facts:\"" << satFormula->facts << "\"})";

        stream << "\n// ========== INCIDENCE GRAPH ===========\n\n";

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
                stream << ",("
                       << "c" << clauseid << ")-[:" + INCIDENCEEDGE + "]->(v" << std::abs(var) << ")";
            }
        }

        stream << ";"
                  "\n\n// ========== DUAL GRAPH QUERY ==========="
                  "\nMATCH (cl1:Clause)-[]-(:Variable)-[]-(cl2:Clause)  WHERE cl1.id<>cl2.id MERGE (cl1)-[:" +
                      DUALEDGE + "]-(cl2);"
                                 "\n\n// ========== PRIMAL GRAPH QUERY ========="
                                 "\nMATCH (va1:Variable)<-[]-(:Clause)-[]->(va2:Variable) WHERE va1.id<>va2.id MERGE (va1)-[:" +
                      PRIMALEDGE + "]-(va2);";
        stream << "\n";
        file << stream.str();
        file.close();
    } else {
        std::cerr << "Failed to open file : " << satFile << " with " << errno << std::endl;
    }
}

/// <summary>
/// Output Cypher query to create the tree decomposition.
/// </summary>
/// <param name="treeDec">The tree decomposition.</param>
void Graphoutput::neo4jTD(treedecType *treeDec) {
    if (!isEnabled())
        return;
    std::ofstream file(tdFile);
    if (file.is_open()) {
        std::stringstream stream;

        for (int i = treeDec->numVars; i > 0; i--) {
            stream << "MERGE (v" << i << ":Variable {id:" << i << "})\n";
        }

        stream << "CREATE ";
        stream << "(:TreeDecomposition{"
               << "numVars:" << treeDec->numVars
               << ",numb:" << treeDec->numb
               << ",width:\"" << treeDec->width << "\"})";

        for (auto bag : treeDec->bags) {
            // Create Bag
            stream << "\nCREATE (b" << bag.id << ":Bag{id:" << bag.id << "})";
            // Edges to variables
            for (auto var_id : bag.variables) {
                stream << "\nMERGE (b" << bag.id << ")-[:" + CONTAINS + "]->(v" << var_id << ")";
            }
        }
        // Edges to bags
        for (auto bag : treeDec->bags) {
            for (auto e : bag.edges) {
                stream << "\nMERGE (b" << bag.id << ")-[:" + USES_BAG + "]->(b" << e << ")";
            }
        }
        stream << "\n";
        file << stream.str();
        file.close();
    } else {
        std::cerr << "Failed to open file : " << satFile << " with " << errno << std::endl;
    }
}

void Graphoutput::graphout(std::string string, bool append) {
    if (!isEnabled())
        return;
    std::ofstream stream(graphfile, append ? std::ios_base::app : std::ios_base::out);

    if (stream.is_open()) {
        stream << string;
        stream.close();
    } else {
        std::cerr << "Failed to open file : " << graphfile << " with " << errno << std::endl;
    }
}

void Graphoutput::setFile(std::string filename) {
    if (filename != "") {
        outputEnabled = true;
    } else
        outputEnabled = false;
    graphfile = filename;
}


} // namespace gpusat