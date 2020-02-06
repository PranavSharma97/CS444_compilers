#pragma once

#define ESC 27
#define RED() printf("%c[91m",ESC)
#define YELLOW() printf("%c[93m",ESC)
#define PURPLE() printf("%c[95m",ESC)
#define CYAN() printf("%c[96m",ESC)
#define DEFAULT() printf("%c[39;49m",ESC)
