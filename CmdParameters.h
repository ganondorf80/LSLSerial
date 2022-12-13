#pragma once
#include <string>
#include <vector>

class CmdParameters
{
private:
	std::string strCmdLine;
	std::vector<std::string> vParameters;
	static std::string emptyString;
public:

	CmdParameters();
	void init(int argc, char* argv[]);
	bool has(std::string strValue);
	std::string getArg(std::string strValue);
	
};

