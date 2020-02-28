#include "token.h"
#include "astNodes.h"

ArrayAccessNode* ArrayAccessCreate(Token node); 

void printChildrenOfNodes(ASTNode* astNode);

TypeNode* TypeCreate(Token node);


PrimitiveTypeNode* PrimitiveTypeCreate(Token node);


ReferenceTypeNode* ReferenceTypeCreate(Token node);


NameNode* ClassOrInterfaceTypeCreate(Token node);


ArrayTypeNode* ArrayTypeCreate(Token node);


NameNode* NameCreate(Token node);


CompilationUnitNode* CompilationUnitCreate(Token node);


std::vector<ImportDeclarationNode*> ImportDeclarationsCreate(Token node);


ImportDeclarationNode* ImportDeclarationCreate(Token node);


ClassDeclarationNode* ClassDeclarationCreate(Token node);


InterfaceDeclarationNode* InterfaceDeclarationCreate(Token node);


std::vector<MethodDeclarationNode*> InterfaceMemberDeclarationsCreate(Token node);


std::vector<ClassBodyDeclarationNode*> ClassBodyDeclarationsCreate(Token node);


ConstructorDeclarationNode* ConstructorDeclarationCreate(Token node);


ConstructorDeclaratorNode* ConstructorDeclaratorCreate(Token node);


std::vector<FormalParameterNode*> FormalParameterListCreate(Token node);


FormalParameterNode* FormalParameterCreate(Token node);


std::vector<BlockStatementNode*> BlockStatementsCreate(Token node);


LocalVariableDeclarationNode* LocalVariableDeclarationCreate(Token node);


VariableDeclaratorNode* VariableDeclaratorCreate(Token node);


StatementNode* StatementCreate(Token node);


StatementNoShortIfNode* StatementNoShortIfCreate(Token node);


StatementWithoutTrailingSubstatementNode* StatementWithoutTrailingSubstatementCreate(Token node);


ReturnStatementNode* ReturnStatementCreate(Token node);


StatementExpressionNode* StatementExpressionCreate(Token node);


IfThenStatementNode* IfThenStatementCreate(Token node);


IfThenElseStatementNode* IfThenElseStatementCreate(Token node);


IfThenElseStatementNoShortIfNode* IfThenElseStatementNoShortIfCreate(Token node);


WhileStatementNode* WhileStatementCreate(Token node);


WhileStatementNoShortIfNode* WhileStatementNoShortIfCreate(Token node);


ForStatementNode* ForStatementCreate(Token node);


ForStatementNoShortIfNode* ForStatementNoShortIfCreate(Token node);


std::vector<InterfaceNode*> ExtendsInterfacesCreate(Token node);


std::vector<InterfaceNode*> InterfacesCreate(Token node);


std::vector<ModifierNode*> ModifiersCreate(Token node);


FieldDeclarationNode* FieldDeclarationCreate(Token node);


MethodDeclarationNode* MethodDeclarationCreate(Token node);


MethodHeaderNode* MethodHeaderCreate(Token node);



MethodDeclaratorNode* MethodDeclaratorCreate(Token node);


PrimaryNoNewArrayNode* PrimaryNoNewArrayCreate(Token node);


ArrayCreationExpressionNode* ArrayCreationExpressionCreate(Token node);


std::vector<ExpressionNode*> ArgumentListCreate(Token node);


ClassInstanceCreationExpressionNode* ClassInstanceCreationExpressionCreate(Token node);


ExpressionNode* ExpressionCreate(Token node);


AssignmentExpressionNode* AssignmentExpressionCreate(Token node);


AssignmentNode* AssignmentCreate(Token node);


LeftHandSideNode* LeftHandSideCreate(Token node);


ConditionalOrExpressionNode* ConditionalOrExpressionCreate(Token node);


ConditionalAndExpressionNode* ConditionalAndExpressionCreate(Token node);


InclusiveOrExpressionNode* InclusiveOrExpressionCreate(Token node);


ExclusiveOrExpressionNode* ExclusiveOrExpressionCreate(Token node);


AndExpressionNode* AndExpressionCreate(Token node);


EqualityExpressionNode* EqualityExpressionCreate(Token node);


RelationalExpressionNode* RelationalExpressionCreate(Token node);


AdditiveExpressionNode* ShiftExpressionCreate(Token node);


AdditiveExpressionNode* AdditiveExpressionCreate(Token node);


MultiplicativeExpressionNode* MultiplicativeExpressionCreate(Token node);


FieldAccessNode* FieldAccessCreate(Token node);



