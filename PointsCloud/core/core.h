#pragma once

#include <string>
#include <vector>

struct ParamsProgram {
    bool printKeyPoints = false;
    bool printKeyPointsMatches = false;
    bool loadCalibrationDataFromFile = true;
    bool loadPointsFromFile = false;
};

class ArgsParserProgram {
public:
    ParamsProgram params;
private:
    std::vector<std::string> args;
public:

    ArgsParserProgram(int argc, char** argv);
    void parse();
};
