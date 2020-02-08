#ifndef DEBUG
#define DEBUG_MSG(str) do { } while( false )
#else
#define DEBUG_MSG(str) do { std::cout << str << std::endl; } while ( false )
#endif
