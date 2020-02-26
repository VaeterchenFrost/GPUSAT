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
    uint columns;
    std::string *headline;
    Grid solutions;
};

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