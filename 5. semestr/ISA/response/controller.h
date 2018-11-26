#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <string.h>
#include <bits/stdc++.h>

#include "smuggler.h"

using namespace std;

class Controller
{
public:
    Controller(int argc, char *argv[]);

    bool error() const;

private:
    //! Function checks and parses parameters and sets up important variables
    void parseParameters(int argc, char *argv[]);

    //! Returns true, if array contains number only, otherwise false
    bool isnumber(const char *array);

    //! Prints help
    void help();
private:
    bool error_ = false;

    string interface_;

    string ipv6_;

    u_int metric_ = 1;

    string nextHop_ = "::";

    u_int routerTag_ = 1;

    u_int prefixLenght_ = 0;

    Smuggler *smuggler_;
};

#endif // CONTROLLER_H
