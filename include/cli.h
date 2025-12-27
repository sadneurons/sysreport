#ifndef CLI_H
#define CLI_H

#include <string>
#include <vector>

// Version information
const std::string VERSION = "0.7.0";
const std::string PROGRAM_NAME = "sysreport";

// Helper functions
bool hasFlag(const std::vector<std::string>& args, const std::string& flag);
std::string getOptionValue(const std::vector<std::string>& args, const std::string& option);
void printHelp();
void printVersion();
void printError(const std::string& message);

#endif // CLI_H
