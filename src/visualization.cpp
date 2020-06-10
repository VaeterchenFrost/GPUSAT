#include <assert.h>
#include <fstream>
#include <gpusatutils.h>
#include <iostream>
#include <sstream>
#include <types.h>
#include <visualization.h>

#define LOGGER2(x)
#define LOGGER(x) (std::cout << "LOGGING: " << x << std::endl)

namespace gpusat {

void Visualization::writeJsonToStdout(
    Json::StreamWriter::Factory const &factory, Json::Value const &value, Json::OStream *sout) {
    std::unique_ptr<Json::StreamWriter> const writer(factory.newStreamWriter());
    writer->write(value, sout);
    std::cout << std::endl;
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

        labeldict[TAG_ID] = bag.id;
        labeldict[TAG_LABELS] = labelarray;
        labeldict[TAG_ITEMS] = var_list;

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
    if (verbose) 
        std::cout << "Entering writeJsonFile, enabled " << isEnabled();
    if (!isEnabled())
        return;

    std::ofstream stream(visufile,
                         append ? std::ios_base::app : std::ios_base::out);

    if (stream.is_open()) {
        Json::Value visu_json;
        if (clausesJson.type() == Json::ValueType::arrayValue) {
            Json::Value incidenceGraph;
            incidenceGraph[TAG_VARNAMEONE] = "c_";
            incidenceGraph[TAG_VARNAMETWO] = "v_";
            incidenceGraph[TAG_INFERPRIMAL] = true;
            incidenceGraph[TAG_EDGES] = clausesJson;

            visu_json[TAG_INCID] = incidenceGraph;
        } else {
            if (verbose) LOGGER("clausesJson not array-type");
        }
        visu_json[TAG_GENERAL] = false;

        if (treeDecJson.type() == Json::ValueType::objectValue) {
            visu_json[TAG_TDJ] = treeDecJson;
        } else {
            if (verbose) LOGGER("treeDecJson not object-type");
        }
        if (tdTimeline.type() == Json::ValueType::arrayValue) {
            visu_json[TAG_TDTIMELINE] = tdTimeline;
        } else {
            if (verbose) LOGGER("tdTimeline not array-type");
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
 * we disable further output from the Visualization.
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
 *  List[Dict{INT: List[Int]}]
 * Error if:
 *  sat formula is null or empty.
 */
void Visualization::visuClauses(satformulaType *sat) {
    if (sat == nullptr || sat->clauses.empty()) {
        std::cerr << "tried to read from empty satformulaType in Visualization::visuClauses\n";
        return;
    }

    Json::Value resultJson;
    Json::Value varsArr;
    Json::Value clauseJ;
    auto clause_counter = 0u;

    for (auto clause : sat->clauses) {
        for (auto var : clause) {
            varsArr.append(var);
        }
        clauseJ[TAG_ID] = ++clause_counter;
        clauseJ[TAG_LIST] = varsArr;

        resultJson.append(clauseJ);
        varsArr.clear();
        clauseJ.clear();
    }

    clausesJson = resultJson;
}
} // namespace gpusat