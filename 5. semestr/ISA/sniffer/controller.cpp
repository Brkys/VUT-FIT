#include "controller.h"

Controller::Controller(int argc, char *argv[])
{
    setvbuf(stdout, nullptr, _IONBF, 0);
    parseParameters(argc, argv);
    if(!error_)
    {
        sniffer_ = new Sniffer(interface_);
        error_ = sniffer_->error();
    }
}

Controller::~Controller()
{
    delete sniffer_;
}

bool Controller::error() const
{
    return error_;
}

void Controller::parseParameters(int argc, char *argv[])
{
    if(argc != 3)
    {
        if(argc == 2 && (strcmp(argv[1], "-help") == 0))
        {
            help();
        }
        else
        {
            cerr << "Error: Unknown parameters. Type ./myripsniffer -help for help." << endl;
            error_ = true;
        }
    }
    else
    {
        if(strcmp(argv[1], "-i") == 0)
        {
            interface_ = argv[2];
        }
        else
        {
            cerr << "Error: Unknown parameters. Type ./myripsniffer -help for help." << endl;
            error_ = true;
        }
    }
}

void Controller::help()
{
    cout << "Program is used to sniff packets on interface. Start it with \n"
            "./myripsniffer -i <interface>, where:\n"
            "-i <interface> sets interface, which should be sniffed from.";
}
