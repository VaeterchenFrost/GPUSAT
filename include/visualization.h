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
typedef cl_long BAGID;
/**
 * Class for storing 2dim 'ulong' values
 * using unique_ptr and a 1dim memory layout.
 * See https://stackoverflow.com/a/32279494
 * Example usage:
 * 
 * Grid mygrid (5, 5);
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
class Grid {
    size_t _rows;
    size_t _columns;
    std::unique_ptr<BAGID[]> data;

  public:
    Grid(size_t rows, size_t columns)
        : _rows{rows},
          _columns{columns},
          data{std::make_unique<BAGID[]>(rows * columns)} {}

    Grid() : _rows(0), _columns(0), data(nullptr){};

    void operator=(Grid &grid) {
        _rows = grid._rows;
        _columns = grid._columns;
        data = std::move(grid.data);
    }
    
    size_t rows() const { return _rows; }

    size_t columns() const { return _columns; }

    BAGID *operator[](size_t row) { return row * _columns + data.get(); }

    BAGID &operator()(size_t row, size_t column) {
        return data[row * _columns + column];
    }
};

/**
 * Solutions in Timeline. Example:
 * [["id", "v2", "v4", "n Sol"],
 *  [0, 0, 0, 1], [1, 1, 0, 2], [2, 0, 1, 2],
 *  [3, 1, 1, 3]]
 */
struct TABLELINES {
    std::vector<std::string> headline;
    Grid solutions;
};

/// Generate a formatted stringoutput for a solved node with solutions
inline std::string solJson(bagType node, dataStructure solutionType) {

    size_t var_count = node.variables.size();
    cl_double totalSol = 0;
    TABLELINES lines;
    std::vector<std::string> headline;
    if (node.solution->elements != nullptr) {
        headline.push_back("id");
        for (int i = 0; i < var_count; ++i) {
            headline.push_back("v" + node.variables[i]);
        }
        headline.push_back("n Sol");
        lines.headline = headline;

        Grid mygrid(1 << var_count, var_count + 2); // Grid 2^var_count * ('id' + var_count + 'n Sol')
        // all ID-lines
        cl_long row_n;
        for (cl_long id = node.solution->minId; id < node.solution->maxId; id++) {
            row_n = id - node.solution->minId;
            mygrid[row_n][0] = id;
            cl_long id_b = id;
            for (int v = 0; v < var_count; ++v) { // find binary rep. of id
                mygrid[row_n][v + 1] = id_b % 2;
                id_b /= 2;
            }
            cl_double sol = -1.;

            if (solutionType == dataStructure::TREE) {
                sol = getCount(id, node.solution->elements, var_count) * pow(2, node.correction);
            } else {
                sol = *reinterpret_cast<cl_double *>(&node.solution->elements[id - node.solution->minId]) * pow(2, node.correction);
            }
            totalSol += sol;
            // last slot filled with solution in row
            mygrid[row_n][mygrid.columns - 1] = (BAGID)sol; // Further cast from double to BAGID ?
        }
        lines.solutions = mygrid;

        os << "sum: " << totalSol;
    } else {
        os << "no solutions (nullptr)";
    }
    return os.str();
}

class Visualization {

  public:
    Visualization(std::string filename = "visugpusat.json") {
        setFile(filename);

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
    void visuTD(treedecType *treeDec);
    Json::StreamWriterBuilder *getWriterBuilder();
    void writeJsonToStdout(Json::StreamWriter::Factory const &factory, Json::Value const &value);
    // One step in the timeline
    void tdTimelineAppend(std::vector<BAGID> bag_ids, TABLELINES tablelines, std::string const toplabel = "", std::string const bottomlabel = "", bool transpose = true);
    void tdTimelineAppend(std::vector<BAGID> bag_ids);
    Json::Value getTdTimeline() {
        return tdTimeline;
    }
    // template <typename Iter>
    // inline static Json::Value copyIntoJsonArray(Iter it, Iter end) {
    //     Json::Value result;
    //     for (; it!=end; ++it) { it. }
    //     return result;
    // }

  private:
    bool outputEnabled = false;
    const std::string
        BAGPRE = "bag %s",
        TAG_BAGPRE = "bagpre",
        TAG_LABELDICT = "labeldict",
        TAG_EDGEARRAY = "edgearray";

    std::string visufile;
    Json::StreamWriterBuilder *writerBuilder;
    Json::Value tdTimeline;
    void visuout(std::string string, bool append = true);
    void setFile(std::string filename);
};
} // namespace gpusat

#endif