#include <iostream>
#include "controller.h"

int main(int argc, char *argv[])
{
    Controller controller(argc, argv);
    return -controller.error();
}
