// Copyright(C) 2020 utahman https://github.com/utahman
// MIT License http://www.opensource.org/licenses/mit-license.php

#include "pch.h"
#include "Config.h"

Config g;


int main()
{
    winrt::init_apartment();
    g.Load(); // Loads config and creates objects

    while (g._run)
    {
        Sleep(100);
        g.updateSystemTime();

        if (_kbhit())
        {
            switch (std::tolower(_getch()))
            {
            case 'u': // Show GUI
                g._gui->Show();
                break;
            case 'o': // Reload configuration file
                g.Load();
                break;
            case 's': // Stop
                if (IDYES == MessageBox(NULL, "Stop?",
                    "Executor", MB_YESNO | MB_ICONQUESTION | MB_SYSTEMMODAL))
                {
                    g._run = false;
                }
                break;
            }
        }
    }

    g._broker->Stop();
    g._data->Stop();
    g._gui->Stop();
}

