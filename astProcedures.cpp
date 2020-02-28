#include "astNodes.h"
#include "token.h"

using namespace std;

TypeNode* Type(const Token& node) {
    if (node.m_generated_tokens[0].m_type == TokenType::PrimitiveType) {
        return PrimitiveType(node.m_generated_tokens[0]);
    }
    else {
        return ReferenceType(node.m_generated_tokens[0]);
    }
}

PrimitiveTypeNode* PrimitiveType(const Token& node) {
    if (node.m_generated_tokens[0].m_type == TokenType::NumericType) {
        return new PrimitiveTypeNode(node.m_generated_tokens[0].m_generated_tokens[0].m_lex);
    }
    
    return new PrimitiveTypeNode(node.m_generated_tokens[0].m_lex); 
}

ReferenceTypeNode* ReferenceType() {
    if (node.m_generated_tokens[0].m_type == TokenType::ClassOrInterfaceType) {
        return ClassOrInterfaceType(node.m_generated_tokens[0]);
    }
    else {
        return ArrayType(node.m_generated_tokens[0]);
    }    
}

NameNode* ClassOrInterfaceType() {
    return Name(node.m_generated_tokens[0]);
}

ArrayTypeNode* ArrayType() {
   PrimitiveTypeNode* primitiveTypeNode = nullptr;
   NameNode* nameNode = nullptr;
   if (node.m_generated_tokens[0].m_type == TokenType::PrimitiveType) {
        primitiveTypeNode = PrimitiveType(node.m_generated_tokens[0]);
        return new ArrayTypeNode(primitiveTypeNode, nameNode);
    }
    else {
        nameNode = Name(node.m_generated_tokens[0]);
        return new ArrayTypeNode(primitiveTypeNode, nameNode);
    }
}

NameNode* Name() {
    Token& child = node.m_generated_tokens[0];
    vector<string> identifiers;
    while (child.m_generated_tokens[0].m_type == TokenType::QualifiedName) {
        identifiers.push_back(child.m_generated_tokens[0].m_generated_tokens[2].m_lex);
        child = child.m_generated_tokens[0];
    }

    identifiers.push_back(child.m_generated_tokens[0].m_generated_tokens[0].m_lex);
    reverse(identifiers.begin(), identifiers.end());

    return new NameNode(identifiers);
}

CompilationUnitNode* CompilationUnitNode() {
    NameNode* name = nullptr;
    vector<ImportDeclNode*> importDeclarations;
    ClassDeclarationNode* classDeclarationNode = nullptr;
    InterfaceDeclarationNode* interfaceDeclarationNode = nullptr;
    for (Token& child: node.m_generated_tokens) {
        if (child.m_type == TokenType::PackageDeclaration) {
            name = Name(child.m_generated_tokens[1]);
        }
        else if (child.m_type == TokenType::ImportDeclarations) {
            importDeclarations = ImportDeclarations(child);
        }
        else if (child.m_type == TokenType::TypeDeclarations) {
            if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::ClassDeclaration) {
                classDeclarationNode = ClassDeclaration(child.m_generated_tokens[0].m_generated_tokens[0]);
            }
            else if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::InterfaceDeclaration) {
                interfaceDeclarationNode = InterfaceDeclaration(child.m_generated_tokens[0].m_generated_tokens[0]);
            }
        }
    }

    return new CompilationUnitNode(name, importDeclarations, classDeclarationNode, interfaceDeclarationNode);
}

vector<ImportDeclarationNode*> ImportDeclarations() {
    Token& child = node.m_generated_tokens[0];
    vector<ImportDeclarationNode*> importDeclarations;
    while (child.m_generated_tokens.size() > 1) {
        importDeclarations.push_back(ImportDeclaration(child.m_generated_tokens[1]));
        child = child.m_generated_tokens[0];
    }

    importDeclarations.push_back(ImportDeclaration(child.m_generated_tokens[0]));
    reverse(importDeclarations.begin(), importDeclarations.end());

    return importDeclarations;
}

ImportDeclarationNode* ImportDeclaration() {
    if (node.m_generated_tokens[0].m_type == TokenType::SingleTypeImportDeclaration) {
        return new ImportDeclarationNode(0, Name(node.m_generated_tokens[0].m_generated_tokens[1]));
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::TypeImportOnDemandDeclaration) {
        return new ImportDeclarationNode(1, Name(node.m_generated_tokens[0].m_generated_tokens[1]));    
    }
}

ClassDeclarationNode* ClassDeclaration() {
    vector<ModifierNode*> modifiers;
    string identifier;
    NameNode* name = nullptr;
    vector<InterfaceNode*> interfaces;
    vector<FieldDeclarationNode*> fieldDeclarations;
    vector<MethodDeclarationNode*> methodDeclarations;
    for (Token& child: node.m_generated_tokens) {
        if (child.m_type == TokenType::Modifiers) {
            modifiers = Modifiers(child);
        }
        else if (child.m_type == TokenType::T_IDENTIFIER) {
            identifier = child.m_lex;
        }
        else if (child.m_type == TokenType::Super) {
            name = ClassOrInterfaceType(Classchild.m_generated_tokens[1].m_generated_tokens[0]);
        }
        else if (child.m_type == TokenType::ClassBody) {
            if (child.m_generated_tokens[1] == TokenType::ClassBodyDeclarations) {
                
            }
        }
    }
}

vector<ClassBodyDeclarationNode*> ClassBodyDeclarations() {
    Token& child = node.m_generated_tokens[0];
    vector<ClassBodyDeclarationNode*> classBodyDeclarations;
    while (child.m_generated_tokens.size() > 1) {
        if (child.m_generated_tokens[1].m_generated_tokens[0].m_type == TokenType::ClassMemberDeclaration) {
            if (child.m_generated_tokens[1].m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::FieldDeclaration) {
                classBodyDeclarations.push_back(FieldDeclaration(child.m_generated_tokens[1].m_generated_tokens[0].m_generated_tokens[0]);
            }
            else if (child.m_generated_tokens[1].m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::MethodDeclaration) {
                classBodyDeclarations.push_back(MethodDeclaration(child.m_generated_tokens[1].m_generated_tokens[0].m_generated_tokens[0]);
            }
        }
        else if (child.m_generated_tokens[1].m_generated_tokens[0].m_type == TokenType::ConstructorDeclaration) {
            classBodyDeclarations.push_back(ConstructorDeclaration(child.m_generated_tokens[1].m_generated_tokens[0]);
        }
        child = child.m_generated_tokens[0];
    }

    if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::ClassMemberDeclaration) {
        if (child.m_generated_tokens[0].m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::FieldDeclaration) {
            classBodyDeclarations.push_back(FieldDeclaration(child.m_generated_tokens[0].m_generated_tokens[0].m_generated_tokens[0]);
        }
        else if (child.m_generated_tokens[0].m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::MethodDeclaration) {
            classBodyDeclarations.push_back(MethodDeclaration(child.m_generated_tokens[0].m_generated_tokens[0].m_generated_tokens[0]);
        }
    }
    else if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::ConstructorDeclaration) {
        classBodyDeclarations.push_back(ConstructorDeclaration(child.m_generated_tokens[0].m_generated_tokens[0]);
    }

    reverse(classBodyDeclarations.begin(), classBodyDeclarations.end());

    return classBodyDeclarations;
}

ConstructorDeclarationNode* ConstructorDeclaration() {
    vector<ModifierNode*> modifiers = Modifiers(node.m_generated_tokens[0]);
    ConstructorDeclaratorNode* constructorDeclaratorNode = ConstructorDeclarator(node.m_generated_tokens[1]);
    vector<BlockStatementNode*> blockStatements;
    if (node.m_generated_tokens[2].m_generated_tokens[1].m_type == TokenType::BlockStatements) {
        blockStatements = BlockStatements(node.m_generated_tokens[2].m_generated_tokens[1]);      
    }

    return new ConstructorDeclarationNode(modifiers, constructorDeclaratorNode, blockStatements);
}

ConstructorDeclaratorNode* ConstructorDeclarator() {
    string simpleName = node.m_generated_tokens[0].m_generated_tokens[0].m_lex;
    vector<FormalParameterNode*> formalParameters;
    if (node.m_generated_tokens[0].m_generated_tokens[2].m_type == TokenType::FormalParameterList) {
        formalParameters = FormalParameterList(node.m_generated_tokens[0].m_generated_tokens[2]);
    }

    return new ConstructorDeclaratorNode(simpleName, formalParameters);
}

vector<FormalParameterNode*> FormalParameterList() {
    Token& child = node.m_generated_tokens[0];
    vector<FormalParameterNode*> formalParameters;
    while (child.m_generated_tokens.size() > 1) {
        formalParameters.push_back(FormalParameter(child.m_generated_tokens[1]));
        child = child.m_generated_tokens[0];
    }

    formalParameters.push_back(FormalParameter(child.m_generated_tokens[0]));
    reverse(formalParameters.begin(), formalParameters.end());

    return formalParameters;
}

FormalParameterNode* FormalParameter() {
    TypeNode* type = Type(node.m_generated_tokens[0]);
    string identifier = node.m_generated_tokens[1].m_generated_tokens[0].m_lex;

    return new FormalParameterNode(type, identifier);
}

vector<BlockStatementNode*> BlockStatements() {
    Token& child = node.m_generated_tokens[0];
    vector<BlockStatementNode*> blockStatements;
    while (child.m_generated_tokens.size() > 1) {
        if (child.m_generated_tokens[1].m_generated_tokens[0].m_type == TokenType::LocalVariableDeclarationStatement) {
            blockStatements.push_back(LocalVariableDeclaration(child.m_generated_tokens[1].m_generated_tokens[0].m_generated_tokens[0]);
        }
        else if (child.m_generated_tokens[1].m_generated_tokens[0].m_type == TokenType::Statement) {
            blockStatements.push_back(Statement(child.m_generated_tokens[1].m_generated_tokens[0]);
        }
        child = child.m_generated_tokens[0];
    }

    if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::LocalVariableDeclarationStatement) {
        blockStatements.push_back(LocalVariableDeclaration(child.m_generated_tokens[0].m_generated_tokens[0].m_generated_tokens[0]);
    }
    else if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::Statement) {
        blockStatements.push_back(Statement(child.m_generated_tokens[0].m_generated_tokens[0]);
    }

    reverse(blockStatements.begin(), blockStatements.end());

    return blockStatements;

}

LocalVariableDeclarationNode* LocalVariableDeclaration() {
    Type* type = Type(node.m_generated_tokens[0]);
    VariableDeclaratorNode* variableDeclarator = VariableDeclarator(node.m_generated_tokens[1]);

    return new LocalVariableDeclarationNode(type, variableDeclarator);
}

VariableDeclaratorNode* VariableDeclarator() {
    string identifier = node.m_generated_tokens[0];
    ExpressionNode* expression = nullptr;
    if (node.m_generated_tokens.size() > 1) {
        expression = Expression(node.m_generated_tokens[2].m_generated_tokens[0]);
    }

    return new VariableDeclaratorNode(identifier, expression);
}

StatementNode* Statement() {
    StatementNode* statementNode = nullptr;
    if (node.m_generated_tokens[0].m_type == TokenType::StatementWithoutTrailingSubstatement) {
        statementNode = StatementWithoutTrailingSubstatement(node.m_generated_tokens[0]); 
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::IfThenStatement) {
        statementNode = IfThenStatement(node.m_generated_tokens[0]);
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::IfThenElseStatement) {
        statementNode = IfThenElseStatement(node.m_generated_tokens[0]);
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::WhileStatement) {
        statementNode = WhileStatement(node.m_generated_tokens[0]);
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::ForStatement) {
        statementNode = ForStatement(node.m_generated_tokens[0]);
    }

    return statementNode;
}

StatementNoShortIfNode* StatementNoShortIf() {
    StatementNoShortIfNode* statementNoShortIfNode = nullptr;
    if (node.m_generated_tokens[0].m_type == TokenType::StatementWithoutTrailingSubstatement) {
        statementNoShortIfNode = StatementWithoutTrailingSubstatement(node.m_generated_tokens[0]);
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::IfThenElseStatementNoShortIf) {
        statementNoShortIfNode = IfThenElseStatementNoShortIf(node.m_generated_tokens[0]);
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::WhileStatementNoShortIf) {
        statementNoShortIfNode = WhileStatementNoShortIf(node.m_generated_tokens[0]);
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::ForStatementNoShortIf) {
        statementNoShortIfNode = ForStatementNoShortIf(node.m_generated_tokens[0]);
    }

    return statementNoShortIfNode;
}

StatementWithoutTrailingSubstatementNode* StatementWithoutTrailingSubstatement() {
    StatementWithoutTrailingSubstatementNode* statementWithoutTrailingSubstatementNode = nullptr;
    if (node.m_generated_tokens[0].m_type == TokenType::Block) {
        if (node.m_generated_tokens[0].m_generated_tokens[1].m_type == TokenType::BlockStatements) {
            statementWithoutTrailingSubstatementNode = new BlockNode(BlockStatements(node.m_generated_tokens[0].m_generated_tokens[1]));
        }
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::ExpressionStatement) {
        statementWithoutTrailingSubstatementNode = StatementExpression(node.m_generated_tokens[0].m_generated_tokens[0]);
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::ReturnStatement) {
        statementWithoutTrailingSubstatementNode = ReturnStatement(node.m_generated_tokens[0]);
    }

    return statementWithoutTrailingSubstatementNode;
}

ReturnStatementNode* ReturnStatement() {
    ExpressionNode* expression = nullptr;
    if (node.m_generated_tokens[1].m_type == TokenType::Expression) {
        expression = Expression(node.m_generated_tokens[1]);
    }

    return new ReturnStatementNode(expression);
}

StatementExpressionNode* StatementExpression() {
    StatementExpressionNode* statementExpressionNode = nullptr;
    if (node.m_generated_tokens[0].m_type == TokenType::Assignment) {
        statementExpressionNode = Assignment(node.m_generated_tokens[0]);
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::MethodInvocation) {
        statementExpressionNode = MethodInvocation(node.m_generated_tokens[0]);
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::ClassInstanceCreationExpression) {
        statementExpressionNode = ClassInstanceCreationExpression(node.m_generated_tokens[0]);
    }

    return statementExpressionNode;
}

IfThenStatementNode* IfThenStatement() {
    ExpressionNode* expression = Expression(node.m_generated_tokens[2]);
    StatementNode* statement = Statement(node.m_generated_tokens[4]);

    return new IfThenStatementNode(expression, statement);
}

IfThenElseStatementNode* IfThenElseStatement() {
    ExpressionNode* expression = Expression(node.m_generated_tokens[2]);
    StatementNoShortIfNode* statementNoShortIf = StatementNoShortIf(node.m_generated_tokens[4]);
    StatementNode* statement = Statement(node.m_generated_tokens[6]);

    return IfThenElseStatementNode(expression, statementNoShortIf, statement);
}

IfThenElseStatementNoShortIfNode* IfThenElseStatementNoShortIf() {
    ExpressionNode* expression = Expression(node.m_generated_tokens[2]);
    StatementNoShortIfNode* statementNoShortIf1 = StatementNoShortIf(node.m_generated_tokens[4]);
    StatementNoShortIfNode* statementNoShortIf2 = StatementNoShortIf(node.m_generated_tokens[6]);

    return IfThenElseStatementNode(expression, statementNoShortIf1, statementNoShortIf2);
}

WhileStatementNode* WhileStatement() {
    ExpressionNode* expression = Expression(node.m_generated_tokens[2]);
    StatementNode* statement = Statement(node.m_generated_tokens[4]);

    return new WhileStatementNode(expression, statement);

}

WhileStatementNoShortIfNode* WhileStatementNoShortIf() {
    ExpressionNode* expression = Expression(node.m_generated_tokens[2]);
    StatementNoShortIfNode* statementNoShortIf = StatementNoShortIf(node.m_generated_tokens[4]);

    return WhileStatementNoShortIfNode(expression, statementNoShortIf);
}

ForStatementNode* ForStatement() {
    LocalVariableDeclarationNode* localVariableDeclaration = nullptr;
    StatementExpressionNode* statementExpressionForInit = nullptr;
    StatementExpressionNode* statementExpressionForUpdate = nullptr;
    ExpressionNode* expression = nullptr;
    StatementNode* statement = nullptr;

    for (Token& child: node.m_generated_tokens) {
        if (child.m_type == TokenType::ForInit) {
            if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::StatementExpression) {
                statementExpressionForInit = StatementExpression(child.m_generated_tokens[0].m_generated_tokens[0]);
            }
            else if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::LocalVariableDeclaration) {
                localVariableDeclaration = LocalVariableDeclaration(child.m_generated_tokens[0].m_generated_tokens[0]);
            }
        }
        else if (child.m_type == TokenType::Expression) {
            expression = Expression(child);
        }
        else if (child.m_type == TokenType::ForUpdate) {
            statementExpressionForUpdate = StatementExpression(child.m_generated_tokens[0]);
        }
        else if (child.m_type == TokenType::Statement) {
             statement= Statement(child);
        }
    }

    return new ForStatementNode(localVariableDeclaration, statementExpressionForInit, statementExpressionForUpdate, expression, statement);
}

ForStatementNoShortIfNode* ForStatementNoShortIf() {
    LocalVariableDeclarationNode* localVariableDeclaration = nullptr;
    StatementExpressionNode* statementExpressionForInit = nullptr;
    StatementExpressionNode* statementExpressionForUpdate = nullptr;
    ExpressionNode* expression = nullptr;
    StatementNoShortIfNode* statementNoShortIf = nullptr;
    
    for (Token& child: node.m_generated_tokens) {
        if (child.m_type == TokenType::ForInit) {
            if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::StatementExpression) {
                statementExpressionForInit = StatementExpression(child.m_generated_tokens[0].m_generated_tokens[0]);
            }
            else if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::LocalVariableDeclaration) {
                localVariableDeclaration = LocalVariableDeclaration(child.m_generated_tokens[0].m_generated_tokens[0]);
            }
        }
        else if (child.m_type == TokenType::Expression) {
            expression = Expression(child);
        }
        else if (child.m_type == TokenType::ForUpdate) {
            statementExpressionForUpdate = StatementExpression(child.m_generated_tokens[0]);
        }
        else if (child.m_type == TokenType::StatementNoShortIf) {
             statementNoShortIf = StatementNoShortIf(child);
        }
    }
      
    return new ForStatementNode(localVariableDeclaration, statementExpressionForInit, statementExpressionForUpdate, expression, statementNoShortIf);
}

vector<InterfaceNode*> Interfaces() {
    Token& child = node.m_generated_tokens[0];
    vector<InterfaceNode*> interfaces;
    while (child.m_generated_tokens.size() > 1) {
        interfaces.push_back(new InterfaceNode(ClassOrInterfaceType(child.m_generated_tokens[1].m_generated_tokens[0])));
        child = child.m_generated_tokens[0];
    }

    interfaces.push_back(new InterfaceNode(ClassOrInterfaceType(child.m_generated_tokens[0].m_generated_tokens[0])));
    reverse(interfaces.begin(), interfaces.end());

    return interfaces;
}

vector<ModifierNode*> Modifiers() {
    Token& child = node.m_generated_tokens[0];
    vector<ModifierNode*> modifiers;
    while (child.m_generated_tokens.size() > 1) {
        modifiers.push_back(new ModifierNode(child.m_generated_tokens[1].m_lex));
        child = child.m_generated_tokens[0];
    }

    modifiers.push_back(new ModifierNode(child.m_generated_tokens[0].m_lex));
    reverse(modifiers.begin(), modifiers.end());

    return modifiers;
}

FieldDeclarationNode* FieldDeclaration() {
    ModifiersNode* modifiers = Modifiers(node.m_generated_tokens[0]);
    TypeNode* type = Type(node.m_generated_tokens[1]);
    VariableDeclaratorNode* variableDeclarator = VariableDeclarator(node.m_generated_tokens[2].m_generated_tokens[0]);

    return new FieldDeclarationNode(modifiers, type, variableDeclarator)
}

MethodDeclarationNode* MethodDeclaration() {
    MethodHeaderNode* methodHeader = MethodHeader(node.m_generated_tokens[0]);
    BlockNode* block = nullptr;
    if (node.m_generated_tokens[1].m_generated_tokens[0].m_type == TokenType::Block) {
        block = Block(node.m_generated_tokens[1].m_generated_tokens[0]);
    }
     
    return new MethodDeclarationNode(methodHeader, block);
}

MethodHeaderNode* MethodHeader() {
    ModifiersNode* modifiers = Modifiers(node.m_generated_tokens[0]);
    TypeNode* type = nullptr;
    if (node.m_generated_tokens[1].m_type == TokenType::Type) {
        type = Type(node.m_generated_tokens[1]);
    }
    
    MethodDeclaratorNode* methodDeclarator = MethodDeclarator(node.m_generated_tokens[2]);

    return new MethodHeaderNode(modifiers, type, methodDeclarator);

}

MethodDeclaratorNode* MethodDeclarator() {
    string identifier = node.m_generated_tokens[0].m_lex;
    vector<FormalParameterNode*> formalParameters;
    if (node.m_generated_tokens[2].m_type == TokenType::FormalParameterList) {
        formalParameters = FormalParameterList(node.m_generated_tokens[2]);
    }

    return new MethodDeclaratorNode(identifier, formalParameters);
}

PrimaryNoNewArrayNode* PrimaryNoNewArray() {
    
}

ArrayCreationExpressionNode* ArrayCreationExpression() {
    PrimitiveTypeNode* primitiveType = nullptr;
    NameNode* name = nullptr;
    ExpressionNode* expression = Expression(node.m_generated_tokens[2].m_generated_tokens[1]);
    if (node.m_generated_tokens[1].m_type == TokenType::PrimitiveType) {
        primitiveType = PrimitiveType(node.m_generated_tokens[1]);
    }
    else {
        name = ClassOrInterfaceType(node.m_generated_tokens[1]);
    }

    return new ArrayCreationExpressionNode(primitiveType, name);
}

vector<ExpressionNode*> ArgumentList() {
    Token& child = node.m_generated_tokens[0];
    vector<ExpressionNode*> expressions;
    while (child.m_generated_tokens.size() > 1) {
        expressions.push_back(Expression(child.m_generated_tokens[2]));
        child = child.m_generated_tokens[0];
    }

    expressions.push_back(Expression(child.m_generated_tokens[0]));
    reverse(expressions.begin(), expressions.end());

    return expressions;
}

ClassInstanceCreationExpressionNode* ClassInstanceCreationExpression() {
    ClassOrInterfaceTypeNode* classOrInterfaceType = ClassOrInterfaceType(node.m_generated_tokens[1]);
    vector<ExpressionNode*> argumentsList;
    if (node.m_generated_tokens[3].m_type == TokenType::ArgumentList) {
        argumentList = ArgumentList(node.m_generated_tokens[3]);
    }

    return new ClassInstanceCreationExpressionNode(classOrInterfaceType, argumentsList);
}

ExpressionNode* Expression() {
    return AssignmentExpression(node.m_generated_tokens[0]);
}

AssignmentExpressionNode* AssignmentExpression() {
    if (node.m_generated_tokens[0].m_type == TokenType::Assignment) {
        return Assignment(node.m_generated_tokens[0]);
    }
    else {
        return ConditionalOrExpression(node.m_generated_tokens[0].m_generated_tokens[0]);
    }  
}

AssignmentNode* Assignment() {
    LeftHandSideNode* leftHandSide = LeftHandSide(node.m_generated_tokens[0]);
    AssignmentExpressionNode* assignmentExpression = AssignmentExpression(node.m_generated_tokens[2]);

    return new AssignmentNode(leftHandSide, assignmentExpression);
}

LeftHandSideNode* LeftHandSide() {
    LeftHandSideNode* leftHandSideNode = nullptr;
    if (node.m_generated_tokens[0].m_type == TokenType::Name) {
        statementExpressionNode = Name(node.m_generated_tokens[0]);
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::FieldAccess) {
        statementExpressionNode = FieldAccess(node.m_generated_tokens[0]);
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::ArrayAccess) {
        statementExpressionNode = ArrayAccess(node.m_generated_tokens[0]);
    }
    
    if (leftHandSideNode == nullptr) {
        throw std::invalid_argument( "Left hand side" );
    }
    else {
        return leftHandSideNode; 
    }
}

ConditionalOrExpressionNode* ConditionalOrExpression() {
    vector<ConditionalAndExpressionNode*> conditionalAndExpressions;
    Token& child = node.m_generated_tokens[0];
    while (child.m_generated_tokens.size() > 1) {
        conditionalAndExpressions.push_back(ConditionalAndExpression(child.m_generated_tokens[2]));
        child = child.m_generated_tokens[0];
    }

    conditionalAndExpressions.push_back(ConditionalAndExpression(child.m_generated_tokens[0]));
    reverse(conditionalAndExpressions.begin(), conditionalAndExpressions.end());

    return new ConditionalOrExpressionNode(conditionalAndExpressions);
}

ConditionalAndExpressionNode* ConditionalAndExpression() {
    vector<InclusiveOrExpressionNode*> inclusiveOrExpressions;
    Token& child = node.m_generated_tokens[0];
    while (child.m_generated_tokens.size() > 1) {
        inclusiveOrExpressions.push_back(InclusiveOrExpression(child.m_generated_tokens[2]));
        child = child.m_generated_tokens[0];
    }

    inclusiveOrExpressions.push_back(InclusiveOrExpression(child.m_generated_tokens[0]));
    reverse(inclusiveOrExpressions.begin(), inclusiveOrExpressions.end());

    return new ConditionalAndExpressionNode(inclusiveOrExpressions);
}

InclusiveOrExpressionNode* InclusiveOrExpression() {
    vector<ExclusiveOrExpressionNode*> exclusiveOrExpressions;
    Token& child = node.m_generated_tokens[0];
    while (child.m_generated_tokens.size() > 1) {
        exclusiveOrExpressions.push_back(ExclusiveOrExpression(child.m_generated_tokens[2]));
        child = child.m_generated_tokens[0];
    }

    exclusiveOrExpressions.push_back(ExclusiveOrExpression(child.m_generated_tokens[0]));
    reverse(exclusiveOrExpressions.begin(), exclusiveOrExpressions.end());

    return new InclusiveOrExpressionNode(exclusiveOrExpressions);
}

ExclusiveOrExpressionNode* ExclusiveOrExpression() {
    return AndExpression(node.m_generated_tokens[0]);
}

AndExpressionNode* AndExpression() {
    vector<EqualityExpressionNode*> equalityExpressions;
    Token& child = node.m_generated_tokens[0];
    while (child.m_generated_tokens.size() > 1) {
        equalityExpressions.push_back(EqualityExpression(child.m_generated_tokens[2]));
        child = child.m_generated_tokens[0];
    }

    equalityExpressions.push_back(EqualityExpression(child.m_generated_tokens[0]));
    reverse(equalityExpressions.begin(), equalityExpressions.end());

    return new AndExpressionNode(equalityExpressions);
}

EqualityExpressionNode* EqualityExpression() {
    RelationalExpressionNode* relationalExpression = nullptr;
    EqualityExpressionNode* equalityExpression = nullptr;
    int op = -1;

    if (node.m_generated_tokens.size() == 1) {
        return new EqualityExpressionNode(RelationalExpression(node.m_generated_tokens[0]), op, equalityExpression);
    }

    if (node.m_generated_tokens[1].m_type == TokenType::T_EQUAL_EQUAL) {
        op = 0;
        return new EqualityExpressionNode(RelationalExpression(node.m_generated_tokens[2]), op, EqualityExpression(node.m_generated_tokens[0]));
    }
    else if (node.m_generated_tokens[1].m_type == TokenType::T_NOT_EQUAL) {
        op = 1;
        return new EqualityExpressionNode(RelationalExpression(node.m_generated_tokens[2]), op, EqualityExpression(node.m_generated_tokens[0]));
    }
}

RelationalExpressionNode* relationalExpression() {
    RelationalExpressionNode* relationalExpression = nullptr;
    ShiftExpressionNode* shiftExpression = nullptr;
    ReferenceTypeNode* referenceType = nullptr;

    int op = -1;

    if (node.m_generated_tokens.size() == 1) {
        return new RelationalExpressionNode(ShiftExpression(node.m_generated_tokens[0]), op, relationalExpression, referenceType);
    }

    if (node.m_generated_tokens[1].m_type == TokenType::T_LESS) {
        op = 0;
        return new EqualityExpressionNode(ShiftExpression(node.m_generated_tokens[2]), op, RelationalExpression(node.m_generated_tokens[0]), referenceType);
    }
    else if (node.m_generated_tokens[1].m_type == TokenType::T_GREATER) {
        op = 1;
        return new EqualityExpressionNode(ShiftExpression(node.m_generated_tokens[2]), op, RelationalExpression(node.m_generated_tokens[0]), referenceType);
    }
    else if (node.m_generated_tokens[1].m_type == TokenType::T_LESS_EQUAL) {
        op = 2;
        return new EqualityExpressionNode(ShiftExpression(node.m_generated_tokens[2]), op, RelationalExpression(node.m_generated_tokens[0]), referenceType);
    }
    else if (node.m_generated_tokens[1].m_type == TokenType::T_GREATER_EQUAL) {
        op = 3;
        return new EqualityExpressionNode(ShiftExpression(node.m_generated_tokens[2]), op, RelationalExpression(node.m_generated_tokens[0], referenceType));
    }
    else {
        op = 4;
        return new EqualityExpressionNode(shiftExpression, op, RelationalExpression(node.m_generated_tokens[0]), ReferenceType(node.m_generated_tokens[2]));
    }
}

ShiftExpressionNode* ShiftExpression() {
    return AdditiveExpression(node.m_generated_tokens[0]); 
}

AdditiveExpressionNode* AdditiveExpression() {
    UnaryExpressionNode* unaryExpression = nullptr;
    MultiplicativeExpressionNode* multiplicativeExpression = nullptr;
    int op = -1;

    if (node.m_generated_tokens[1].m_type == TokenType::T_STAR) {
        op = 0;
        return new EqualityExpressionNode(ShiftExpression(node.m_generated_tokens[2]), op, RelationalExpression(node.m_generated_tokens[0]), referenceType);
    }
    else if (node.m_generated_tokens[1].m_type == TokenType::T_SLASH) {
        op = 1;
        return new EqualityExpressionNode(ShiftExpression(node.m_generated_tokens[2]), op, RelationalExpression(node.m_generated_tokens[0]), referenceType);
    }
    else if (node.m_generated_tokens[1].m_type == TokenType::T_PERCENT) {
        op = 2;
        return new EqualityExpressionNode(ShiftExpression(node.m_generated_tokens[2]), op, RelationalExpression(node.m_generated_tokens[0]), referenceType);
    }

}

MultiplicativeExpressionNode* MultiplicativeExpression() {

}
