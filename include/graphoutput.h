#ifndef GPUSAT_GRAPHOUTPUT_H
#define GPUSAT_GRAPHOUTPUT_H

#include <map>
#include <string>
#include <types.h>

namespace gpusat {

class Graphoutput {

  public:
    static const std::string Graphoutput::DUALEDGE = "SHARE_VAR";
    static const std::string Graphoutput::INCIDENCEEDGE = "VAR_IN_CLAUSE";
    static const std::string Graphoutput::PRIMALEDGE = "SHARE_CLAUSE";
    static const std::string Graphoutput::CONTAINS = "CONTAINS";
    static const std::string Graphoutput::USES_BAG = "USES_BAG";

    static const unsigned int baseIdSol = 1'000'000;
    static const unsigned int baseIdJoin = 10'000'000; // avoid factor 2 that would collide with adding two ids.

    Graphoutput(std::string filename) {
        setFile(filename);
    }

    void nodeBag(unsigned int id, std::string solution);
    void nodeJoin(unsigned int id1, unsigned int id2, std::string solution);

    void graphEdgeSet(treedecType *dec);
    void graphStart(treedecType *dec);
    void graphEnd();

    std::string getFilename() {
        return graphfile;
    }

    bool isEnabled() {
        return outputEnabled;
    }

    void neo4jSat(satformulaType *satFormula);
    void neo4jTD(treedecType *treeDec);

  private:
    bool outputEnabled = false;
    std::map<int, int> joinmap;
    std::map<int, std::vector<cl_long>> variablesmap;

    unsigned int countJoin = baseIdJoin;
    unsigned int countSol = baseIdSol;
    std::string graphfile;
    std::string satFile = "cypherSatFormula.txt";
    std::string tdFile = "cypherTreedec.txt";

    void graphEdge(unsigned int source, unsigned int target);
    void graphNode(unsigned int id, std::string label);
    void graphSolutionNode(unsigned int id, std::string label, std::string solution);

    void graphout(std::string string, bool append = true);
    void setFile(std::string filename);
};
} // namespace gpusat

#endif // !GPUSAT_GRAPHOUTPUT_H
