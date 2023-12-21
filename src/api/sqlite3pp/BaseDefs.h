#pragma once

#ifdef _WIN32
#  define SQLITE3PP_EXPORT __declspec(dllexport)
#else
#  define SQLITE3PP_EXPORT
#endif

// Forward declaration of internal types
struct sqlite3;
struct sqlite3_stmt;
