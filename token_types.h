#pragma once

enum TokenType{
  COMMENT = 0,
  ASSIGNMENT_OP = 1,
  INFIX_OP = 2,
  PRE_POST_FIX_OP = 3,
  PREFIX_OP = 4,
  T_LEFT_ROUND_BRACKET = 5,
  T_RIGHT_ROUND_BRACKET = 6,
  T_LEFT_SQUARE_BRACKET = 7,
  T_RIGHT_SQUARE_BRACKET = 8,
  T_LEFT_CURLY_BRACKET = 9,
  T_RIGHT_CURLY_BRACKET = 10,
  T_DOT = 11,
  T_COMMA = 12,
  T_QUESTION = 13,
  T_COLON = 14,
  T_SEMICOLON = 15,
  INT_LITERAL = 16,
  FLOAT_LITERAL = 17,
  CHAR_LITERAL = 18,
  STRING_LITERAL = 19,
  BOOL_LITERAL = 20,
  NULL_LITERAL = 21,
  BASIC_TYPE = 22,
  VOID = 23,
  MODIFIER = 24,
  KEYWORDS = 25,
  T_IDENTIFIER = 26,
  T_STAR = 27,
  T_MINUS = 28,
  T_PLUS = 29,

  // New tokens for parser
  T_IF = 30,
  T_ELSE = 31,
  T_SWITCH = 32,
  T_BOOLEAN = 33,
  T_BYTE = 34,
  T_SHORT = 35,
  T_INT = 36,
  T_LONG = 37,
  T_CHAR = 38,
  T_FLOAT = 39,
  T_DOUBLE = 40,
  T_CASE = 41,
  T_DEFAULT = 42,
  T_WHILE = 43,
  T_DO = 44,
  T_FOR = 45,
  T_BREAK = 46,
  T_CONTINUE = 47,
  T_RETURN = 48,
  T_THROW = 49,
  T_SYNCHRONIZED = 50,
  T_TRY = 51,
  T_PACKAGE = 52,
  T_IMPORT = 53,
  T_CLASS = 54,
  T_CATCH = 55,
  T_FINALLY = 56,
  T_THIS = 57,
  T_NEW = 58,
  T_SUPER = 59,
  T_PLUS_PLUS = 60,
  T_MINUS_MINUS = 61,
  T_TILDE = 62,
  T_NOT = 63,
  T_PERCENT = 64,
  T_LESS_LESS = 65,
  T_GREATER_GREATER = 66,
  T_GREATER_GREATER_GREATER = 67,
  T_LESS = 68,
  T_GREATER = 69,
  T_LESS_EQUAL = 70,
  T_GREATER_EQUAL = 71,
  T_EXTENDS = 72,
  T_IMPLEMENTS = 73,
  T_INSTANCE_OF = 74,
  T_EQUAL_EQUAL = 75,
  T_NOT_EQUAL = 76,
  T_AND = 77,
  T_EXP = 78,
  T_OR = 79,
  T_OR_OR = 80,
  T_AND_AND = 81,
  T_EQUAL = 82,
  T_STAR_EQUAL = 83,
  T_SLASH_EQUAL = 84,
  T_PERCENT_EQUAL = 85,
  T_PLUS_EQUAL = 86,
  T_MINUS_EQUAL = 87,
  T_LESS_LESS_EQUAL = 88,
  T_GREATER_GREATER_EQUAL = 89,
  T_GREATER_GREATER_GREATER_EQUAL = 90,
  T_AND_EQUAL = 91,
  T_EXP_EQUAL = 92,
  T_OR_EQUAL = 93,
  T_SLASH = 94,
  T_PUBLIC = 95,
  T_PROTECTED = 96,
  T_PRIVATE = 97,
  T_STATIC = 98,
  T_ABSTRACT = 99,
  T_FINAL = 100,
  T_NATIVE = 101,
  T_TRANSIENT = 102,
  T_VOLATILE = 103,
  T_STRICTFP = 104,
  T_INTERFACE = 105,
  T_THROWS = 106,
  T_BOF = 107,
  T_EOF = 108,
  
  // Failure type
  TOKEN_FAILURE = 1000,
  Goal = 1001,
  Literal = 1002,
  Type = 1003,
  PrimitiveType = 1004,
  NumericType = 1005,
  IntegralType = 1006,
  FloatingPointType = 1007,
  ReferenceType = 1008,
  ClassOrInterfaceType = 1009,
  ClassType = 1010,
  InterfaceType = 1011,
  ArrayType = 1012,
  Name = 1013,
  SimpleName = 1014,
  QualifiedName = 1015,
  CompilationUnit = 1016,
  ImportDeclarations = 1017,
  TypeDeclarations = 1018,
  PackageDeclaration = 1019,
  ImportDeclaration = 1020,
  SingleTypeImportDeclaration = 1021,
  TypeImportOnDemandDeclaration = 1022,
  TypeDeclaration = 1023,
  Modifiers = 1024,
  Modifier = 1025,
  ClassDeclaration = 1026,
  Super = 1027,
  Interfaces = 1028,
  InterfaceTypeList = 1029,
  ClassBody = 1030,
  ClassBodyDeclarations = 1031,
  ClassBodyDeclaration = 1032,
  ClassMemberDeclaration = 1033,
  FieldDeclaration = 1034,
  VariableDeclarators = 1035,
  VariableDeclarator = 1036,
  VariableDeclaratorId = 1037,
  VariableInitializer = 1038,
  MethodDeclaration = 1039,
  MethodHeader = 1040,
  MethodDeclarator = 1041,
  FormalParameterList = 1042,
  FormalParameter = 1043,
  Throws = 1044,
  ClassTypeList = 1045,
  MethodBody = 1046,
  StaticInitializer = 1047,
  ConstructorDeclaration = 1048,
  ConstructorDeclarator = 1049,
  ConstructorBody = 1050,
  ExplicitConstructorInvocation = 1051,
  InterfaceDeclaration = 1052,
  ExtendsInterfaces = 1053,
  InterfaceBody = 1054,
  InterfaceMemberDeclarations = 1055,
  InterfaceMemberDeclaration = 1056,
  ConstantDeclaration = 1057,
  AbstractMethodDeclaration = 1058,
  ArrayInitializer = 1059,
  VariableInitializers = 1060,
  Block = 1061,
  BlockStatements = 1062,
  BlockStatement = 1063,
  LocalVariableDeclarationStatement = 1064,
  LocalVariableDeclaration = 1065,
  Statement = 1066,
  StatementNoShortIf = 1067,
  StatementWithoutTrailingSubstatement = 1068,
  EmptyStatement = 1069,
  LabeledStatement = 1070,
  LabeledStatementNoShortIf = 1071,
  ExpressionStatement = 1072,
  StatementExpression = 1073,
  IfThenStatement = 1074,
  IfThenElseStatement = 1075,
  IfThenElseStatementNoShortIf = 1076,
  SwitchStatement = 1077,
  SwitchBlock = 1078,
  SwitchBlockStatementGroups = 1079,
  SwitchBlockStatementGroup = 1080,
  SwitchLabels = 1081,
  SwitchLabel = 1082,
  WhileStatement = 1083,
  WhileStatementNoShortIf = 1084,
  DoStatement = 1085,
  ForStatement = 1086,
  ForStatementNoShortIf = 1087,
  ForInit = 1088,
  ForUpdate = 1089,
  StatementExpressionList = 1090,
  BreakStatement = 1091,
  ContinueStatement = 1092,
  ReturnStatement = 1093,
  ThrowStatement = 1094,
  SynchronizedStatement = 1095,
  TryStatement = 1096,
  Catches = 1097,
  CatchClause = 1098,
  Finally = 1099,
  Primary = 1100,
  PrimaryNoNewArray = 1101,
  ClassInstanceCreationExpression = 1102,
  ArgumentList = 1103,
  ArrayCreationExpression = 1104,
  DimExprs = 1105,
  DimExpr = 1106,
  Dims = 1107,
  FieldAccess = 1108,
  MethodInvocation = 1109,
  ArrayAccess = 1110,
  PostfixExpression = 1111,
  PostIncrementExpression = 1112,
  PostDecrementExpression = 1113,
  UnaryExpression = 1114,
  PreIncrementExpression = 1115,
  PreDecrementExpression = 1116,
  UnaryExpressionNotPlusMinus = 1117,
  CastExpression = 1118,
  MultiplicativeExpression = 1119,
  AdditiveExpression = 1120,
  ShiftExpression = 1121,
  RelationalExpression = 1122,
  EqualityExpression = 1123,
  AndExpression = 1124,
  ExclusiveOrExpression = 1125,
  InclusiveOrExpression = 1126,
  ConditionalAndExpression = 1127,
  ConditionalOrExpression = 1128,
  ConditionalExpression = 1129,
  AssignmentExpression = 1130,
  Assignment = 1131,
  LeftHandSide = 1132,
  AssignmentOperator = 1133,
  Expression = 1134,
  ConstantExpression = 1135
};
