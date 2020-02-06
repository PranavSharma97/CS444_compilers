#include "token.h"

Token::Token():
  m_type(TokenType::TOKEN_EMPTY)
{
  m_rule = -1;
  m_display_name = "TOKEN_EMPTY";
}

Token::Token(TokenType type, std::string lex):
  m_type(type),
  m_lex(lex)
{
  m_rule = -1;
  // Determine the display name
  int type_int = static_cast<int>(type);
  switch(type_int){
  case 0:
    m_display_name = "COMMENT";
    break;
  case 1:
    m_display_name = "ASSIGNMENT_OP";
    break;
  case 2:
    m_display_name = "INFIX_OP";
    break;
  case 3:
    m_display_name = "PRE_POST_FIX_OP";
    break;
  case 4:
    m_display_name = "PREFIX_OP";
    break;
  case 5:
    m_display_name = "T_LEFT_ROUND_BRACKET";
    break;
  case 6:
    m_display_name = "T_RIGHT_ROUND_BRACKET";
    break;
  case 7:
    m_display_name = "T_LEFT_SQUARE_BRACKET";
    break;
  case 8:
    m_display_name = "T_RIGHT_SQUARE_BRACKET";
    break;
  case 9:
    m_display_name = "T_LEFT_CURLY_BRACKET";
    break;
  case 10:
    m_display_name = "T_RIGHT_CULRY_BRACKET";
    break;
  case 11:
    m_display_name = "T_DOT";
    break;
  case 12:
    m_display_name = "T_COMMA";
    break;
  case 13:
    m_display_name = "T_QUESTION";
    break;
  case 14:
    m_display_name = "T_COLON";
    break;
  case 15:
    m_display_name = "T_SEMICOLON";
    break;
  case 16:
    m_display_name = "INT_LITERAL";
    break;
  case 17:
    m_display_name = "FLOAT_LITERAL";
    break;
  case 18:
    m_display_name = "CHAR_LITERAL";
    break;
  case 19:
    m_display_name = "STRING_LITERAL";
    break;
  case 20:
    m_display_name = "BOOL_LITERAL";
    break;
  case 21:
    m_display_name = "NULL_LITERAL";
    break;
  case 22:
    m_display_name = "BASIC_TYPE";
    break;
  case 23:
    m_display_name = "VOID";
    break;
  case 24:
    m_display_name = "MODIFIER";
    break;
  case 25:
    m_display_name = "KEYWORDS";
    break;
  case 26:
    m_display_name = "T_IDENTIFIER";
    break;
  case 27:
    m_display_name = "T_STAR";
    break;
  case 28:
    m_display_name = "T_MINUS";
    break;
  case 29:
    m_display_name = "T_PLUS";
    break;
  case 1001:
    m_display_name = "Goal";
    break;
  case 1002:
    m_display_name = "Literal";
    break;
  case 1003:
    m_display_name = "Type";
    break;
  case 1004:
    m_display_name = "PrimitiveType";
    break;
  case 1005:
    m_display_name = "NumericType";
    break;
  case 1006:
    m_display_name = "IntegralType";
    break;
  case 1007:
    m_display_name = "FloatingPointType";
    break;
  case 1008:
    m_display_name = "ReferenceType";
    break;
  case 1009:
    m_display_name = "ClassOrInterfaceType";
    break;
  case 1010:
    m_display_name = "ClassType";
    break;
  case 1011:
    m_display_name = "InterfaceType";
    break;
  case 1012:
    m_display_name = "ArrayType";
    break;
  case 1013:
    m_display_name = "Name";
    break;
  case 1014:
    m_display_name = "SimpleName";
    break;
  case 1015:
    m_display_name = "QualifiedName";
    break;
  case 1016:
    m_display_name = "CompilationUnit";
    break;
  case 1017:
    m_display_name = "ImportDeclarations";
    break;
  case 1018:
    m_display_name = "TypeDeclarations";
    break;
  case 1019:
    m_display_name = "PackageDeclaration";
    break;
  case 1020:
    m_display_name = "ImportDeclaration";
    break;
  case 1021:
    m_display_name = "SingleTypeImportDeclaration";
    break;
  case 1022:
    m_display_name = "TypeImportOnDemandDeclaration";
    break;
  case 1023:
    m_display_name = "TypeDeclaration";
    break;
  case 1024:
    m_display_name = "Modifiers";
    break;
  case 1025:
    m_display_name = "Modifier";
    break;
  case 1026:
    m_display_name = "ClassDeclaration";
    break;
  case 1027:
    m_display_name = "Super";
    break;
  case 1028:
    m_display_name = "Interfaces";
    break;
  case 1029:
    m_display_name = "InterfaceTypeList";
    break;
  case 1030:
    m_display_name = "ClassBody";
    break;
  case 1031:
    m_display_name = "ClassBodyDeclarations";
    break;
  case 1032:
    m_display_name = "ClassBodyDeclaration";
    break;
  case 1033:
    m_display_name = "ClassMemberDeclaration";
    break;
  case 1034:
    m_display_name = "FieldDeclaration";
    break;
  case 1035:
    m_display_name = "VariableDeclarators";
    break;
  case 1036:
    m_display_name = "VariableDeclarator";
    break;
  case 1037:
    m_display_name = "VariableDeclaratorId";
    break;
  case 1038:
    m_display_name = "VariableInitializer";
    break;
  case 1039:
    m_display_name = "MethodDeclaration";
    break;
  case 1040:
    m_display_name = "MethodHeader";
    break;
  case 1041:
    m_display_name = "MethodDeclarator";
    break;
  case 1042:
    m_display_name = "FormalParameterList";
    break;
  case 1043:
    m_display_name = "FormalParameter";
    break;
  case 1044:
    m_display_name = "Throws";
    break;
  case 1045:
    m_display_name = "ClassTypeList";
    break;
  case 1046:
    m_display_name = "MethodBody";
    break;
  case 1047:
    m_display_name = "StaticInitializer";
    break;
  case 1048:
    m_display_name = "ConstructorDeclaration";
    break;
  case 1049:
    m_display_name = "ConstructorDeclarator";
    break;
  case 1050:
    m_display_name = "ConstructorBody";
    break;
  case 1051:
    m_display_name = "ExplicitConstructorInvocation";
    break;
  case 1052:
    m_display_name = "InterfaceDeclaration";
    break;
  case 1053:
    m_display_name = "ExtendsInterfaces";
    break;
  case 1054:
    m_display_name = "InterfaceBody";
    break;
  case 1055:
    m_display_name = "InterfaceMemberDeclarations";
    break;
  case 1056:
    m_display_name = "InterfaceMemberDeclaration";
    break;
  case 1057:
    m_display_name = "ConstantDeclaration";
    break;
  case 1058:
    m_display_name = "AbstractMethodDeclaration";
    break;
  case 1059:
    m_display_name = "ArrayInitializer";
    break;
  case 1060:
    m_display_name = "VariableInitializers";
    break;
  case 1061:
    m_display_name = "Block";
    break;
  case 1062:
    m_display_name = "BlockStatements";
    break;
  case 1063:
    m_display_name = "BlockStatement";
    break;
  case 1064:
    m_display_name = "LocalVariableDeclarationStatement";
    break;
  case 1065:
    m_display_name = "LocalVariableDeclaration";
    break;
  case 1066:
    m_display_name = "Statement";
    break;
  case 1067:
    m_display_name = "StatementNoShortIf";
    break;
  case 1068:
    m_display_name = "StatementWithoutTrailingSubstatement";
    break;
  case 1069:
    m_display_name = "EmptyStatement";
    break;
  case 1070:
    m_display_name = "LabeledStatement";
    break;
  case 1071:
    m_display_name = "LabeledStatementNoShortIf";
    break;
  case 1072:
    m_display_name = "ExpressionStatement";
    break;
  case 1073:
    m_display_name = "StatementExpression";
    break;
  case 1074:
    m_display_name = "IfThenStatement";
    break;
  case 1075:
    m_display_name = "IfThenElseStatement";
    break;
  case 1076:
    m_display_name = "IfThenElseStatementNoShortIf";
    break;
  case 1077:
    m_display_name = "SwitchStatement";
    break;
  case 1078:
    m_display_name = "SwitchBlock";
    break;
  case 1079:
    m_display_name = "SwitchBlockStatementGroups";
    break;
  case 1080:
    m_display_name = "SwitchBlockStatementGroup";
    break;
  case 1081:
    m_display_name = "SwitchLabels";
    break;
  case 1082:
    m_display_name = "SwitchLabel";
    break;
  case 1083:
    m_display_name = "WhileStatement";
    break;
  case 1084:
    m_display_name = "WhileStatementNoShortIf";
    break;
  case 1085:
    m_display_name = "DoStatement";
    break;
  case 1086:
    m_display_name = "ForStatement";
    break;
  case 1087:
    m_display_name = "ForStatementNoShortIf";
    break;
  case 1088:
    m_display_name = "ForInit";
    break;
  case 1089:
    m_display_name = "ForUpdate";
    break;
  case 1090:
    m_display_name = "StatementExpressionList";
    break;
  case 1091:
    m_display_name = "BreakStatement";
    break;
  case 1092:
    m_display_name = "ContinueStatement";
    break;
  case 1093:
    m_display_name = "ReturnStatement";
    break;
  case 1094:
    m_display_name = "ThrowStatement";
    break;
  case 1095:
    m_display_name = "SynchronizedStatement";
    break;
  case 1096:
    m_display_name = "TryStatement";
    break;
  case 1097:
    m_display_name = "Catches";
    break;
  case 1098:
    m_display_name = "CatchClause";
    break;
  case 1099:
    m_display_name = "Finally";
    break;
  case 1100:
    m_display_name = "Primary";
    break;
  case 1101:
    m_display_name = "PrimaryNoNewArray";
    break;
  case 1102:
    m_display_name = "ClassInstanceCreationExpression";
    break;
  case 1103:
    m_display_name = "ArgumentList";
    break;
  case 1104:
    m_display_name = "ArrayCreationExpression";
    break;
  case 1105:
    m_display_name = "DimExprs";
    break;
  case 1106:
    m_display_name = "DimExpr";
    break;
  case 1107:
    m_display_name = "Dims";
    break;
  case 1108:
    m_display_name = "FieldAccess";
    break;
  case 1109:
    m_display_name = "MethodInvocation";
    break;
  case 1110:
    m_display_name = "ArrayAccess";
    break;
  case 1111:
    m_display_name = "PostfixExpression";
    break;
  case 1112:
    m_display_name = "PostIncrementExpression";
    break;
  case 1113:
    m_display_name = "PostDecrementExpression";
    break;
  case 1114:
    m_display_name = "UnaryExpression";
    break;
  case 1115:
    m_display_name = "PreIncrementExpression";
    break;
  case 1116:
    m_display_name = "PreDecrementExpression";
    break;
  case 1117:
    m_display_name = "UnaryExpressionNotPlusMinus";
    break;
  case 1118:
    m_display_name = "CastExpression";
    break;
  case 1119:
    m_display_name = "MultiplicativeExpression";
    break;
  case 1120:
    m_display_name = "AdditiveExpression";
    break;
  case 1121:
    m_display_name = "ShiftExpression";
    break;
  case 1122:
    m_display_name = "RelationalExpression";
    break;
  case 1123:
    m_display_name = "EqualityExpression";
    break;
  case 1124:
    m_display_name = "AndExpression";
    break;
  case 1125:
    m_display_name = "ExclusiveOrExpression";
    break;
  case 1126:
    m_display_name = "InclusiveOrExpression";
    break;
  case 1127:
    m_display_name = "ConditionalAndExpression";
    break;
  case 1128:
    m_display_name = "ConditionalOrExpression";
    break;
  case 1129:
    m_display_name = "ConditionalExpression";
    break;
  case 1130:
    m_display_name = "AssignmentExpression";
    break;
  case 1131:
    m_display_name = "Assignment";
    break;
  case 1132:
    m_display_name = "LeftHandSide";
    break;
  case 1133:
    m_display_name = "AssignmentOperator";
    break;
  case 1134:
    m_display_name = "Expression";
    break;
  case 1135:
    m_display_name = "ConstantExpression";
    break;
  default:
    m_display_name = "TOKEN_FAILURE";
    break;
  }
}

Token::Token(TokenType type, int rule, const std::vector<Token>& generated_token):
  Token(type,"")
{
  m_rule = rule;
  m_generated_tokens = generated_token;
}

void Token::clear(){
  m_type = TokenType::TOKEN_EMPTY;
  m_display_name = "TOKEN_EMPTY";
  m_rule = -1;
  m_generated_tokens.clear();
}

std::ostream& operator<<(std::ostream& os, const Token& t){
  os << t.m_display_name;
  return os;
}
