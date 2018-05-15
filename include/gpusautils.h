#ifndef GPUSAT_GPUSAUTILS_H
#define GPUSAT_GPUSAUTILS_H

#include <queue>
#include <types.h>

namespace gpusat {
    class GPUSATUtils {
    public:

        /**
         * function to print the solution of a single bag in tree decomposition in human readable form
         *
         * @param numS              the number of assignments
         * @param numVariables      the number of variables
         * @param vars              arrray containing the variables
         * @param sol               array ontaining the number of models for each assignment
         */
        static void printSol(cl_long numS, std::vector<cl_long> vars, solType **sol, satformulaType &type, cl_long i);

        /**
         * reads a binary file from @path and retuns the contents in a string
         *
         * @param path      path to the file
         * @return          the file contents
         */
        static std::string readBinary(std::string path);
    };

    inline cl_long popcount(cl_long x) {
        x -= (x >> 1) & 0x5555555555555555;
        x = (x & 0x3333333333333333) + ((x >> 2) & 0x3333333333333333);
        return (((x + (x >> 4)) & 0x0f0f0f0f0f0f0f0f) * 0x0101010101010101) >> 56;
    }
}
#endif //GPUSAT_GPUSAUTILS_H
