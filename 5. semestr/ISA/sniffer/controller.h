#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <string.h>
#include <iostream>
#include "sniffer.h"

using namespace std;

class Controller
{
public:
    Controller(int argc, char *argv[]);

    ~Controller();

    bool error() const;

private:
    void parseParameters(int argc, char *argv[]);

    void help();

private:
    bool error_;

    string interface_;

    Sniffer *sniffer_;
};

#endif // CONTROLLER_H
