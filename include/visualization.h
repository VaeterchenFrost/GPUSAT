#ifndef GPUSAT_VISUALISIERUNG_H
#define GPUSAT_VISUALISIERUNG_H

#include <string>
#include <types.h>
// #include <map>
#include <iostream>
#include <json/reader.h>
#include <json/value.h>
#include <json/writer.h>

namespace gpusat {
/**
 * Class for storing 2dim 'unsigned int' values
 * using unique_ptr and a 1dim memory layout.
 * See https://stackoverflow.com/a/32279494
 */
class Grid {
    size_t _rows;
    size_t _columns;
    std::unique_ptr<unsigned int[]> data;

  public:
    Grid(size_t rows, size_t columns)
        : _rows{rows},
          _columns{columns},
          data{std::make_unique<unsigned int[]>(rows * columns)} {}

    size_t rows() const { return _rows; }

    size_t columns() const { return _columns; }

    unsigned int *operator[](size_t row) { return row * _columns + data.get(); }

    unsigned int &operator()(size_t row, size_t column) {
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
    unsigned int columns;
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

    void output(const Json::Value &value) {
        // querying the json object is very simple
        std::cout << value["hello"];
        std::cout << value["number"];
        std::cout << value["array"][0] << value["array"][1];
        std::cout << value["object"]["hello"];
    }

    void testJson();
    void visuTD(treedecType *treeDec);
    Json::StreamWriterBuilder *getWriterBuilder();
    void writeJsonToStdout(Json::StreamWriter::Factory const &factory, Json::Value const &value);
    void visuMainTimeline();

  private:
    bool outputEnabled = false;
    const std::string
        BAGPRE = "bag %s",
        TAG_BAGPRE = "bagpre",
        TAG_LABELDICT = "labeldict",
        TAG_EDGEARRAY = "edgearray";

    std::string visufile;
    Json::StreamWriterBuilder *writerBuilder;

    void visuout(std::string string, bool append = true);
    void setFile(std::string filename);
};
} // namespace gpusat

#endif