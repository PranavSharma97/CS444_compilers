#include <vector>
#include <string>
#include "token_types.h"
#include "environment.h"

class ASTNode {
  public:
    virtual TokenType type() { return TOKEN_FAILURE; };
    std::vector<ASTNode*> children;
    ASTNode() = default;
    ASTNode(const std::vector<ASTNode*>& ): children(children){}
};

class ExpressionNode: public virtual ASTNode {
  public:
    TokenType type() { return Expression; }
    //std::vector<ASTNode*> children;
    ExpressionNode() = default;
    ExpressionNode(std::vector<ASTNode*> children): ASTNode(children) {}
};

class AssignmentExpressionNode: public ExpressionNode {
// Contains Assignment, ConditionalOrExpression pointers
  public:
    TokenType type() { return AssignmentExpression; }
    //std::vector<ASTNode*> children;
    AssignmentExpressionNode() = default;
    AssignmentExpressionNode(std::vector<ASTNode*> children): ExpressionNode(children) {}
};

class ConditionalOrExpressionNode: public AssignmentExpressionNode {
// Contains ConditionalOrExpression and ConditionalAndExpression pointers
  public:
    TokenType type() { return ConditionalOrExpression; }
    std::vector<ASTNode*> children;
    ConditionalOrExpressionNode() = default;
    ConditionalOrExpressionNode(std::vector<ASTNode*> children): AssignmentExpressionNode(children) {}
};

class ConditionalAndExpressionNode: public ConditionalOrExpressionNode {
// Contains InclusiveOrExpression and ConditionalAndExpression pointers
  public:
    TokenType type() { return ConditionalAndExpression; }
    std::vector<ASTNode*> children;
    ConditionalAndExpressionNode() = default;
    ConditionalAndExpressionNode(std::vector<ASTNode*> children): ConditionalOrExpressionNode(children) {}
};

class InclusiveOrExpressionNode: public ConditionalAndExpressionNode {
// Contains InclusiveOrExpression and ExclusiveOrExpression pointers
  public:
    TokenType type() { return InclusiveOrExpression; }
    std::vector<ASTNode*> children;
    InclusiveOrExpressionNode() = default;
    InclusiveOrExpressionNode(std::vector<ASTNode*> children): ConditionalAndExpressionNode(children) {}
};

class ExclusiveOrExpressionNode: public InclusiveOrExpressionNode {
  public:
    TokenType type() { return ExclusiveOrExpression; }
    std::vector<ASTNode*> children;
    ExclusiveOrExpressionNode() = default;
    ExclusiveOrExpressionNode(std::vector<ASTNode*> children): InclusiveOrExpressionNode(children) {}
};

class AndExpressionNode: public ExclusiveOrExpressionNode {
  public:
    TokenType type() { return AndExpression; }
    std::vector<ASTNode*> children;
    AndExpressionNode() = default;
    AndExpressionNode(std::vector<ASTNode*> children): ExclusiveOrExpressionNode(children) {}
};

class EqualityExpressionNode: public AndExpressionNode {
  public:
    TokenType type() { return EqualityExpression; }
    std::vector<ASTNode*> children;
    EqualityExpressionNode() = default;
    int op;
    EqualityExpressionNode(int op, std::vector<ASTNode*> children): op(op), AndExpressionNode(children) {}
};

class RelationalExpressionNode: public EqualityExpressionNode {
// Contains RelationalExpression, AdditiveExpression, and ReferenceType pointers and operator
  public:
    TokenType type() { return RelationalExpression; }
    std::vector<ASTNode*> children;
    RelationalExpressionNode() = default;
    int op;
    RelationalExpressionNode(int op, std::vector<ASTNode*> children): op(op), EqualityExpressionNode(op, children) {}
};

class AdditiveExpressionNode: public RelationalExpressionNode {
// Contains MultiplicativeExpression and AdditiveExpression pointers and operator
  public:
    TokenType type() { return AdditiveExpression; }
    std::vector<ASTNode*> children;
    AdditiveExpressionNode() = default;
    int op;
    AdditiveExpressionNode(int op, std::vector<ASTNode*> children): op(op), children(children) {}
};

class MultiplicativeExpressionNode: public AdditiveExpressionNode {
// Contains MultiplicativeExpression, UnaryExpression pointers and operator
  public:
    TokenType type() { return MultiplicativeExpression; }
    std::vector<ASTNode*> children;
    MultiplicativeExpressionNode() = default;
    MultiplicativeExpressionNode(std::vector<ASTNode*> children): children(children) {}
};

class UnaryExpressionNode: public MultiplicativeExpressionNode {
    // Indicate minus, tilde, exclaimation or none
  public:
    TokenType type() { return UnaryExpression; }
    std::vector<ASTNode*> children;
    UnaryExpressionNode() = default;
    UnaryExpressionNode(std::vector<ASTNode*> children): MultiplicativeExpressionNode(children) {}
};

class PostfixExpressionNode: public UnaryExpressionNode {
  public:
    TokenType type() { return PostfixExpression; }
    std::vector<ASTNode*> children;
    PostfixExpressionNode() = default;
    PostfixExpressionNode(std::vector<ASTNode*> children): UnaryExpressionNode(children) {}
};

class PrimaryNode: public PostfixExpressionNode {
  public:
    TokenType type() { return Primary; }
    std::vector<ASTNode*> children;
    PrimaryNode() = default;
    PrimaryNode(std::vector<ASTNode*> children): PostfixExpressionNode(children) {}
};

class PrimaryNoNewArrayNode: public PrimaryNode {
  public:
    TokenType type() { return PrimaryNoNewArray; }
    std::vector<ASTNode*> children;
    PrimaryNoNewArrayNode() = default;
    PrimaryNoNewArrayNode(std::vector<ASTNode*> children): PrimaryNode(children) {}
};

class IntLiteralNode: public PrimaryNoNewArrayNode {
  public:
    int value;
    TokenType type() { return INT_LITERAL; }
    IntLiteralNode(int value): value(value) {}
};

class FloatLiteralNode: public PrimaryNoNewArrayNode {
  public:
    float value;
    TokenType type() { return FLOAT_LITERAL; }
    FloatLiteralNode(float value): value(value) {}
};

class BoolLiteralNode: public PrimaryNoNewArrayNode {
  public:
    bool value;
    TokenType type() { return BOOL_LITERAL; }
    BoolLiteralNode(bool value): value(value) {}
};

class CharLiteralNode: public PrimaryNoNewArrayNode {
  public:
    char value;
    TokenType type() { return CHAR_LITERAL; }
    CharLiteralNode(char value): value(value) {}
};

class stringLiteralNode: public PrimaryNoNewArrayNode {
  public:
    std::string value;
    TokenType type() { return STRING_LITERAL; }
    stringLiteralNode(std::string value): value(value) {}
};

class NullLiteralNode: public PrimaryNoNewArrayNode {
  public:
    TokenType type() { return NULL_LITERAL; }
    NullLiteralNode() {}
};

class IdentifierNode: public ASTNode {
  public:
    TokenType type() { return T_IDENTIFIER; }
    std::vector<std::string> identifier;
    IdentifierNode(std::vector<std::string> identifier): identifier(identifier) {}
};

class TypeNode: public virtual ASTNode {
  public:
    TokenType type() { return Type; }
    std::string type_name;
    TypeNode() = default;
    TypeNode(std::string type_name): type_name(type_name) {}
};


class ReferenceTypeNode: public TypeNode {
  public:
    TokenType type() { return ReferenceType; }
    std::vector<ASTNode*> children;
    ReferenceTypeNode() = default;
    ReferenceTypeNode(std::vector<ASTNode*> children): children(children) {}

};

class ClassOrInterfaceTypeNode: public virtual ReferenceTypeNode {
  public:
    TokenType type() { return ClassOrInterfaceType; }
    std::vector<ASTNode*> children;
    ClassOrInterfaceTypeNode() = default;
    ClassOrInterfaceTypeNode(std::vector<ASTNode*> children): children(children) {}
};

class ArrayTypeNode: public virtual ReferenceTypeNode {
  public:
    TokenType type() { return ArrayType; }
    std::vector<ASTNode*> children;
    ArrayTypeNode() = default;
    ArrayTypeNode(std::vector<ASTNode*> children): children(children) {}
};

class PrimitiveTypeNode: public ArrayTypeNode {
  public:
    TokenType type() { return PrimitiveType; }
    std::string lexeme;
    PrimitiveTypeNode() = default;
    PrimitiveTypeNode(std::string lexeme): lexeme(lexeme) {}
};

class IntegralTypeNode: ASTNode {
  public:
    TokenType type() { return IntegralType; }
    std::string type_name;
    IntegralTypeNode(std::string type_name): type_name(type_name) {}
};

class LeftHandSideNode: public virtual ASTNode {
  public:
    TokenType type() { return LeftHandSide; }
    std::vector<ASTNode*> children;
    LeftHandSideNode() = default;
    LeftHandSideNode(std::vector<ASTNode*> children): ASTNode(children) {}
};

class NameNode: public ArrayTypeNode, public ClassOrInterfaceTypeNode, public PostfixExpressionNode, public LeftHandSideNode {
  public:
    TokenType type() { return Name; }
    IdentifierNode* identifierNode;
    NameNode(std::vector<std::string> identifiers): identifierNode(new IdentifierNode(identifiers)) {}
};

class CompilationUnitNode: public ASTNode {
  public:
    environment scope;
    TokenType type() { return CompilationUnit; }
    std::vector<ASTNode*> children;
    CompilationUnitNode(std::vector<ASTNode*> children): ASTNode(children) {}
};

class ImportDeclarationNode: public ASTNode {
  public:
    TokenType type() { return ImportDeclaration; }
    int importType;
    std::vector<ASTNode*> children;
    ImportDeclarationNode(int importType, std::vector<ASTNode*> children): importType(importType), children(children) {}
};

class ClassDeclarationNode: public ASTNode {
  public:
    TokenType type() { return ClassDeclaration; }
    environment scope;
    std::string identifier;
    std::vector<ASTNode*> children;
    ClassDeclarationNode(std::string identifier, std::vector<ASTNode*> children): identifier(identifier), children(children) {}
    // Contains Superclass,
};

class ModifierNode: public ASTNode {
  public:
    TokenType type() { return Modifier; }
    std::string lexeme;
    ModifierNode(std::string lexeme): lexeme(lexeme) {}
};

class InterfaceNode: public ASTNode {
  public:
    TokenType type() { return T_INTERFACE; }
    std::vector<ASTNode*> children;
    InterfaceNode(std::vector<ASTNode*> children): ASTNode(children) {}
// Contains list of Name pointers (Interfaces implemented)
};

class ClassBodyDeclarationNode: public ASTNode {
  public:
    TokenType type() { return ClassBodyDeclaration; }
    std::vector<ASTNode*> children;
    ClassBodyDeclarationNode(std::vector<ASTNode*> children): ASTNode(children) {}
};

class ClassMemberDeclarationNode: public ClassBodyDeclarationNode {
  public:
    TokenType type() { return ClassMemberDeclaration; }
    std::vector<ASTNode*> children;
    ClassMemberDeclarationNode(std::vector<ASTNode*> children): ClassBodyDeclarationNode(children) {}
};

class FieldDeclarationNode: public ClassMemberDeclarationNode {
// Contains Modifiers, Type, and VariableDeclarator pointer
  public:
    TokenType type() { return FieldDeclaration; }
    environment scope;
    std::vector<ASTNode*> children;
    FieldDeclarationNode(std::vector<ASTNode*> children): ClassMemberDeclarationNode(children) {}
};

class VariableDeclaratorNode: public ASTNode {
//Id
// and Expression Node
  public:
    TokenType type() { return VariableDeclarator; }
    std::vector<ASTNode*> children;
    std::string identifier;
    VariableDeclaratorNode(std::string identifier, std::vector<ASTNode*> children): identifier(identifier), children(children) {}
};

class MethodDeclarationNode: public ClassMemberDeclarationNode {
  public:
    TokenType type() { return MethodDeclaration; }
    environment scope;
    std::vector<ASTNode*> children;
    MethodDeclarationNode(std::vector<ASTNode*> children): ClassMemberDeclarationNode(children) {}
};

class MethodHeaderNode: public ASTNode {
  public:
    TokenType type() { return MethodHeader; }
    std::vector<ASTNode*> children;
    MethodHeaderNode(std::vector<ASTNode*> children): ASTNode(children) {}
};

class MethodDeclaratorNode: public ASTNode {
  public:
    TokenType type() { return MethodDeclarator; }
    std::vector<ASTNode*> children;
    std::string identifier;
    MethodDeclaratorNode(std::string identifier, std::vector<ASTNode*> children): identifier(identifier), children(children) {}
};

class FormalParameterListNode: public ASTNode {
// Flatten
  public:
    TokenType type() { return FormalParameterList; }
    std::vector<ASTNode*> children;
    FormalParameterListNode(std::vector<ASTNode*> children): ASTNode(children) {}
};

class FormalParameterNode: public ASTNode {
  public:
    TokenType type() { return FormalParameter; }
    std::vector<ASTNode*> children;
    std::string identifier;
    FormalParameterNode(std::string identifier, std::vector<ASTNode*> children): identifier(identifier), children(children) {}
};

class ConstructorDeclarationNode: public ClassBodyDeclarationNode {
  public:
    TokenType type() { return ConstructorDeclaration; }
    environment scope;
    std::vector<ASTNode*> children;
    ConstructorDeclarationNode(std::vector<ASTNode*> children): ClassBodyDeclarationNode(children) {}
};

class ConstructorDeclaratorNode: public ASTNode {
    // Name and  Formal Parameter list
  public:
    TokenType type() { return ConstructorDeclarator; }
    std::vector<ASTNode*> children;
    std::string simpleName;
    ConstructorDeclaratorNode(std::string simpleName, std::vector<ASTNode*> children): simpleName(simpleName), children(children) {}
};

class InterfaceDeclarationNode: public ASTNode {
    // InterfaceMemberDeclarations pointer
  public:
    TokenType type() { return InterfaceDeclaration; }
    environment scope;
    std::vector<ASTNode*> children;
    std::string identifier;
    InterfaceDeclarationNode(std::string identifier, std::vector<ASTNode*> children): identifier(identifier), children(children) {}
};

class ExtendsInterfacesNode: public ASTNode {
    // Flatten
  public:
    TokenType type() { return ExtendsInterfaces; }
    std::vector<ASTNode*> children;
    ExtendsInterfacesNode(std::vector<ASTNode*> children): ASTNode(children) {}
};

class InterfaceMemberDeclarationsNode: public ASTNode {
    // Flatten
  public:
    TokenType type() { return InterfaceMemberDeclarations; }
    std::vector<ASTNode*> children;
    InterfaceMemberDeclarationsNode(std::vector<ASTNode*> children): ASTNode(children) {}
};

class BlockStatementNode: public virtual ASTNode {
  public:
    TokenType type() { return BlockStatement; }
    environment scope;
    std::vector<ASTNode*> children;
    BlockStatementNode(std::vector<ASTNode*> children): ASTNode(children) {}
};

class LocalVariableDeclarationNode: public BlockStatementNode {
  public:
    TokenType type() { return LocalVariableDeclaration; }
    environment scope;
    std::vector<ASTNode*> children;
    LocalVariableDeclarationNode(std::vector<ASTNode*> children): BlockStatementNode(children) {}
};

class StatementNode: public BlockStatementNode {
  public:
    TokenType type() { return Statement; }
    std::vector<ASTNode*> children;
    StatementNode(std::vector<ASTNode*> children): BlockStatementNode(children) {}
};

class StatementNoShortIfNode: public virtual ASTNode {
  public:
    TokenType type() { return StatementNoShortIf; }
    std::vector<ASTNode*> children;
    StatementNoShortIfNode() = default;
    StatementNoShortIfNode(std::vector<ASTNode*> children): ASTNode(children) {}
};

class StatementWithoutTrailingSubstatementNode: public StatementNode, public StatementNoShortIfNode {
  public:
    TokenType type() { return StatementWithoutTrailingSubstatement; }
    std::vector<ASTNode*> children;
    StatementWithoutTrailingSubstatementNode(std::vector<ASTNode*> children): StatementNode(children) {}
};

class BlockNode: public StatementWithoutTrailingSubstatementNode{
  public:
    TokenType type() { return Block; }
    std::vector<ASTNode*> children;
    BlockNode(std::vector<ASTNode*> children): StatementWithoutTrailingSubstatementNode(children) {}
};

class IfThenStatementNode: public StatementNode {
  public:
    TokenType type() { return IfThenStatement; }
    std::vector<ASTNode*> children;
    IfThenStatementNode(std::vector<ASTNode*> children): StatementNode(children) {}
};

class IfThenElseStatementNode: public StatementNode {
  public:
    TokenType type() { return IfThenElseStatement; }
    std::vector<ASTNode*> children;
    IfThenElseStatementNode(std::vector<ASTNode*> children): StatementNode(children) {}
};

class WhileStatementNode: public StatementNode {
  public:
    TokenType type() { return WhileStatement; }
    std::vector<ASTNode*> children;
    WhileStatementNode(std::vector<ASTNode*> children): StatementNode(children) {}
};

class ForStatementNode: public StatementNode {
  public:
    TokenType type() { return ForStatement; }
    environment scope;
    std::vector<ASTNode*> children;
    ForStatementNode(std::vector<ASTNode*> children): StatementNode(children) {}
};

class IfThenElseStatementNoShortIfNode: public StatementNoShortIfNode {
  public:
    TokenType type() { return IfThenElseStatementNoShortIf; }
    std::vector<ASTNode*> children;
    IfThenElseStatementNoShortIfNode(std::vector<ASTNode*> children): StatementNoShortIfNode(children) {}
};

class WhileStatementNoShortIfNode: public StatementNoShortIfNode {
  public:
    TokenType type() { return WhileStatementNoShortIf; }
    std::vector<ASTNode*> children;
    WhileStatementNoShortIfNode(std::vector<ASTNode*> children): StatementNoShortIfNode(children) {}
};

class ForStatementNoShortIfNode: public StatementNoShortIfNode {
  public:
    TokenType type() { return ForStatementNoShortIf; }
    std::vector<ASTNode*> children;
    ForStatementNoShortIfNode(std::vector<ASTNode*> children): StatementNoShortIfNode(children) {}
};

class ReturnStatementNode: public StatementWithoutTrailingSubstatementNode {
  public:
    TokenType type() { return ReturnStatement; }
    std::vector<ASTNode*> children;
    ReturnStatementNode(std::vector<ASTNode*> children): StatementWithoutTrailingSubstatementNode(children) {}
};

class StatementExpressionNode: public StatementWithoutTrailingSubstatementNode {
  public:
    TokenType type() { return StatementExpression; }
    std::vector<ASTNode*> children;
    StatementExpressionNode(std::vector<ASTNode*> children): StatementWithoutTrailingSubstatementNode(children) {}
};

class AssignmentNode: public StatementExpressionNode, public AssignmentExpressionNode {
  public:
    TokenType type() { return Assignment; }
    std::vector<ASTNode*> children;
    AssignmentNode(std::vector<ASTNode*> children): StatementExpressionNode(children) {}
};

class MethodInvocationNode: public StatementExpressionNode, public PrimaryNoNewArrayNode {
  public:
    TokenType type() { return MethodInvocation; }
    std::vector<ASTNode*> children;
    MethodInvocationNode(std::vector<ASTNode*> children): StatementExpressionNode(children) {}
};

class ClassInstanceCreationExpressionNode: public StatementExpressionNode, public PrimaryNoNewArrayNode {
  public:
    TokenType type() { return ClassInstanceCreationExpression; }
    std::vector<ASTNode*> children;
    ClassInstanceCreationExpressionNode(std::vector<ASTNode*> children): StatementExpressionNode(children) {}
};

class ArrayCreationExpressionNode: public PrimaryNode {
  public:
    TokenType type() { return ArrayCreationExpression; }
    std::vector<ASTNode*> children;
    ArrayCreationExpressionNode(std::vector<ASTNode*> children): PrimaryNode(children) {}
};

class LiteralNode: public PrimaryNoNewArrayNode {
  public:
    TokenType type() { return Literal; }
    std::vector<ASTNode*> children;
    LiteralNode(std::vector<ASTNode*> children): PrimaryNoNewArrayNode(children) {}
};

class ThisKeywordNode: public PrimaryNoNewArrayNode {
  public:
    TokenType type() { return T_THIS; }
    std::vector<ASTNode*> children;
    ThisKeywordNode(std::vector<ASTNode*> children): PrimaryNoNewArrayNode(children) {}
};

class FieldAccessNode: public PrimaryNoNewArrayNode, public LeftHandSideNode {
  public:
    TokenType type() { return FieldAccess; }
    std::vector<ASTNode*> children;
    std::string identifier;
    FieldAccessNode(std::string identifier, std::vector<ASTNode*> children): identifier(identifier), children(children) {}
};

class ArrayAccessNode: public PrimaryNoNewArrayNode, public LeftHandSideNode {
  public:
    TokenType type() { return ArrayAccess; }
    std::vector<ASTNode*> children;
    ArrayAccessNode(std::vector<ASTNode*> children): children(children) {}
};

class ArgumentListNode: public ASTNode {
    // Flatten
  public:
    TokenType type() { return ArgumentList; }
    std::vector<ASTNode*> children;
    ArgumentListNode(std::vector<ASTNode*> children): ASTNode(children) {}
};

class CastExpressionNode: public UnaryExpressionNode {
  public:
    TokenType type() { return CastExpression; }
    std::vector<ASTNode*> children;
    CastExpressionNode(std::vector<ASTNode*> children): UnaryExpressionNode(children) {}
};
