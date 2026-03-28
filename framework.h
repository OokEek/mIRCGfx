#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define NOMINMAX 1

// Windows Header Files
#include <windows.h>
#include <windowsx.h>
#include <Uxtheme.h>

#include <memory>
#include <vector>
#include <string>
#include <optional>
#include <filesystem>
#include <concepts>
#include <shared_mutex>

#include <gsl/gsl>

#include <TString.h>
#include <hashString.h>
