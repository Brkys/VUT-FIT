#include "controller.h"

Controller::Controller(int argc, char *argv[])
{
    setvbuf(stdout, nullptr, _IONBF, 0);
    parseParameters(argc, argv);
    if(!error_)
    {
        smuggler_ = new Smuggler(interface_, ipv6_, metric_, nextHop_, routerTag_, prefixLenght_);
        error_ = smuggler_->error();
    }
}

bool Controller::error() const
{
    return error_;
}

void Controller::parseParameters(int argc, char *argv[])
{
    if(argc < 5 || argc > 11)
    {
        if(argc == 2 && strcmp(argv[1], "-help") == 0)
        {
            help();
            return;
        }
        cout << "Error: Unknown parameters. Type ./myripresponse -help for help." << endl;
        error_ = true;
        return;
    }
    for(int i = 1; i < argc; i +=2 )
    {
        if(strcmp(argv[i], "-i") == 0)
        {
            if(argv[i+1] != nullptr)
                interface_ = argv[i+1];
            else
            {
                error_ = true;
                break;
            }
        }
        else if(strcmp(argv[i], "-r") == 0)
        {
            if(argv[i+1] != nullptr)
            {
                string tmp = argv[i+1];
                size_t pos = tmp.find_last_of("/");
                if(pos != string::npos)
                {
                    string substr = tmp.substr(pos+1, tmp.length()-1);
                    if(isnumber(substr.c_str()))
                    {
                        prefixLenght_ = static_cast<u_int>(stoi(tmp.substr(pos+1, tmp.length()-1)));
                        if(prefixLenght_ < 16 && prefixLenght_ > 128)
                        {
                            cerr << "Error: Prefix has wrong value.";
                            error_ = true;
                            break;
                        }
                    }
                    else
                    {
                        cerr << "Error: Prefix has wrong value.";
                        error_ = true;
                        break;
                    }
                    ipv6_ = tmp.substr(0, pos);
                }
                else
                {
                    cerr << "Error: Missing prefix after IP!" << endl;
                    error_ = true;
                    break;
                }
            }
            else
            {
                error_ = true;
                break;
            }
        }
        else if(strcmp(argv[i], "-m") == 0)
        {
            if(argv[i+1] != nullptr)
            {
                if(isnumber(argv[i+1]))
                {
                    metric_ = static_cast<u_int>(stoi(argv[i+1]));
                    if(metric_ > 16)
                    {
                        cerr << "Error: Metric has wrong value!" << endl;
                        error_ = true;
                        break;
                    }
                }
                else
                {
                    error_ = true;
                    break;
                }
            }
            else
            {
                error_ = true;
                break;
            }
        }
        else if(strcmp(argv[i], "-n") == 0)
        {
            if(argv[i+1] != nullptr)
            {
                nextHop_ = argv[i+1];
            }
            else
            {
                error_ = true;
                break;
            }
        }
        else if(strcmp(argv[i], "-t") == 0)
        {
            if(argv[i+1] != nullptr)
            {
                if(isnumber(argv[i+1]))
                {
                    routerTag_ = static_cast<u_int>(stoi(argv[i+1]));
                    if(routerTag_ > 65535)
                    {
                        cerr << "Error: Router tag has wrong value!" << endl;
                        error_ = true;
                        break;
                    }
                }
                else
                {
                    error_ = true;
                    break;
                }
            }
            else
            {
                error_ = true;
                break;
            }
        }
    }
    if(interface_.empty() || ipv6_.empty() || !prefixLenght_)
    {
        cerr << "Error: Missing one of non optional arguments.";
        error_ = true;
    }
}

bool Controller::isnumber(const char *array)
{
    for(unsigned int i = 0; i < string(array).length(); i++)
    {
        if(!isdigit(array[i]))
            return false;
    }
    return true;
}

void Controller::help()
{
    cout << "This application is made to send false RIPng response message."
            "\nIt's launch format is:"
            "\n./myripresponse -i <interface> -r <IPv6>/[16-128] {-n <IPv6>} {-m [0-16]} {-t [0-65535]}."
            "\n -i <interface> - interface where is response sended"
            "\n -r <IPv6>/[16-128] - IPv6 and prefix of false network"
            "\n -n <IPv6> - next hop"
            "\n -m [0-16] - metrics"
            "\n -t [0-65535] - router tag"
            "\nwhere:"
            "\n\t<x> - single value"
            "\n\t[x-y] - indicates range from x to y"
            "\n\t{x} - optional parameter x"
            "\nLaunch without optional parameters starts application with implicit -n as ::, -m as 1 and -t as 1" << endl;
}
