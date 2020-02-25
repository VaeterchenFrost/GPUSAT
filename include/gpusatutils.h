#ifndef GPUSAT_GPUSAUTILS_H
#define GPUSAT_GPUSAUTILS_H
#if defined __CYGWIN__ || defined __MINGW32__
#define alloca __builtin_alloca
#endif

#include <CL/cl_platform.h>
#include <chrono>
#include <iostream>
#include <sstream>
#include <types.h>
#include <vector>

namespace gpusat {

inline std::ostream &operator<<(std::ostream &os, const std::vector<bagType *> vec) {
    if (vec.empty())
        return os << "()";
    os << "-(";
    for (auto bag : vec) {
        os << bag->id << " ";
    }
    return os << ")-";
}

inline std::ostream &operator<<(std::ostream &os, const std::vector<cl_long> vec) {
    os << "[";
    for (auto n : vec) {
        os << n << " ";
    }
    return os << "]";
};

inline std::ostream &operator<<(std::ostream &os, const dataStructure ds) {
    switch (ds) {
    case dataStructure::ARRAY:
        return os << "ARRAY";
    case dataStructure::TREE:
        return os << "TREE";
        // omit default case to trigger compiler warning for missing cases
    };
    return os;
}

/**
	 * returns the model count which corresponds to the given id
	 *
	 * @param id
	 *      the id for which the model count should be returned
	 * @param tree
	 *      a pointer to the tree structure
	 * @param numVars
	 *      the number of variables in the bag
	 * @return
	 *      the model count
	 */
inline cl_double getCount(cl_long id, cl_long *tree, cl_long numVars) {
    cl_long nextId = 0;
    for (cl_long i = 0; i < numVars; i++) {
        nextId = ((cl_uint *)&(tree[nextId]))[(id >> (numVars - i - 1)) & 1];
        if (nextId == 0) {
            return 0.0;
        }
    }
    return *reinterpret_cast<cl_double *>(&tree[nextId]);
}

/**
	 * @return the time in millisecons since the epoch
	 */
inline long long int getTime() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

inline void printtreeType(treeType *tree, std::ostream &stream, size_t size, int depth = 0) {

    stream << std::string(depth, ' ') << "treeType ( " << tree->minId << " - " << tree->maxId << "): size=" << tree->size
           << " sol=" << tree->numSolutions << "\n";

    if (tree->elements != nullptr) {
        stream << std::string(depth, ' ') << "elements: "
               << "\n";
        for (cl_long i = tree->minId; i < tree->maxId; i++) {
            stream << std::string(depth, ' ') << "id: " << i << " # ";
            // ONLY FOR TREE format, not ARRAY!!!
            cl_double sol = getCount(i, tree->elements, (cl_long)size);
            // ONLY FOR THE ARRAY format:
            // cl_double sol = *reinterpret_cast <cl_double*>(&tree->elements[i - tree->minId]);
            stream << sol << "\n";
        }
    }
}

/// print information for a bag in the tree decomposition
inline void printbagType(bagType *bag, std::ostream &stream, int depth = 0) {
    depth += 2;
    stream << std::string(depth, ' ');
    stream << "bag(" << bag->id << "): bags= " << bag->bags << " , exp= " << bag->exponent
           << " , correction= " << bag->correction << "\n";
    stream << std::string(depth, ' ') << "var= [";
    for (auto a : bag->variables) {
        stream << a << ", ";
    }
    stream << "]"
           << "\n";
    if (!bag->edges.empty()) {
        stream << std::string(depth, ' ') << "edges to [";
        for (bagType *b : bag->edges) {
            stream << b->id << ", ";
        }
        stream << "]\n";
    }

    /*if (bag->solution != nullptr) {
			stream << "\n" << std::string(depth, ' ') << "solution: \n";
			printtreeType(bag->solution, stream, bag->variables.size(), depth);
		}*/
}
/// Generate a formatted stringoutput for a solved node with solutions
inline std::string solutiontable(bagType node, dataStructure solutionType) {
    std::ostringstream os;
    size_t var_count = node.variables.size();
    cl_double totalSol = 0;
    std::string underline = std::string(var_count * 3 + 12, '-');

    if (node.solution->elements != nullptr) {
        os << "id |";
        for (int i = 0; i < var_count; ++i) {
            os << " v" << node.variables[i];
        }
        os << " || n Sol\n";
        os << underline << "\n";

        // all ID-lines
        for (cl_long id = node.solution->minId; id < node.solution->maxId; id++) {
            os.width(3);
            os << id << "|";
            cl_long id_b = id;
            for (int v = 0; v < var_count; ++v) { // find binary rep. of id
                os << "  " << id_b % 2;
                id_b /= 2;
            }
            cl_double sol = -1.;

            if (solutionType == dataStructure::TREE) {
                sol = getCount(id, node.solution->elements, var_count) * pow(2, node.correction);
            } else {
                sol = *reinterpret_cast<cl_double *>(&node.solution->elements[id - node.solution->minId]) * pow(2, node.correction);
            }

            os << "   ||  ";
            os.width(3);
            totalSol += sol;
            os << sol << "    \n";
        }
        // underlined for total count of solutions
        os << underline << "\n";
        os.width(var_count * 3 + 10);
        os << "sum: " << totalSol;
    } else {
        os << "no solutions (nullptr)";
    }
    return os.str();
}

/// print a tree decomposition
inline void printtreedecType(treedecType *dec, std::ostream &stream) {

    stream << "bags: " << dec->numb << "\n";
    for (auto v : dec->bags)
        stream << v.id << " : " << v.variables
               << v.edges << "\n";
}

} // namespace gpusat
#endif //GPUSAT_GPUSAUTILS_H
