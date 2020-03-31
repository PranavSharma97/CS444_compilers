#include <string>

// Union return type to hold possible Type values
class Token;

struct TCType {
  // Indicates the type:
  // 0: Primitive type
  // 1: Reference type
  // 2: Primitive Array type
  // 3: Reference Array type
  // 4: VOID
  // 5: String literal
  // 6: Null literal
  int type;
  std::string primitive;
  Token* reference;
};
