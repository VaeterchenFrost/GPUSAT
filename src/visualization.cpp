#include <visualization.h>
#include <iostream>
#include <fstream>
#include <types.h>
#include <sstream>
#include <gpusatutils.h>

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

TD-TIMEELINE:
TIMELINE = [(0,), (1,),
                (2, ([['id', 'v1', 'v2', 'n Sol'], [0, 0, 0, 0], [1, 1, 0, 1],
                      [2, 0, 1, 1], [3, 1, 1, 2]], 'sol bag 2', 'sum: 4', True)),
                (3,),
                (4, ([["id", "v2", "v8", "n Sol"], [0, 0, 0, 1], [1, 1, 0, 2],
                      [2, 0, 1, 1], [3, 1, 1, 1]], "sol bag 4", "sum: 5", True)),
                (3, ([["id", "v2", "v4", "n Sol"],
                      [0, 0, 0, 1], [1, 1, 0, 2], [2, 0, 1, 2],
                      [3, 1, 1, 3]], "sol bag 3", "sum: 8", True)),
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
EDGELIST = [[1, [1, 4, 6]], [2, [1, -5]], [3, [-1, 7]], [4, [2, 3]], [5, [2, 5]],
                [6, [2, -6]], [7, [3, -8]], [8, [4, -8]], [9, [-4, 6]], [10, [-4, 7]]]
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

	void Visualization::visuout(std::string string, bool append) {
		if (!isEnabled()) return;

		std::ofstream stream(visufile, append ? std::ios_base::app : std::ios_base::out);

		if (stream.is_open()) {
			stream << string;
			stream.close();
		}
		else { std::cerr << "Failed to open file : " << visufile << " with " << errno << std::endl; }
	}


	void Visualization::setFile(std::string filename) {
		if (filename != "") {
			outputEnabled = true;
		}
		else outputEnabled = false;
		visufile = filename;
	}

}