#include "CmdParameters.h"

std::string CmdParameters::emptyString;

CmdParameters::CmdParameters() {};

void CmdParameters::init(int argc, char* argv[]) {
	strCmdLine = "";
	vParameters.clear();

	for (int n = 1; n < argc; n++) {
		strCmdLine += std::string(argv[n]);
		vParameters.push_back(argv[n]);
		if (n < argc) strCmdLine += " ";
	};
};

bool CmdParameters::has(std::string strValue) {
	for (int n = 0; n < vParameters.size(); n++) {
		if (vParameters[n].compare(strValue) == 0) return true;
	};
	return false;
};


std::string CmdParameters::getArg(std::string strValue) {
	for (int n = 0; n < vParameters.size(); n++) {
		if ((vParameters[n].compare(strValue) == 0) && (n < (vParameters.size() - 1))) return vParameters[n + 1];
	};
	return emptyString;
};
