#pragma once

#include <iostream>
#include <string>

#define LogError(err) std::cerr << "ERROR - " << __FUNCTION__ << "() " << err << "\n";
#define LogDebug(dbg) std::cerr << "DEBUG - " << __FUNCTION__ << "() " << dbg << "\n";