class IntLiteralNode: public PrimaryNoNewArrayNode {
  int value;
  IntLiteralNode(int value): value(value) {}
};

class FloatLiteralNode: public PrimaryNoNewArrayNode {
    float value;
    FloatLiteralNode(float value): value(value) {}
};

class BoolLiteralNode: public PrimaryNoNewArrayNode {
    bool value;
    BoolLiteralNode(bool value): value(value) {}
};

class CharLiteralNode: public PrimaryNoNewArrayNode {
    char value;
    CharLiteralNode(char value): value(value) {}
};

class StringLiteralNode: public PrimaryNoNewArrayNode {
    string value;
    StringLiteralNode(string value): value(value) {}
};

class NullLiteralNode: public PrimaryNoNewArrayNode {
    NullLiteralNode() {}
};

class IdentifierNode {
    string identifier;
    IdentifierNode(string identifier): identifier(identifier) {}
};

class TypeNode {
    string type;
    TypeNode(string type): type(type) {}
};

class PrimitiveTypeNode: public ArrayTypeNode {
    
};

class ReferenceTypeNode: public TypeNode {
    
};

class ClassOrInterfaceTypeNode: public ReferenceTypeNode {

};

class ArrayTypeNode: public ReferenceTypeNode {

};

class IntegralTypeNode {
    string type;
};

class NameNode: public ArrayType, public ClassOrInterfaceType, public PostfixExpression, public LeftHandSideNode {
    vector<string> identifiers;
    NameNode(vector<string> identifiers): identifiers(identifiers) {}
};

class CompilationUnitNode {
    
};

class ImportDeclarationNode {
    string name;
    ImportDeclarationNode(string name): name(name) {}
};

class SingleTypeImportNode: public ImportDeclNode {
    string name;
    SingleTypeImportNode(string name): super(name) {}    
};

class TypeImportOnDemandNode: public ImportDeclNode {
    string name;
    TypeImportOnDemandNode(string name): super(name) {}
};

class ClassDeclarationNode {
    string identifier;
    ClassDeclarationNode(string identifier): identifier(identifier) {}
    // Contains Superclass, 
};

class ModifierNode {

};

class InterfaceNode {
// Contains list of Name pointers (Interfaces implemented)
};

class ClassBodyDeclarationNode {

};

class ClassMemberDeclarationNode: public ClassBodyDeclarationNode {

};

class FieldDeclarationNode: public ClassMemberDeclarationNode {
// Contains Modifiers, Type, and VariableDeclarator pointer
};

class VariableDeclaratorNode {
//Id
// and Expression Node
};

class MethodDeclarationNode: public ClassMemberDeclarationNode {

};

class MethodHeaderNode {

};

class MethodDeclaratorNode {

};

class FormalParameterListNode {
// Flatten
};

class FormalParameterNode {
    
};

class ConstructorDeclarationNode: public ClassBodyDeclarationNode {

};

class ConstructorDeclaratorNode {
    // Name and  Formal Parameter list
};

class InterfaceDeclarationNode {
    // InterfaceMemberDeclarations pointer
};

class ExtendsInterfacesNode {
    // Flatten
};

class InterfaceMemberDeclarationsNode {
    // Flatten
};

class BlockNode: public StatementWithoutTrailingSubstatement{
};

class BlockStatementNode {

};

class LocalVariableDeclarationNode: public BlockStatementNode {

};

class StatementNode: public BlockStatementNode {

};

class StatementWithoutTrailingSubstatementNode: public StatementNode {

};

class IfThenStatementNode: public StatementNode {

};

class IfThenElseStatementNode: public StatementNode {

};

class WhileStatementNode: public StatementNode {

};

class ForStatementNode: public StatementNode {

};

class StatementNoShortIfNode {

};

class StatementWithoutTrailingSubstatementNode: public StatementNoShortIfNode {

};

class IfThenElseStatementNoShortIfNode: public StatementNoShortIfNode {

};

class WhileStatementNoShortIfNode: public StatementNoShortIfNode {

};

class ForStatementNoShortIfNode: public StatementNoShortIfNode {

};

class ReturnStatementNode: public StatementWithoutTrailingSubstatementNode {

};

class StatementExpressionNode: public StatementWithoutTrailingSubstatementNode {

};

class AssignmentNode: public StatementExpressionNode {

};

class MethodInvocationNode: public StatementExpression, public PrimaryNoNewArrayNode {

};

class ClassInstanceCreationExpressionNode: public StatementExpression, public PrimaryNoNewArrayNode {

};

class PrimaryNode: public PostfixExpressionNode {

};

class PrimaryNoNewArrayNode: public PrimaryNode {

};

class ArrayCreationExpressionNode: public PrimaryNode {

};

class LiteralNode: public PrimaryNoNewArrayNode {

};

class ThisKeywordNode: public PrimaryNoNewArrayNode {

};

class LiteralNode: public PrimaryNoNewArrayNode {

};

class FieldAccessNode: public PrimaryNoNewArray, public LeftHandSideNode {

};

class ArrayAccessNode: public PrimaryNoNewArray, public LeftHandSideNode {

};

class ArgumentListNode {
    // Flatten
};

class ArrayCreationExpressionNode {
    // ReferenceType pointer 
};

class PostfixExpressionNode: public UnaryExpressionNode {

};

class UnaryExpressionNode: public MultiplicativeExpressionNode {
    // Indicate minus, tilde, exclaimation or none
};

class CastExpressionNode: public UnaryExpressionNode {

};

class ExpressionNode {

};

class AssignmentExpressionNode: public ExpressionNode {
// Contains Assignment, ConditionalOrExpression pointers 
};

class AssignmentNode: public AssignmentExpressionNode {
// Contains LeftHandSide, =, and AssignmentExpression pointer
};

class LeftHandSideNode {

};

class ConditionalOrExpressionNode: public AssignmentExpressionNode {
// Contains ConditionalOrExpression and ConditionalAndExpression pointers
};

class ConditionalAndExpressionNode: public ConditionalOrExpressionNode {
// Contains InclusiveOrExpression and ConditionalAndExpression pointers
};

class InclusiveOrExpressionNode: public ConditionalAndExpressionNode {
// Contains InclusiveOrExpression and ExclusiveOrExpression pointers
};

class ExclusiveOrExpressionNode: public InclusiveOrExpressionNode {

};

class AndExpressionNode: public ExclusiveOrExpressionNode {

};

class EqualityExpressionNode: public AndExpressionNode {

};

class RelationalExpressionNode: public EqualityExpressionNode {
// Contains RelationalExpression, AdditiveExpression, and ReferenceType pointers and operator
};

class AdditiveExpressionNode: public RelationalExpressionNode {
// Contains MultiplicativeExpression and AdditiveExpression pointers and operator 
};

class MultiplicativeExpressionNode: public AdditiveExpressionNode {
// Contains MultiplicativeExpression, UnaryExpression pointers and operator
};


