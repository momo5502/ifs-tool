#pragma once

#define NOMINMAX
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define _USE_MATH_DEFINES

#ifdef _WIN32
#include <windows.h>
#endif

#include <math.h>
#include <stdio.h>
#include <string.h>

#pragma warning(push)
#pragma warning(disable: 4244)
#include <map>
#include <list>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <fstream>
#include <random>
#include <utility>
#include <iostream>
#include <sstream>
#include <filesystem>

#pragma warning(pop)

using namespace std::literals;
