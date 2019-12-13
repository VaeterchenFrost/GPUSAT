#ifndef GPUSAT_GPUSAUTILS_H
#define GPUSAT_GPUSAUTILS_H
#if defined __CYGWIN__ || defined __MINGW32__
#define alloca __builtin_alloca
#endif

#include <CL/cl_platform.h>
#include <queue>
#include <chrono>
#include <numeric>

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
	inline void printtreeType(treeType* tree, std::ostream& stream, cl_long size) {
		stream << "treeType ( " << tree->minId << " - " << tree->maxId << "): size=" << tree->size
			<< " sol=" << tree->numSolutions << "\n";

		if (tree->elements != nullptr) {
			stream << "elements: " << "\n";
			for (cl_long i = tree->minId; i < tree->maxId; i++) {
				stream << "id: " << i << " # ";
				// ONLY FOR TREE format, not ARRAY!!!
				stream << getCount(i, tree->elements, size) << "\n";
				
			}
		}
	};

	/// print information for a bag in the tree decomposition
	inline void printbagType(bagType* bag, std::ostream& stream) {
		stream << "bagType ( " << bag->id << "): bags= " << bag->bags << " , exp= " << bag->exponent
			<< " , correction= " << bag->correction << "\n";
		stream << "var= [";
		for (auto a : bag->variables) {
			stream << a << ", ";
		}
		stream << "]" << "\nbags=\n";
		for (auto b : bag->edges) {
			printbagType(b, stream);
		}
		if (bag->solution != nullptr) {
			stream << "\nsolution: \n";
			printtreeType(bag->solution, stream, bag->variables.size());
		}

	};

	/// print a tree decomposition
	inline void printtreedecType(treedecType* dec, std::ostream& stream) {
		stream << "\nprinting treedec with numb:" << dec->numb << ", numVars:" << dec->numVars <<
			", width:" << dec->width << "\nbags:\n";
		for (auto bag : dec->bags) {
			printbagType(&bag, stream);
		}
		stream << "\n===treedec " << dec->numb << "===\n";
	};
}
#endif //GPUSAT_GPUSAUTILS_H
