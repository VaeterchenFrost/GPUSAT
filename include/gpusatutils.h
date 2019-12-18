#ifndef GPUSAT_GPUSAUTILS_H
#define GPUSAT_GPUSAUTILS_H
#if defined __CYGWIN__ || defined __MINGW32__
#define alloca __builtin_alloca
#endif

#include <CL/cl_platform.h>
#include <queue>
#include <chrono>
#include <numeric>
#include <string>
#include <fstream>
namespace gpusat {
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
	inline cl_double getCount(cl_long id, cl_long* tree, cl_long numVars) {
		cl_long nextId = 0;
		for (cl_long i = 0; i < numVars; i++) {
			nextId = ((cl_uint*)&(tree[nextId]))[(id >> (numVars - i - 1)) & 1];
			if (nextId == 0) {
				return 0.0;
			}
		}
		return *reinterpret_cast <cl_double*>(&tree[nextId]);
	}

	/**
	 * @return the time in millisecons since the epoch
	 */
	inline long long int getTime() {
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	/// print information from a treeType
	inline void printtreeType(treeType* tree, std::ostream& stream, cl_long size, int depth = 0) {
		stream << std::string(depth, ' ') << "treeType ( " << tree->minId << " - " << tree->maxId << "): size=" << tree->size
			<< " sol=" << tree->numSolutions << "\n";

		if (tree->elements != nullptr) {
			stream << std::string(depth, ' ') << "elements: " << "\n";
			for (cl_long i = tree->minId; i < tree->maxId; i++) {
				stream << std::string(depth, ' ') << "id: " << i << " # ";
				// ONLY FOR TREE format, not ARRAY!!!
				cl_double sol = getCount(i, tree->elements, size);
				// ONLY FOR THE ARRAY format:
				// cl_double sol = *reinterpret_cast <cl_double*>(&tree->elements[i - tree->minId]);
				stream << sol << "\n";
			}
		}
	}

	/// print information for a bag in the tree decomposition
	inline void printbagType(bagType* bag, std::ostream& stream, int depth = 0) {
		depth += 2;
		stream << std::string(depth, ' ');
		stream << "bag(" << bag->id << "): bags= " << bag->bags << " , exp= " << bag->exponent
			<< " , correction= " << bag->correction << "\n";
		stream << std::string(depth, ' ') << "var= [";
		for (auto a : bag->variables) {
			stream << a << ", ";
		}
		stream << "]" << "\n";
		if (!bag->edges.empty()) {
			stream << std::string(depth, ' ') << "edges to [";
			for (bagType* b : bag->edges) {
				stream << b->id << ", ";
			}
			stream << "]\n";
		}
		
		if (bag->solution != nullptr) {
			stream << "\n" << std::string(depth, ' ') << "solution: \n";
			printtreeType(bag->solution, stream, bag->variables.size(), depth);
		}

	}

	/// print a tree decomposition
	inline void printtreedecType(treedecType* dec, std::ostream& stream) {
		stream << "\nprinting treedec with numb:" << dec->numb << ", numVars:" << dec->numVars <<
			", width:" << dec->width << "\nbags:\n";
		for (auto bag : dec->bags) {
			printbagType(&bag, stream);
		}
		stream << "^^^ treedec " << dec->numb << "^^^\n";
	}

	inline void graphout(std::string filename, std::string string) {
		std::ofstream stream(filename, std::ios_base::app);
		if (stream.is_open()) {
			stream << string;
			stream.close();
		}
		else { std::cerr << "Failed to open file : " << filename << " with " << errno << std::endl; }
	}

	inline void graphNode(std::string filename, int id, std::string label) {
		graphout(filename,
			"node\n"
			"[\n id " + std::to_string(id) + "\n"
			"label \"" + label + "\"\n"
			"]\n"
		);
	}

}
#endif //GPUSAT_GPUSAUTILS_H
