// Copyright(C) 2020 utahman https://github.com/utahman
// MIT License http://www.opensource.org/licenses/mit-license.php

#pragma once

void MSecToTimeGlobal(int glmsec, int& hour, int& min, int& sec, int& msec);
std::string* format(const char* fmt_str, ...);
std::string sformat(const char* fmt_str, ...);
