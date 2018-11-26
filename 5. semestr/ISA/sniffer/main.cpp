#include "controller.h"

int main(int argc, char *argv[])
{
    Controller controller(argc, argv);
    if(!controller.error())
        return -1;
    return 0;
}
