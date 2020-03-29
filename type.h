#include <string>
#include "token.h"

// Union return type to hold possible Type values

struct Type {
  // Indicates the type:
  // 0: Primitive type
  // 1: Reference type
  // 2: Primitive Array type
  // 3: Reference Array type
  // 4: VOID
  // 5: String literal
  // 6: Null literal
  int type;
  string primitive;
  Token* reference;
}
