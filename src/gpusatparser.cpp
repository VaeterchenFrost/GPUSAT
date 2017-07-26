#include <math.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <main.h>
#include <gpusatparser.h>

namespace gpusat {

    satformulaType CNFParser::parseSatFormula(std::string formula) {
        satformulaType ret = satformulaType();
        std::stringstream ss(formula);
        std::string item;
        std::queue<std::queue<cl_long  >> *clauses = NULL;
        cl_long clauseSize = 0;
        while (getline(ss, item)) {
            char type = item.at(0);
            if (type == 'c') {
                //comment (ignore)
            } else if (type == 'p') {
                //start
                parseProblemLine(ret, item, clauses);
            } else {
                //clause
                parseClauseLine(item, clauses, clauseSize);
            }
        }

        if (clauses != NULL) {
            int a = 0, s = 0;
            ret.totalNumVar = clauseSize;
            ret.clauses = new cl_long[clauseSize]();
            while (!clauses->empty()) {
                std::queue<cl_long> &clause = clauses->front();
                ret.numVarsC[a] = clause.size();

                int b = 0;
                while (!clause.empty()) {
                    ret.clauses[s + b] = clause.front();
                    clause.pop();
                    b++;
                }
                std::sort(&ret.clauses[s], &ret.clauses[s + b]);
                clauses->pop();
                s += ret.numVarsC[a];
                a++;
            }
        }
        return ret;
    }

    void CNFParser::parseClauseLine(std::string item, std::queue<std::queue<cl_long>> *clauses, cl_long &clauseSize) {
        std::stringstream sline(item);
        std::string i;
        std::queue<cl_long> clause;
        getline(sline, i, ' ');
        cl_long match_count = 1;
        std::istringstream ss(item);
        std::string word;
        while (ss >> word) {
            std::istringstream maybenumber(word);
            int number = 0;
            if (maybenumber >> number) {
                match_count++;
            }
        }
        clauseSize += match_count - 1;
        while (!sline.eof()) {
            if (i.size() > 0) {
                if (stoi(i) == 0) {
                    break;
                } else {
                    clause.push(stoi(i));
                }
            }
            getline(sline, i, ' ');
        }
        clauses->push(clause);
    }

    void CNFParser::parseProblemLine(satformulaType &satformula, std::string item,
                                     std::queue<std::queue<cl_long>> *&clauses) {
        std::stringstream sline(item);
        std::string i;
        getline(sline, i, ' '); //p
        getline(sline, i, ' '); //cnf
        getline(sline, i, ' '); //num vars
        getline(sline, i, ' '); //num clauses
        satformula.numclauses = stoi(i);
        satformula.numVarsC = new cl_long[satformula.numclauses]();
        clauses = new std::queue<std::queue<cl_long>>();
    }

    treedecType TDParser::parseTreeDecomp(std::string graph) {
        treedecType ret = treedecType();
        std::stringstream ss(graph);
        std::string item;
        std::queue<cl_long> **edges = NULL;
        while (getline(ss, item)) {
            char type = item.at(0);
            if (type == 'c') {
                //comment (ignore)
            } else if (type == 's') {
                //start
                parseStartLine(ret, item, edges);
            } else if (type == 'b') {
                //bag
                parseBagLine(ret, item);
            } else {
                //edge
                parseEdgeLine(item, edges);
            }
        }

        if (edges != NULL) {
            for (int a = 0; a < ret.numb; a++) {
                ret.bags[a].edges = new cl_long[edges[a]->size()]();
                ret.bags[a].numEdges = edges[a]->size();
                int b = 0;
                while (!edges[a]->empty()) {
                    ret.bags[a].edges[b] = edges[a]->front();
                    edges[a]->pop();
                    b++;
                }
            }
        }
        return ret;
    }

    void TDParser::parseEdgeLine(std::string item, std::queue<cl_long> **edges) {
        std::stringstream sline(item);
        std::string i;
        getline(sline, i, ' '); //start
        cl_long start = stoi(i);
        getline(sline, i, ' '); //end
        cl_long end = stoi(i);
        edges[start - 1]->push(end);
    }

    void TDParser::parseStartLine(treedecType &ret, std::string &item, std::queue<cl_long> **&edges) {
        std::stringstream sline(item);
        std::string i;
        getline(sline, i, ' '); //s
        getline(sline, i, ' '); //tw
        getline(sline, i, ' '); //num bags
        ret.bags = new bagType[stoi(i)];
        ret.numb = stoi(i);
        edges = new std::queue<cl_long> *[stoi(i)];
        for (int a = 0; a < stoi(i); a++) {
            edges[a] = new std::queue<cl_long>();
        }
    }

    void TDParser::parseBagLine(treedecType ret, std::string item) {
        std::stringstream sline(item);
        std::string i;
        getline(sline, i, ' '); //b
        getline(sline, i, ' '); //bag number
        int bnum = stoi(i);
        int a = 0;
        cl_long match_count = 0;
        std::istringstream ss(item);
        std::string word;
        while (ss >> word) {
            std::istringstream maybenumber(word);
            int number = 0;
            if (maybenumber >> number) {
                match_count++;
            }
        }

        ret.bags[bnum - 1].variables = new cl_long[match_count - 1]();
        ret.bags[bnum - 1].numSol = (long) pow(2, match_count - 1);
        //ret.bags[bnum - 1].solution = new cl_long[ret.bags[bnum - 1].numSol]();
        ret.bags[bnum - 1].numVars = match_count - 1;
        while (getline(sline, i, ' ')) //vertices
        {
            ret.bags[bnum - 1].variables[a] = stoi(i);
            a++;
        }
        std::sort(ret.bags[bnum - 1].variables, &ret.bags[bnum - 1].variables[ret.bags[bnum - 1].numVars]);
    }
}