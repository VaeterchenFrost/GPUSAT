#include <fstream>
#include <gpusatutils.h>
#include <iostream>
#include <sstream>
#include <types.h>
#include <visualization.h>

/*
TD GRAPH:
    s.node(bagpre % 4, bagNode(bagpre % 4, ["[2 3 8]", "Test2"]))
    s.node(bagpre % 3, bagNode(bagpre % 3, "[2 4 8]"))
    # s.node('join1', bagNode("Join", "2~3"))
    s.node(bagpre % 2, bagNode(bagpre % 2, "[1 2 5]"))
    s.node(bagpre % 1, bagNode(bagpre % 1, "[1 2 4 6]"))
    s.node(bagpre % 0, bagNode(bagpre % 0, "[1 4 7]"))

    s.edges(
        [(bagpre % 4, bagpre % 3), (bagpre % 2, bagpre % 1),
         (bagpre % 3, bagpre % 1), (bagpre % 1, bagpre % 0)])

TD-TIMELINE:
TIMELINE = [(0,), (1,),
                (2, ([['id', 'v1', 'v2', 'n Sol'], [0, 0, 0, 0], [1, 1, 0, 1],
                      [2, 0, 1, 1], [3, 1, 1, 2]], 'sol bag 2', 'sum: 4',
True)), (3,), (4, ([["id", "v2", "v8", "n Sol"], [0, 0, 0, 1], [1, 1, 0, 2], [2,
0, 1, 1], [3, 1, 1, 1]], "sol bag 4", "sum: 5", True)), (3, ([["id", "v2", "v4",
"n Sol"], [0, 0, 0, 1], [1, 1, 0, 2], [2, 0, 1, 2], [3, 1, 1, 3]], "sol bag 3",
"sum: 8", True)),
                ((2, 3), ([["id", "v1", "v2", "v4", "n Sol"],
                           [0, 0, 0, 0, 0],
                           [1, 1, 0, 0, 1],
                           [2, 0, 1, 0, 2],
                           [3, 1, 1, 0, 4],
                           [4, 0, 0, 1, 0],
                           [5, 1, 0, 1, 2],
                           [6, 0, 1, 1, 3],
                           [7, 1, 1, 1, 6]],
                          "sol Join 2~3", "sum: 18", True)),
                (1, ([["id", "v1", "v4", "n Sol"],
                      [0, 0, 0, 2], [1, 1, 0, 9], [2, 0, 1, 3],
                      [3, 1, 1, 6]], "sol bag 1", "sum: 20", True)),
                (0, ([["id", "v1", "v4", "v7", "n Sol"],
                      [0, 0, 0, 0, 2],
                      [1, 1, 0, 0, 0],
                      [2, 0, 1, 0, 0],
                      [3, 1, 1, 0, 0],
                      [4, 0, 0, 1, 2],
                      [5, 1, 0, 1, 9],
                      [6, 0, 1, 1, 3],
                      [7, 1, 1, 1, 6]],
                     "sol bag 0", "sum: 22", True))
                ]

INCIDENCE:
EDGELIST = [[1, [1, 4, 6]], [2, [1, -5]], [3, [-1, 7]], [4, [2, 3]], [5, [2,
5]], [6, [2, -6]], [7, [3, -8]], [8, [4, -8]], [9, [-4, 6]], [10, [-4, 7]]]
TIMELINE = (None, None,
                [1, 2, 5],
                None,
                [2, 3, 8],
                [2, 4, 8],
                [1, 2, 4],  # JOIN

                [1, 2, 4, 6],
                [1, 4, 7]
                )
*/
namespace gpusat {

void Visualization::writeJsonToStdout(
    Json::StreamWriter::Factory const &factory, Json::Value const &value) {
    std::unique_ptr<Json::StreamWriter> const writer(factory.newStreamWriter());
    writer->write(value, &std::cout);
    std::cout << std::endl; // add lf and flush
}

void Visualization::testJson() {

    // ---- create from scratch ----

    Json::Value fromScratch;
    Json::Value array;
    array.append("hello");
    array.append("world");
    fromScratch["hello"] = "world";
    fromScratch["number"] = 2;
    fromScratch["array"] = array;
    fromScratch["object"]["hello"] = "world";

    std::cout << std::endl;
    // write in a nice readable way
    writeJsonToStdout(*getWriterBuilder(), fromScratch);

    // ---- parse from string ----

    // write in a compact way
    Json::FastWriter fastWriter;
    std::stringstream jsonMessage(fastWriter.write(fromScratch));
    std::cout << jsonMessage.str();

    Json::Value parsedFromString;
    Json::CharReaderBuilder builder;
    builder.strictMode(0);

    Json::String errs;
    bool parsingSuccessful =
        parseFromStream(builder, jsonMessage, &parsedFromString, &errs);

    if (parsingSuccessful) {
        writeJsonToStdout(*getWriterBuilder(), parsedFromString);
    }
}

/*
tdGraph = {
        "bagpre" : "bag %s",
        "edgearray" : 
        [
            [ 1, 0 ],
            [ 2, 1 ],
            [ 3, 1 ],
            [ 4, 3 ]
        ],
        "labeldict" : 
        {
            "0" : [ "[1 4 7]" ],
            "1" : [ "[1 2 4 6]" ],
            "2" : [ "[1 2 5]" ],
            "3" : [ "[2 4 8]" ],
            "4" : [ "[2 3 8]" ]
        }
    }
         */
void Visualization::visuTD(treedecType *treeDec) {
    Json::Value tdGraph;
    Json::Value labeldict;
    Json::Value edgearray;

    tdGraph[TAG_BAGPRE] = BAGPRE;
    for (auto bag : treeDec->bags) {
        std::ostringstream variables;
        variables << "["; // overwrite previous

        for (auto var_id : bag.variables) {
            variables << var_id << " ";
        }
        variables.seekp(-1, std::ios::end); // overwrite last space
        variables << "]";

        labeldict.append(variables.str()); // might be the only label...

        tdGraph[TAG_LABELDICT][std::to_string(bag.id)] = labeldict;
        labeldict.clear();

        for (bagType *e : bag.edges) // Edges between bags
        {
            Json::Value edge;
            edge.append(e->id);
            edge.append(bag.id);

            edgearray.append(edge);
        }
    }
    tdGraph[TAG_EDGEARRAY] = edgearray;
    writeJsonToStdout(*getWriterBuilder(), tdGraph);
}

/**
 * bag_ids: 
 *  - if one: one (IF) solution found
 *  - more: join solution found
 */
void visuMainTimeline(BAGID &bag_ids, TABLELINES tablelines, std::string const toplabel, std::string const bottomlabel, bool transpose){

}

Json::StreamWriterBuilder *Visualization::getWriterBuilder() {
    return writerBuilder;
}

void Visualization::visuout(std::string string, bool append) {
    if (!isEnabled())
        return;

    std::ofstream stream(visufile,
                         append ? std::ios_base::app : std::ios_base::out);

    if (stream.is_open()) {
        stream << string;
        stream.close();
    } else {
        std::cerr << "Failed to open file : " << visufile << " with " << errno
                  << std::endl;
    }
}

void Visualization::setFile(std::string filename) {
    if (filename != "") {
        outputEnabled = true;
    } else
        outputEnabled = false;
    visufile = filename;
}

} // namespace gpusat