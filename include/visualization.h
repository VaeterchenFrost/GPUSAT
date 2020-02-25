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

  private:
    bool outputEnabled = false;
    std::string bagpre = "bag %d";
    std::string visufile;
    Json::StreamWriterBuilder *writerBuilder;

    void visuout(std::string string, bool append = true);
    void setFile(std::string filename);
};
} // namespace gpusat

#endif