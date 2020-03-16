#include <assert.h>
#include <fstream>
#include <gpusatutils.h>
#include <iostream>
#include <sstream>
#include <types.h>
#include <visualization.h>
#define LOGGER2(x)
#define LOGGER(x) (std::cout << "LOGGING: " << x << std::endl)

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
    Json::StreamWriter::Factory const &factory, Json::Value const &value, Json::OStream *sout) {
    std::unique_ptr<Json::StreamWriter> const writer(factory.newStreamWriter());
    writer->write(value, sout);
    std::cout << std::endl; // add lf and flush
}

void Visualization::writeJsonToStdout(Json::Value const &value, Json::OStream *sout) {
    Visualization::writeJsonToStdout(*writerBuilder, value, sout);
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
    // JSONCPP_DEPRECATED("Use StreamWriterBuilder instead") JSON_API FastWriter
    // Json::FastWriter fastWriter;
    // std::stringstream jsonMessage(fastWriter.write(fromScratch));
    // std::cout << jsonMessage.str();
    std::stringstream jsonMessage;
    jsonMessage << (fromScratch);
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

void Visualization::visuTreeDec(treedecType *treeDec) {
    Json::Value tdGraph;
    Json::Value labelarray;
    Json::Value labeldict;
    Json::Value edgearray;

    tdGraph[TAG_BAGPRE] = BAGPRE;
    tdGraph[TAG_NUMVARS] = treeDec->numVars;
    for (auto bag : treeDec->bags) {
        std::ostringstream variables;
        Json::Value var_list;
        variables << "["; // overwrite previous

        for (auto var_id : bag.variables) {
            var_list.append(var_id);
            variables << var_id << " ";
            var_list.append(var_id);
        }
        variables.seekp(-1, std::ios::end); // overwrite last space
        variables << "]";

        labelarray.append(variables.str()); // might be the only label...

        labeldict["id"] = bag.id;
        labeldict["labels"] = labelarray;
        labeldict["items"] = var_list;

        tdGraph[TAG_LABELDICT].append(labeldict);
        labelarray.clear();
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
    treeDecJson = tdGraph; // Store result in Visualization object
}

/**
 * bag_ids: asserts not empty  
 *  - if one: one (IF) solution found
 *  - more: join solution found
 * tablelines: all lines for the table
 * toplabel: string to label the whole solution
 * bottomlabel: string to summarize the solution
 * transpose: whether the tablelines are rowfirst (true) or not.
 */
void Visualization::tdTimelineAppend(std::vector<cl_long> bag_ids, TableLines tablelines, std::string const toplabel, std::string const bottomlabel, bool transpose) {
    assert(bag_ids.empty() == false);
    Json::Value timestepJson;
    if (bag_ids.size() == 1) {
        timestepJson.append(bag_ids[0]);
        LOGGER2(bag_ids[0]);
    } else {
        Json::Value ids;
        for (auto id : bag_ids)
            ids.append(id);
        timestepJson.append(ids);
        LOGGER2(ids);
    }

    // add table
    Json::Value solutionArJson;
    Json::Value rowJson;
    Json::Value solutionInfo;
    for (auto val : tablelines.headline)
        rowJson.append(val);
    solutionArJson.append(rowJson);
    rowJson.clear();

    for (int r = 0; r < tablelines.solutions.rows(); r++) { // add grid rows
        for (int c = 0; c < tablelines.solutions.columns(); c++) {
            rowJson.append(tablelines.solutions(r, c));
        }
        solutionArJson.append(rowJson);
        rowJson.clear();
    }
    solutionInfo.append(solutionArJson);
    solutionInfo.append(toplabel);
    solutionInfo.append("sum: " + std::to_string((ulong)tablelines.totalSol));
    solutionInfo.append(transpose);
    timestepJson.append(solutionInfo);
    tdTimeline.append(timestepJson);
}

/**
 * Only append those bags as one array.
 * Might even be only one bag.
 * The behaviour with zero length is not implemented (yet).
*/
void Visualization::tdTimelineAppend(std::vector<cl_long> bag_ids) {

    assert(bag_ids.empty() == false);

    Json::Value ids;
    for (auto id : bag_ids)
        ids.append(id);
    tdTimeline.append(ids);
    LOGGER2(ids);
}

Json::StreamWriterBuilder *Visualization::getWriterBuilder() {
    return writerBuilder;
}

void Visualization::writeJsonFile(bool append) {
    std::cout << "Entering writeJsonFile, enabled " << isEnabled();
    if (!isEnabled())
        return;

    std::ofstream stream(visufile,
                         append ? std::ios_base::app : std::ios_base::out);

    if (stream.is_open()) {
        Json::Value visu_json;
        if (clausesJson.type() == Json::ValueType::arrayValue) {
            visu_json["clausesJson"] = clausesJson;
        } else {
            LOGGER("clausesJson not array-type");
        }
        if (treeDecJson.type() == Json::ValueType::objectValue) {
            visu_json["treeDecJson"] = treeDecJson;
        } else {
            LOGGER("treeDecJson not object-type");
        }
        if (tdTimeline.type() == Json::ValueType::arrayValue) {
            visu_json["tdTimeline"] = tdTimeline;
        } else {
            LOGGER("tdTimeline not array-type");
        }

        writeJsonToStdout(visu_json, &stream);

        stream.close();
    } else {
        std::cerr << "Failed to open file : " << visufile << " with " << errno
                  << std::endl;
    }
}
/**
 * Store a new filename.
 * In case the name has 0 length like "",
 * we disable further output from the Visualization
 */
void Visualization::setFile(std::string filename) {
    if (filename.compare("")) {
        outputEnabled = true;
    } else
        outputEnabled = false;
    visufile = filename;
}
BagMatrix::BagMatrix(size_t rows, size_t cols) : mRows(rows), mCols(cols), mData(rows * cols) {
}
BagMatrix::BagMatrix() : mRows(0), mCols(0), mData(0){};

cl_long &BagMatrix::operator()(size_t i, size_t j) {
    return mData[i * mCols + j];
}

cl_long BagMatrix::operator()(size_t i, size_t j) const {
    return mData[i * mCols + j];
}

TableLines solJson(bagType node, dataStructure solutionType) {

    size_t var_count = node.variables.size();
    cl_double totalSol = 0;
    TableLines lines;
    std::vector<std::string> headline;
    if (node.solution->elements != nullptr) {
        headline.push_back("id");
        for (int i = 0; i < var_count; ++i) {
            headline.push_back("v" + std::to_string(node.variables[i]));
        }
        headline.push_back("n Sol");
        lines.headline = headline;

        BagMatrix mygrid(1 << var_count, var_count + 2); // Grid 2^var_count * ('id' + var_count + 'n Sol')
        // all ID-lines
        cl_long row_n;
        for (cl_long id = node.solution->minId; id < node.solution->maxId; id++) {
            row_n = id - node.solution->minId;
            mygrid(row_n, 0) = id;
            cl_long id_b = id;
            for (int v = 0; v < var_count; ++v) { // find binary rep. of id
                mygrid(row_n, v + 1) = id_b % 2;
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
            mygrid(row_n, mygrid.columns() - 1) = sol; // Further cast from double to BAGID ?
        }
        lines.solutions = mygrid;

        lines.totalSol = totalSol;
        return {headline, mygrid, totalSol};
    } else {
        BagMatrix mygrid;
        return {headline, mygrid, -1.};
    }
}

/**
 * Save the clauses into the Visualization::clausesJson.
 * Form: 
 *  List[Dict{"id": List[Int]}]
 * Error if:
 *  sat formula is null or empty.
 */
void Visualization::visuSatForm(satformulaType *sat) {
    if (sat == nullptr || sat->clauses.empty()) {
        std::cerr << "tried to read from empty satformulaType in Visualization::visuSatForm\n";
        return;
    }

    Json::Value resultJsom;
    Json::Value varsArr;
    Json::Value clauseJ;
    auto clause_counter = 0u;

    for (auto clause : sat->clauses) {
        for (auto var : clause) {
            varsArr.append(var);
        }
        clauseJ["id"] = ++clause_counter;
        clauseJ["list"] = varsArr;

        resultJsom.append(clauseJ);
        varsArr.clear();
        clauseJ.clear();
    }

    clausesJson = resultJsom;
}
} // namespace gpusat