class Literal {
// Contains type indicator (0,1,2,3,4,5), and value
};

class IntLiteralNode {
  int value;
  IntLiteral(int value): value(value) {}
};

class FloatLiteralNode {
    float value;
    FloatLiteral(float value): value(value) {}
};

class BoolLiteralNode {
    bool value;
    BoolLiteral(bool value): value(value) {}
};

class CharLiteralNode {
    char value;
    CharLiteral(char value): value(value) {}
};

class StringLiteralNode {
    string value;
    StringLiteral(string value): value(value) {}
};

class NullLiteralNode {
    NullLiteral() {}
};

class IdentifierNode {
    string identifier;
    Identifier(string identifier): identifier(identifier) {}
};

class TypeNode {
    string type;
    TypeNode(string type): type(type) {}
};

class PrimitiveTypeNode: public ArrayType {
    
};

class ReferenceTypeNode: public TypeNode {
    
};

class ClassOrInterfaceType: public ReferenceTypeNode {

};

class ArrayType: public ReferenceTypeNode {

};

class IntegralTypeNode {
    string type;
};

class NameNode: public ArrayType, public ClassOrInterfaceType, public PostfixExpression, public LeftHandSide {
    string identifier;
    Name(string identifier): identifier(identifier) {}
};

class QualifiedNameNode: public NameNode {
    NameNode *nameNode;
    QualifiedNameNode(string identifier, NameNode *nameNode): super(identifier), nameNode(nameNode) {}
};

class CompilationUnitNode {
    
};

class ImportDeclNode {
    string name;
    ImportDeclNode(string name): name(name) {}
};

class SingleTypeImportNode: public ImportDeclNode {
    string name;
    SingleTypeImportNode(string name): super(name) {}    
};

class TypeImportOnDemandNode: public ImportDeclNode {
    string name;
    TypeImportOnDemandNode(string name): super(name) {}
};

class ClassDeclNode {
    string identifier;
    ClassDeclNode(string identifier): super(identifier) {}
    // Contains Superclass, 
};

class Modifiers {

};

class InterfacesNode {
// Contains list of Name pointers (Interfaces implemented)
};

class ClassBodyDeclarations {
// Contains list of ClassBodyDeclaration pointers
};

class ClassBodyDeclaration {

};

class ClassMemberDeclaration: public ClassBodyDeclaration {

};

class ConstructorDeclaration: public ClassBodyDeclaration {

};

class FieldDeclaration: public ClassMemberDeclaration {

};

class VariableDeclarator {
//Id
// and Expression Node
};

class MethodDeclaration: public ClassMemberDeclaration {

};

class MethodHeader {

};

class MethodDeclarator {

};

class FormalParameterList {
// Flatten
};

class FormalParameter {
    
};

class ConstructorDeclaration: public ClassBodyDeclaration {

};

class ConstructorDeclarator {
    // Name and  Formal Parameter list
};

class InterfaceDeclaration {
    // InterfaceMemberDeclarations pointer
};

class ExtendsInterfaces {
    // Flatten
};

class InterfaceMemberDeclarations {
    // Flatten
};

class Block: public StatementWithoutTrailingSubstatement{
};

class BlockStatements {
    // Flatten
};

class BlockStatement {

};

class LocalVariableDeclaration {

};

class Statement {

};

class StatementWithoutTrailingSubstatement: public Statement {

};

class IfThenStatement: public Statement {

};

class IfThenElseStatement: public Statement {

};

class WhileStatement: public Statement {

};

class ForStatement: public Statement {

};

class StatementNoShortIf {

};

class StatementWithoutTrailingSubstatement: public StatementNoShortIf {

};

class IfThenElseStatementNoShortIf: public StatementNoShortIf {

};

class WhileStatementNoShortIf: public StatementNoShortIf {

};

class ForStatementNoShortIf: public StatementNoShortIf {

};

class StatementWithoutTrailingSubstatement {

};

class ReturnStatement: public StatementWithoutTrailingSubstatement {

};

class StatementExpression: public StatementWithoutTrailingSubstatement {

};

class Assignment: public StatementExpression {

};

class MethodInvocation: public StatementExpression, public PrimaryNoNewArray {

};

class ClassInstanceCreationExpression: public StatementExpression, public PrimaryNoNewArray {

};

class Primary: public PostfixExpression {

};

class PrimaryNoNewArray: public Primary {

};

class ArrayCreationExpression: public Primary {

};

class Literal: public PrimaryNoNewArray {

};

class ThisKeyword: public PrimaryNoNewArray {

};

class Literal: public PrimaryNoNewArray {

};

class FieldAccess: public PrimaryNoNewArray, public LeftHandSide {

};

class ArrayAccess: public PrimaryNoNewArray, public LeftHandSide {

};

class ArgumentList {
    // Flatten
};

class ArrayCreationExpression {
    // ReferenceType pointer 
};

class PostfixExpression: public UnaryExpression {

};

class UnaryExpression: public MultiplicativeExpression {
    // Indicate minus, tilde, exclaimation or none
};

class CastExpression: public UnaryExpression {

};

class Expression {

};

class AssignmentExpression: public Expression {
// Contains Assignment, ConditionalOrExpression pointers 
};

class Assignment: public AssignmentExpression {
// Contains LeftHandSide, =, and AssignmentExpression pointer
};

class LeftHandSide {

};

class ConditionalOrExpression: public AssignmentExpression {
// Contains ConditionalOrExpression and ConditionalAndExpression pointers
};

class ConditionalAndExpression: public ConditionalOrExpression {
// Contains InclusiveOrExpression and ConditionalAndExpression pointers
};

class InclusiveOrExpression: public ConditionalAndExpression {
// Contains InclusiveOrExpression and ExclusiveOrExpression pointers
};

class ExclusiveOrExpression: public InclusiveOrExpression {

};

class AndExpression: public ExclusiveOrExpression {

};

class EqualityExpression: public AndExpression {

};

class RelationalExpression: public EqualityExpression {
// Contains RelationalExpression, AdditiveExpression, and ReferenceType pointers and operator
};

class AdditiveExpression: public RelationalExpression {
// Contains MultiplicativeExpression and AdditiveExpression pointers and operator 
};

class MultiplicativeExpression: public AdditiveExpression {
// Contains MultiplicativeExpression, UnaryExpression pointers and operator
};


