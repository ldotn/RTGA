/* final_all_pp.cpp

   Simple libftdi-cpp usage

   This program is distributed under the GPL, version 2
*/

#include "ftdi.hpp"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <thread>
using namespace Ftdi;
using namespace std;

int main(int argc, char** argv)
{
    // Show help
    if (argc > 1)
    {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
        {
            std::cout << "Usage: " << argv[0] << " [-v VENDOR_ID] [-p PRODUCT_ID]" << std::endl;
            return EXIT_SUCCESS;
        }
    }

    // Parse args
    int vid = 0x0403, pid = 0x6014, tmp = 0;
    for (int i = 0; i < (argc - 1); i++)
    {
        if (strcmp(argv[i], "-v") == 0)
            if ((tmp = strtol(argv[++i], 0, 16)) >= 0)
                vid = tmp;

        if (strcmp(argv[i], "-p") == 0)
            if ((tmp = strtol(argv[++i], 0, 16)) >= 0)
                pid = tmp;
    }

    // Print header
    std::cout << std::hex << std::showbase
        << "Found devices ( VID: " << vid << ", PID: " << pid << " )"
        << std::endl
        << "------------------------------------------------"
        << std::endl << std::dec;

    // Print whole list
    Context context;
    List* list = List::find_all(context, vid, pid);
    for (List::iterator it = list->begin(); it != list->end(); it++)
    {
        std::cout << "FTDI (" << &*it << "): "
            << it->vendor() << ", "
            << it->description() << ", "
            << it->serial();

        // Open test
        if (it->open() == 0)
            std::cout << " (Open OK)";
        else
            std::cout << " (Open FAILED)";

        unsigned char buffer[1];

        // Send header
        buffer[0] = 16;
        it->write(buffer, 1);

        for(int i = 0;;i++)
        {
            unsigned char val = i % 10;
            cout << int(val) << endl;

            buffer[0] = val;
            it->write(buffer, 1);
            unsigned char response = it->read(buffer, 1);
            
            if (val != response)
            {
                cout << int(val) << " != " << int(response) << endl;
            }
        }


        it->close();

        std::cout << std::endl;

    }

    delete list;

    return EXIT_SUCCESS;
}
