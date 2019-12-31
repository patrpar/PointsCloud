#include "core.h"
#include <iostream>

ArgsParserProgram::ArgsParserProgram(int argc, char** argv)
{
    if (argc > 1) {

        args.reserve(argc);

        std::cout << "Input size: " << argc << std::endl;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            std::cout << arg << ": true" << std::endl;
            args.emplace_back(arg);
        }

        parse();
    }
    else {
        std::cout << "Input empty" << std::endl;
    }

}

void ArgsParserProgram::parse()
{
    for (int i = 1; i < args.size(); i++) {
        
        if (args[i] == "keyPoints") {
            params.printKeyPoints = true;
        }
        else if (args[i] == "matches") {
            params.printKeyPointsMatches = true;
        }
        else if (args[i] == "calibration") {
            params.loadCalibrationDataFromFile = true;
        }
        else if (args[i] == "loadPointsFromFile") {
            params.loadPointsFromFile = true;
        }
        else {
            std::cout << "Unexpected input :" << args[i] << std::endl;
        }
   }

}
