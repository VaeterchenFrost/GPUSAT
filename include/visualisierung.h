#ifndef GPUSAT_VISUALISIERUNG_H
#define GPUSAT_VISUALISIERUNG_H

#include <string>
// #include <types.h>
// #include <map>

namespace gpusat {

	class Visualisierung {


		Visualisierung(std::string filename = "visugpusat.json") {
			setFile(filename);
		}


		std::string getFilename() {
			return visufile;
		}

		bool isEnabled() {
			return outputEnabled;
		}

	private:
		bool outputEnabled = false;

		std::string visufile;

		void visuout(std::string string, bool append = true);
		void setFile(std::string filename);
	};
}

#endif