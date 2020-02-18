#include <visualisierung.h>
#include <iostream>
#include <fstream>
#include <types.h>
#include <sstream>
#include <gpusatutils.h>

namespace gpusat {

	void Visualisierung::visuout(std::string string, bool append) {
		if (!isEnabled()) return;

		std::ofstream stream(visufile, append ? std::ios_base::app : std::ios_base::out);

		if (stream.is_open()) {
			stream << string;
			stream.close();
		}
		else { std::cerr << "Failed to open file : " << visufile << " with " << errno << std::endl; }
	}


	void Visualisierung::setFile(std::string filename) {
		if (filename != "") {
			outputEnabled = true;
		}
		else outputEnabled = false;
		visufile = filename;
	}

}