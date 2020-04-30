#ifndef GPUSAT_VISUALISIERUNG_H
#define GPUSAT_VISUALISIERUNG_H

#include <iostream>
#include <json/reader.h>
#include <json/value.h>
#include <json/writer.h>
#include <memory>
#include <string>
#include <types.h>
#include <vector>

namespace gpusat {

/**
 * Class for storing 2dim 'ulong' values
 * using vector and a 1dim memory layout.
 * See https://stackoverflow.com/a/2076668
 * Example usage:
 * 
 * BagMatrix mygrid (5, 5);
    for (int r = 0; r < mygrid.rows(); r++){
        for (int c = 0; c < mygrid.columns(); c++){
	        mygrid[r][c] = r * c;
	    }
    }
  // Print the array
    for (int r = 0; r < mygrid.rows(); r++){
    std::cout << std::hex << &(mygrid[r][0]) << std::dec;
    std::cout << ": ";
    for (int c = 0; c < mygrid.columns(); c++){
	    std::cout << mygrid[r][c] << ' ';
	}
    std::cout << std::endl;
    }
 */
class BagMatrix {
  public:
    BagMatrix(size_t rows, size_t cols);
    BagMatrix();
    cl_long &operator()(size_t i, size_t j);
    cl_long operator()(size_t i, size_t j) const;
    size_t rows() { return mRows; }
    size_t columns() { return mCols; }

  private:
    size_t mRows;
    size_t mCols;
    std::vector<cl_long> mData;
};

/**
 * Solutions in Timeline. Example:
 * [["id", "v2", "v4", "n Sol"],
 *  [0, 0, 0, 1], [1, 1, 0, 2], [2, 0, 1, 2],
 *  [3, 1, 1, 3]]
 */
struct TableLines {
    std::vector<std::string> headline;
    BagMatrix solutions;
    cl_double totalSol;
};

/// Construct Tablelines from a solved node of the tree decomposition
/// If no solution is stored in this node return empty grid and totalSol -1.
TableLines solJson(bagType node, dataStructure solutionType);

class Visualization {

  public:
    Visualization(std::string filename) {
        setFile(filename);
        std::cout << "Opened visualization with file " << visufile << " " << (isEnabled() ? "true" : "false") << "\n----------" << std::endl;
        writerBuilder = new Json::StreamWriterBuilder();
        (*writerBuilder)["commentStyle"] = "None";
        (*writerBuilder)["indentation"] = "    ";
    }

    std::string getFilename() {
        return visufile;
    }

    bool isEnabled() {
        return outputEnabled;
    }

    void testJson();
    void visuTreeDec(treedecType *treeDec);
    void visuSatForm(satformulaType *sat); // consuming std::vector<std::vector<cl_long>> clauses;
    Json::StreamWriterBuilder *getWriterBuilder();
    void writeJsonToStdout(Json::StreamWriter::Factory const &factory, Json::Value const &value, Json::OStream *sout = &std::cout);
    void writeJsonToStdout(Json::Value const &value, Json::OStream *sout = &std::cout);
    // One step in the timeline
    void tdTimelineAppend(std::vector<cl_long> bag_ids, TableLines tablelines, std::string const toplabel = "", std::string const bottomlabel = "", bool transpose = true);
    void tdTimelineAppend(std::vector<cl_long> bag_ids);
    void writeJsonFile(bool append = false);

    Json::Value getTdTimeline() {
        return tdTimeline;
    }
    Json::Value getTreeDecJson() {
        return treeDecJson;
    }
    Json::Value getClausesJson() {
        return clausesJson;
    }

  private:
    bool outputEnabled = false;
    const std::string
        BAGPRE = "bag %s",
        TAG_BAGPRE = "bagpre",
        TAG_LABELDICT = "labeldict",
        TAG_EDGEARRAY = "edgearray",
        TAG_NUMVARS = "numVars";

    std::string visufile;
    Json::StreamWriterBuilder *writerBuilder;

    Json::Value treeDecJson;
    Json::Value clausesJson;
    Json::Value tdTimeline;
    Json::Value variablesTimeline;
    void setFile(std::string filename);
};
} // namespace gpusat

#endif