#ifndef GPUSAT_TYPES_H_H
#define GPUSAT_TYPES_H_H

#define alloca __builtin_alloca

#include <CL/cl.hpp>
#include <CL/cl_platform.h>
#include <cmath>
#include <list>
#include <set>

struct d4_Type {
    cl_double x[4];

    d4_Type() {
        x[0] = 0.0;
        x[1] = 0.0;
        x[2] = 0.0;
        x[3] = 0.0;
    }

    d4_Type(double a, double b, double c, double d) {
        x[0] = a;
        x[1] = b;
        x[2] = c;
        x[3] = d;
    }

    d4_Type(double a) {
        x[0] = a;
        x[1] = 0.0;
        x[2] = 0.0;
        x[3] = 0.0;
    }

    d4_Type &operator=(double a) {
        x[0] = a;
        x[1] = 0.0;
        x[2] = 0.0;
        x[3] = 0.0;
        return *this;
    }

    d4_Type &operator=(d4_Type a) {
        x[0] = a.x[0];
        x[1] = a.x[1];
        x[2] = a.x[2];
        x[3] = a.x[3];
        return *this;
    }

    d4_Type &operator=(d4_Type *a) {
        x[0] = a->x[0];
        x[1] = a->x[1];
        x[2] = a->x[2];
        x[3] = a->x[3];
        return *this;
    }
};

#ifdef sType_Double
#define solType cl_double
#else
#define solType d4_Type
#endif

namespace gpusat {

    /// type for a bag in the tree decomposition
    struct bagType {
        cl_long numVars = 0;
        cl_long numEdges = 0;
        cl_long numSol = 0;
        cl_long *variables = nullptr;
        cl_long *edges = nullptr;
        solType *solution = nullptr;
    };

    /// type for saving a tree decomposition
    struct treedecType {
        cl_long numb = 0;
        cl_long numVars = 0;
        bagType *bags = nullptr;
    };

    /// type for preprocessing a tree decomposition
    struct preebagType {
        cl_long id = 0;
        cl_long numEdges = 0;
        cl_long numVariables = 0;
        cl_long *variables = nullptr;
        preebagType **edges = nullptr;
    };

    /// type for saving a tree decomposition
    struct preetreedecType {
        cl_long numb = 0;
        cl_long numVars = 0;
        preebagType *bags = nullptr;
    };

    bool compTreedType(const preebagType *a, const preebagType *b);

    /// type for saving the sat formula
    struct satformulaType {
        cl_long numclauses = 0;
        cl_long numVars = 0;
        cl_long totalNumVar = 0;
        cl_long *numVarsC = nullptr;
        cl_long *clauses = nullptr;
    };

    enum graphTypes {
        INCIDENCE, PRIMAL
    };

    enum precisionTypes {
        DOUBLE, D4
    };
}

#endif //GPUSAT_TYPES_H_H
