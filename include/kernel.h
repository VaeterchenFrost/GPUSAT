R"=====(

#if defined(cl_khr_fp64)
# pragma OPENCL EXTENSION cl_khr_fp64: enable
# pragma OPENCL EXTENSION cl_khr_int64_base_atomics : enable
# pragma OPENCL EXTENSION cl_khr_int64_extended_atomics : enable
#elif defined(cl_amd_fp64)
# pragma OPENCL EXTENSION cl_amd_fp64: enable
# pragma OPENCL EXTENSION cl_khr_int64_base_atomics : enable
# pragma OPENCL EXTENSION cl_khr_int64_extended_atomics : enable
#else
# error double precision is not supported
#endif

#if !defined(ARRAY_TYPE)


double getCount(long id, __global long *tree, long numVars) {
    ulong nextId = 0;
    for (ulong i = 0; i < numVars; i++) {
        nextId = ((__global
        uint *) &tree[nextId])[(id >> (numVars - i - 1)) & 1];
        if (nextId == 0) {
            return 0.0;
        }
    }
    return as_double(tree[nextId]);
}


void setCount(long id, __global long *tree, long numVars, __global long *treeSize, double value) {
    ulong nextId = 0;
    ulong val = 0;
    if (numVars == 0) {
        atom_add(treeSize, 1);
    }
    for (ulong i = 0; i < numVars; i++) {
        __global
        uint * lowVal = &((__global
        uint *) &(tree[nextId]))[(id >> (numVars - i - 1)) & 1];
        if (val == 0 && *lowVal == 0) {
            val = atom_add(treeSize, 1) + 1;
        }
        atomic_cmpxchg(lowVal, 0, val);
        if (*lowVal == val) {
            if (i < (numVars - 1)) {
                val = atom_add(treeSize, 1) + 1;
            }
        }
        nextId = *lowVal;
    }
    tree[nextId] = as_long(value);
}


__kernel void resize(long numVars, __global long *tree, __global double *solutions_old, __global long *treeSize, long startId, __global long *exponent) {
    long id = get_global_id(0);
    if (solutions_old[id] > 0) {
        setCount(id + startId, tree, numVars, treeSize, solutions_old[id]);
#if !defined(NO_EXP)
        atom_max(exponent, ilogb(solutions_old[id]));
#endif
    }
}


__kernel void combineTree(long numVars, __global long *tree, __global double *solutions_old, __global long *treeSize, long startId) {
    long id = get_global_id(0);
    double val = getCount(id + startId, solutions_old, numVars);
    if (val > 0) {
        setCount(id + startId, tree, numVars, treeSize, val);
    }
}

#endif


double solveIntroduce_(long numV, __global long *edge, long numVE, __global long *variables, __global long *edgeVariables, long minId, long maxId, __global double *weights, long id) {
    long otherId = 0;
    long a = 0, b = 0;
    double weight = 1.0;
    for (b = 0; b < numVE && a < numV; b++) {
        while ((variables[a] != edgeVariables[b])) {
            a++;
        }

        otherId = otherId | (((id >> a) & 1) << b);
        a++;
    };

    //weighted model count
    if (weights != 0) {
        for (b = 0, a = 0; a < numV; a++) {
            if (edgeVariables == 0 || (variables[a] != edgeVariables[b])) {
                weight *= weights[((id >> a) & 1) > 0 ? variables[a] * 2 : variables[a] * 2 + 1];
            }
            if (edgeVariables != 0 && (variables[a] == edgeVariables[b]) && (b < (numVE - 1))) {
                b++;
            }
        }
    }

    if (edge != 0 && otherId >= (minId) && otherId < (maxId)) {

#if !defined(ARRAY_TYPE)
        return getCount(otherId, edge, numVE) * weight;
#else
        return as_double(edge[otherId - (minId)]) * weight;
#endif

    } else if (edge == 0 && otherId >= (minId) && otherId < (maxId)) {
        return 0.0;
    } else {
        return -1.0;
    }
}


int checkBag(__global long *clauses, __global long *numVarsC, long numclauses, long id, long numV, __global long *variables) {
    long i, varNum = 0;
    long satC = 0, a, b;
    // iterate through all clauses
    for (i = 0; i < numclauses; i++) {
        satC = 0;
        // iterate through clause variables
        for (a = 0; a < numVarsC[i] && !satC; a++) {
            satC = 1;
            //check current variables
            for (b = 0; b < numV; b++) {
                // check if clause is satisfied
                if ((clauses[varNum + a] == variables[b]) ||
                    (clauses[varNum + a] == -variables[b])) {
                    satC = 0;
                    if (clauses[varNum + a] < 0) {
                        //clause contains negative var and var is assigned negative
                        if ((id & (1 << (b))) == 0) {
                            satC = 1;
                            break;
                        }
                    } else {
                        //clause contains positive var and var is assigned positive
                        if ((id & (1 << (b))) > 0) {
                            satC = 1;
                            break;
                        }
                    }
                }
            }
        }
        varNum += numVarsC[i];
        // we have an unsattisifed clause
        if (!satC) {
            return 0;
        }
    }
    return 1;
}


__kernel void solveJoin(__global long *solutions, __global long *edge1, __global long *edge2, __global long *variables, __global long *edgeVariables1, __global long *edgeVariables2, long numV, long numVE1, long numVE2, long minId1, long maxId1, long minId2, long maxId2, long startIDNode, long startIDEdge1, long startIDEdge2, __global double *weights, __global long *sols, double value, __global long *exponent) {
    long id = get_global_id(0);
    double tmp = -1, tmp_ = -1;
    double weight = 1;
    if (minId1 != -1) {
        // get solution count from first edge
        tmp = solveIntroduce_(numV, edge1, numVE1, variables, edgeVariables1, minId1, maxId1, weights, id);
    }
    if (minId2 != -1) {
        // get solution count from second edge
        tmp_ = solveIntroduce_(numV, edge2, numVE2, variables, edgeVariables2, minId2, maxId2, weights, id);
    }
    // weighted model count
    if (weights != 0) {
        for (long a = 0; a < numV; a++) {
            weight *= weights[((id >> a) & 1) > 0 ? variables[a] * 2 : variables[a] * 2 + 1];
        }
    }


    if (tmp_ >= 0.0 && tmp >= 0.0) {
        if (tmp_ > 0.0 && tmp > 0.0) {
            atom_add(sols, 1);
        }
#if !defined(NO_EXP)
        solutions[id - startIDNode] = as_long(tmp_ * tmp / value / weight);
#else
        solutions[id - startIDNode] = as_long(tmp_ * tmp / weight);
#endif
    }

        // we have some solutions in edge1
    else if (tmp >= 0.0) {
        double oldVal = as_double(solutions[id - startIDNode]);
        if (oldVal < 0) {
            if (tmp > 0) {
                atom_add(sols, 1);
            }
        } else if (oldVal > 0) {
            if (tmp == 0) {
                atom_sub(sols, 1);
            }
        }
        if (oldVal < 0) {
            oldVal = 1.0;
        }
        solutions[id - startIDNode] = as_long(tmp * oldVal / weight);
    }

        // we have some solutions in edge2
    else if (tmp_ >= 0.0) {
        double oldVal = as_double(solutions[id - startIDNode]);
        if (oldVal < 0) {
            if (tmp_ > 0) {
                atom_add(sols, 1);
            }
        } else if (oldVal > 0) {
            if (tmp_ == 0) {
                atom_sub(sols, 1);
            }
        }
        if (oldVal < 0) {
            oldVal = 1.0;
        }
#if !defined(NO_EXP)
        solutions[id - startIDNode] = as_long(tmp_ * oldVal / value);
#else
        solutions[id - startIDNode] = as_long(tmp_ * oldVal);
#endif
    }
#if defined(ARRAY_TYPE) && !defined(NO_EXP)
    atom_max(exponent, ilogb(as_double(solutions[id - startIDNode])));
#endif

}

/**
 * Operation to solve an Introduce
 *
 * @param clauses
 *      the clauses in the sat formula
 * @param numVarsC
 *      the number of variables for each clause
 * @param numclauses
 *      the number of clauses
 * @param numV
 *      the number of variables in the current bag
 * @param edge
 *      the number of models for each assignment of the last bags
 * @param numVE
 *      the number of variables of the last bag
 * @param variables
 *      the ids of the variables in the current bag
 * @param edgeVariables
 *      the ids of the variables in the last bag
 * @param minId
 *      the start id of the last bag
 * @param maxId
 *      the end id of the last bag
 * @param weights
 *      the variables weights for weighted model counting
 * @param id
 *      the id for which the introduce should be solved
 * @return
 *      the model count
 */
double solveIntroduceF(__global long *clauses, __global long *numVarsC, long numclauses, long numV, __global long *edge, long numVE, __global long *variables, __global long *edgeVariables, long minId, long maxId, __global double *weights, long id) {
    double tmp;
    if (edge != 0) {
        // get solutions count edge
        tmp = solveIntroduce_(numV, edge, numVE, variables, edgeVariables, minId, maxId, weights, id);
    } else {
        // no edge - solve leaf
        tmp = 1.0;

        //weighted model count
        if (weights != 0) {
            for (long i = 0; i < numV; i++) {
                tmp *= weights[((id >> i) & 1) > 0 ? variables[i] * 2 : variables[i] * 2 + 1];
            }
        }
    }
    if (tmp > 0.0) {
        // check if assignment satisfies the given clauses
        int sat = checkBag(clauses, numVarsC, numclauses, id, numV, variables);
        if (sat != 1) {
            return 0.0;
        } else {
            return tmp;
        }
    } else {
        return 0.0;
    }
}

/**
 * Operation to solve a Introduce and Forget node in the decomposition.
 *
 * @param solsF 0
 *      the number of models for each assignment
 * @param varsF 1
 *      the variables after the forget operation
 * @param solsE 2
 *      the solutions from the last node
 * @param numVE 3
 *      number of variables in the last node
 * @param varsE 4
 *      the variables from the alst oepration
 * @param combinations 5
 *      the number of assignments for which we have to collect the model counts
 * @param numVF 6
 *      number of variables after the forget operation
 * @param minIdE 7
 *      start id of the chunk from the last node
 * @param maxIdE 8
 *      end id of the chunk from the last node
 * @param startIDF 9
 *      start id of the chung from the current node
 * @param sols 10
  *     the number of assignments which lead to a solution
 * @param numVI 11
 *      number of variables after the introduce
 * @param varsI 12
 *      the variables after the introduce
 * @param clauses 13
 *      the clauses which only contain variables from the introduce operation
 * @param numVarsC 14
 *      the number of variables per clause
 * @param numclauses 15
 *      the number of clauses
 * @param weights 16
 *      the variables weights for weighted model counting
 * @param exponent 17
  *     the max exponent of this run
 * @param value 18
  *     correction value for the exponents
 */
__kernel void solveIntroduceForget(__global long *solsF, __global long *varsF, __global long *solsE, long numVE, __global long *varsE, long combinations, long numVF, long minIdE, long maxIdE, long startIDF, long startIDE, __global long *sols, long numVI, __global long *varsI, __global long *clauses, __global long *numVarsC, long numclauses, __global double *weights, __global long *exponent, double value) {
    long id = get_global_id(0);
    if (numVI != numVF) {
        double tmp = 0;
        long templateId = 0;
        // generate templateId
        for (long i = 0, a = 0; i < numVI && a < numVF; i++) {
            if (varsI[i] == varsF[a]) {
                templateId = templateId | (((id >> a) & 1) << i);
                a++;
            }
        }

        // iterate through all corresponding edge solutions
        for (long i = 0; i < combinations; i++) {
            long b = 0, otherId = templateId;
            for (long a = 0; a < numVI; a++) {
                if (b >= numVF || varsI[a] != varsF[b]) {
                    otherId = otherId | (((i >> (a - b)) & 1) << a);
                } else {
                    b++;
                }
            }
            // get solution count of the corresponding assignment in the edge
            tmp += solveIntroduceF(clauses, numVarsC, numclauses, numVI, solsE, numVE, varsI, varsE, minIdE, maxIdE, weights, otherId);
        }
#if !defined(ARRAY_TYPE)
        if (tmp > 0) {
            double last = getCount(id, solsF, numVF);
#if !defined(NO_EXP)
            setCount(id, solsF, numVF, sols, (tmp / value + last));
            atom_max(exponent, ilogb((tmp / value + last)));
#else
            setCount(id, solsF, numVF, sols, (tmp + last));
#endif
        }
#else
        if (tmp > 0) {
            double last=as_double(solsF[id - (startIDF)]);
            *sols += 1;
#if !defined(NO_EXP)
            solsF[id - (startIDF)] = as_long(tmp / value + last);
            atom_max(exponent, ilogb(tmp / value + last));
#else
            solsF[id - (startIDF)] = as_long(tmp + last);
#endif
        }
#endif
    } else {
        // no forget variables, only introduce
        double tmp = solveIntroduceF(clauses, numVarsC, numclauses, numVI, solsE, numVE, varsI, varsE, minIdE, maxIdE, weights, id);
#if !defined(ARRAY_TYPE)
        if (tmp > 0) {
            double last = getCount(id, solsF, numVF);
#if !defined(NO_EXP)
            setCount(id, solsF, numVF, sols, (tmp / value + last));
            atom_max(exponent, ilogb((tmp / value + last)));
#else
            setCount(id, solsF, numVF, sols, (tmp + last));
#endif
        }
#else
        if (tmp > 0) {
            double last=as_double(solsF[id - (startIDF)]);
            *sols += 1;
#if !defined(NO_EXP)
            solsF[id - (startIDF)] = as_long(tmp / value + last);
            atom_max(exponent, ilogb(tmp / value + last));
#else
            solsF[id - (startIDF)] = as_long(tmp + last);
#endif
        }
#endif
    }
}

)=====";
