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

ReferenceTypeNode* ReferenceType(const Token& node) {
    if (node.m_generated_tokens[0].m_type == TokenType::ClassOrInterfaceType) {
        return ClassOrInterfaceType(node.m_generated_tokens[0]);
    }
    else {
        return ArrayType(node.m_generated_tokens[0]);
    }    
}

NameNode* ClassOrInterfaceType(const Token& node) {
    return Name(node.m_generated_tokens[0]);
}

ArrayTypeNode* ArrayType(const Token& node) {
   PrimitiveTypeNode* primitiveTypeNode = nullptr;
   NameNode* nameNode = nullptr;
   if (node.m_generated_tokens[0].m_type == TokenType::PrimitiveType) {
        primitiveTypeNode = PrimitiveType(node.m_generated_tokens[0]);
        vector<ASTNode*> children = {primitiveTypeNode};
        return new ArrayTypeNode(children);
    }
    else {
        vector<ASTNode*> children = {nameNode};
        return new ArrayTypeNode(children);
    }
}

NameNode* Name(const Token& node) {
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

CompilationUnitNode* CompilationUnitNode(const Token& node) {
    NameNode* name = nullptr;
    vector<ImportDeclNode*> importDeclarations;
    ClassDeclarationNode* classDeclarationNode = nullptr;
    InterfaceDeclarationNode* interfaceDeclarationNode = nullptr;
    vector<ASTNode*> children;
    for (Token& child: node.m_generated_tokens) {
        if (child.m_type == TokenType::PackageDeclaration) {
            name = Name(child.m_generated_tokens[1]);
            children.push_back(name);
        }
        else if (child.m_type == TokenType::ImportDeclarations) {
            importDeclarations = ImportDeclarations(child);
            for (int i=0; i<importDeclarations.size(); i++) {
                children.push_back(importDeclarations[i]);
            }
        }
        else if (child.m_type == TokenType::TypeDeclarations) {
            if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::ClassDeclaration) {
                classDeclarationNode = ClassDeclaration(child.m_generated_tokens[0].m_generated_tokens[0]);
                children.push_back(classDeclarationNode);
            }
            else if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::InterfaceDeclaration) {
                interfaceDeclarationNode = InterfaceDeclaration(child.m_generated_tokens[0].m_generated_tokens[0]);
                children.push_back(interfaceDeclarationNode);
            }
        }
    }

    return new CompilationUnitNode(children);
}

vector<ImportDeclarationNode*> ImportDeclarations(const Token& node) {
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

ImportDeclarationNode* ImportDeclaration(const Token& node) {
    if (node.m_generated_tokens[0].m_type == TokenType::SingleTypeImportDeclaration) {
        vector<ASTNode*> children = {Name(node.m_generated_tokens[0].m_generated_tokens[1])};
        return new ImportDeclarationNode(0, children);
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::TypeImportOnDemandDeclaration) {
        vector<ASTNode*> children = {Name(node.m_generated_tokens[0].m_generated_tokens[1])};
        return new ImportDeclarationNode(1, children);
    }
}

ClassDeclarationNode* ClassDeclaration(const Token& node) {
    vector<ModifierNode*> modifiers;
    string identifier;
    NameNode* name = nullptr;
    vector<InterfaceNode*> interfaces;
    vector<FieldDeclarationNode*> fieldDeclarations;
    vector<MethodDeclarationNode*> methodDeclarations;
    vector<ClassBodyDeclarationNode*> classBodyDeclarations; 
    vector<ASTNode*> children;
    for (Token& child: node.m_generated_tokens) {
        if (child.m_type == TokenType::Modifiers) {
            modifiers = Modifiers(child);
            for (int i=0; i<modifiers.size(); i++) {
                children.push_back(modifiers[i]);
            }
        }
        else if (child.m_type == TokenType::T_IDENTIFIER) {
            identifier = child.m_lex;
        }
        else if (child.m_type == TokenType::Super) {
            name = ClassOrInterfaceType(Classchild.m_generated_tokens[1].m_generated_tokens[0]);
            children.push_back(name);
        }
        else if (child.m_type == TokenType::Interfaces) {
            interfaces = Interfaces(child);
            for (int i=0; i<interfaces.size(); i++) {
                children.push_back(interfaces[i]);
            }
        }
        else if (child.m_type == TokenType::ClassBody) {
            if (child.m_generated_tokens[1] == TokenType::ClassBodyDeclarations) {
                classBodyDeclarations = ClassBodyDeclarations(child.m_generated_tokens[1]);
                for (int i=0; i<classBodyDeclarations.size(); i++) {
                    children.push_back(classBodyDeclarations[i]); 
                }
            }
        }
    }

    return new ClassDeclarationNode(identifier, children);
}

InterfaceDeclarationNode* InterfaceDeclaration(const Token& node) {
    vector<ModifierNode*> modifiers;
    string identifier;
    NameNode* name = nullptr;
    vector<InterfaceNode*> interfaces;
    vector<MethodHeaderNode*> methodHeaders;
    vector<MethodHeader*> ;
    vector<ASTNode*> children;
    for (Token& child: node.m_generated_tokens) {
        if (child.m_type == TokenType::Modifiers) {
            modifiers = Modifiers(child);
            for (int i=0; i<modifiers.size(); i++) {
                children.push_back(modifiers[i]);
            }
        }
        else if (child.m_type == TokenType::T_IDENTIFIER) {
            identifier = child.m_lex;
        }
        else if (child.m_type == TokenType::ExtendsInterfaces) {
            interfaces = ExtendsInterfaces(child);
            for (int i=0; i<interfaces.size(); i++) {
                children.push_back(interfaces[i]);
            }
        }
        else if (child.m_type == TokenType::ClassBody) {
            if (child.m_generated_tokens[1] == TokenType::InterfaceMemberDeclarations) {
                interfaceMemberDeclarations = InterfaceMemberDeclarations(child.m_generated_tokens[1]);
                for (int i=0; i<interfaceMemberDeclarations.size(); i++) {
                    children.push_back(interfaceMemberDeclarations[i]);
                }
            }
        }
    }

    return new InterfaceDeclarationNode(identifier, children);

}

vector<MethodHeaderNode*> InterfaceMemberDeclarations(const Token& node) {
    vector<MethodHeaderNode*> methodHeaders;
    Token& child = node.m_generated_tokens[0];
    while (child.m_generated_tokens.size() > 1) {
        methodHeaders.push_back(MethodHeader(child.m_generated_tokens[1].m_generated_tokens[0].m_generated_tokens[0]));
        child = child.m_generated_tokens[0];
    }

    methodHeaders.push_back(MethodHeader(child.m_generated_tokens[1].m_generated_tokens[0].m_generated_tokens[0]));
    reverse(methodHeaders.begin(), methodHeaders.end());

    return methodHeaders;
}

vector<ClassBodyDeclarationNode*> ClassBodyDeclarations(const Token& node) {
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

ConstructorDeclarationNode* ConstructorDeclaration(const Token& node) {
    vector<ASTNode*> children;
    vector<ModifierNode*> modifiers = Modifiers(node.m_generated_tokens[0]);
    for (int i=0; i<modifiers.size(); i++) {
        children.push_back(modifiers[i]);
    }
    ConstructorDeclaratorNode* constructorDeclaratorNode = ConstructorDeclarator(node.m_generated_tokens[1]);
    children.push_back(constructorDeclaratorNode);
    vector<BlockStatementNode*> blockStatements;
    if (node.m_generated_tokens[2].m_generated_tokens[1].m_type == TokenType::BlockStatements) {
        blockStatements = BlockStatements(node.m_generated_tokens[2].m_generated_tokens[1]);
        for (int i=0; i<blockStatements.size(); i++) {
            children.push_back(blockStatements[i]);
        }  
    }

    return new ConstructorDeclarationNode(children);
}

ConstructorDeclaratorNode* ConstructorDeclarator(const Token& node) {
    string simpleName = node.m_generated_tokens[0].m_generated_tokens[0].m_lex;
    vector<FormalParameterNode*> formalParameters;
    vector<ASTNode*> children;
    if (node.m_generated_tokens[0].m_generated_tokens[2].m_type == TokenType::FormalParameterList) {
        formalParameters = FormalParameterList(node.m_generated_tokens[0].m_generated_tokens[2]);
        for (int i=0; i<formalParameters.size(); i++) {
            children.push_back(formalParameters[i]);
        }
    }

    return new ConstructorDeclaratorNode(simpleName, children);
}

vector<FormalParameterNode*> FormalParameterList(const Token& node) {
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

FormalParameterNode* FormalParameter(const Token& node) {
    TypeNode* type = Type(node.m_generated_tokens[0]);
    string identifier = node.m_generated_tokens[1].m_generated_tokens[0].m_lex;
    vector<ASTNode*> children = {type};

    return new FormalParameterNode(identifier, children);
}

vector<BlockStatementNode*> BlockStatements(const Token& node) {
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

LocalVariableDeclarationNode* LocalVariableDeclaration(const Token& node) {
    Type* type = Type(node.m_generated_tokens[0]);
    VariableDeclaratorNode* variableDeclarator = VariableDeclarator(node.m_generated_tokens[1]);
    vector<ASTNode*> children = {type, variableDeclarator};

    return new LocalVariableDeclarationNode(children);
}

VariableDeclaratorNode* VariableDeclarator(const Token& node) {
    string identifier = node.m_generated_tokens[0];
    ExpressionNode* expression = nullptr;
    vector<ASTNode*> children;
    if (node.m_generated_tokens.size() > 1) {
        expression = Expression(node.m_generated_tokens[2].m_generated_tokens[0]);
        children.push_back(expression);
    }

    return new VariableDeclaratorNode(identifier, children);
}

StatementNode* Statement(const Token& node) {
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

StatementNoShortIfNode* StatementNoShortIf(const Token& node) {
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

StatementWithoutTrailingSubstatementNode* StatementWithoutTrailingSubstatement(const Token& node) {
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

ReturnStatementNode* ReturnStatement(const Token& node) {
    ExpressionNode* expression = nullptr;
    vector<ASTNode*> children;
    if (node.m_generated_tokens[1].m_type == TokenType::Expression) {
        expression = Expression(node.m_generated_tokens[1]);
        children.push_back(expression);
    }

    return new ReturnStatementNode(children);
}

StatementExpressionNode* StatementExpression(const Token& node) {
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

IfThenStatementNode* IfThenStatement(const Token& node) {
    ExpressionNode* expression = Expression(node.m_generated_tokens[2]);
    StatementNode* statement = Statement(node.m_generated_tokens[4]);

    vector<ASTNode*> children = {expression, statement};

    return new IfThenStatementNode(children);
}

IfThenElseStatementNode* IfThenElseStatement(const Token& node) {
    ExpressionNode* expression = Expression(node.m_generated_tokens[2]);
    StatementNoShortIfNode* statementNoShortIf = StatementNoShortIf(node.m_generated_tokens[4]);
    StatementNode* statement = Statement(node.m_generated_tokens[6]);

    vector<ASTNode*> children = {expression, statementNoShortIf, statement};
    
    return IfThenElseStatementNode(children);
}

IfThenElseStatementNoShortIfNode* IfThenElseStatementNoShortIf(const Token& node) {
    ExpressionNode* expression = Expression(node.m_generated_tokens[2]);
    StatementNoShortIfNode* statementNoShortIf1 = StatementNoShortIf(node.m_generated_tokens[4]);
    StatementNoShortIfNode* statementNoShortIf2 = StatementNoShortIf(node.m_generated_tokens[6]);

    vector<ASTNode*> children = {expression, statementNoShortIf1, statementNoShortIf2};

    return IfThenElseStatementNode(children);
}

WhileStatementNode* WhileStatement(const Token& node) {
    ExpressionNode* expression = Expression(node.m_generated_tokens[2]);
    StatementNode* statement = Statement(node.m_generated_tokens[4]);

    vector<ASTNode*> children = {expression, statement};

    return new WhileStatementNode(children);

}

WhileStatementNoShortIfNode* WhileStatementNoShortIf(const Token& node) {
    ExpressionNode* expression = Expression(node.m_generated_tokens[2]);
    StatementNoShortIfNode* statementNoShortIf = StatementNoShortIf(node.m_generated_tokens[4]);

    vector<ASTNode*> children = {expression, statementNoShortIf};

    return WhileStatementNoShortIfNode(children);
}

ForStatementNode* ForStatement(const Token& node) {
    LocalVariableDeclarationNode* localVariableDeclaration = nullptr;
    StatementExpressionNode* statementExpressionForInit = nullptr;
    StatementExpressionNode* statementExpressionForUpdate = nullptr;
    ExpressionNode* expression = nullptr;
    StatementNode* statement = nullptr;

    vector<ASTNode*> children;

    for (Token& child: node.m_generated_tokens) {
        if (child.m_type == TokenType::ForInit) {
            if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::StatementExpression) {
                statementExpressionForInit = StatementExpression(child.m_generated_tokens[0].m_generated_tokens[0]);
                children.push_back(statementExpressionForInit);
            }
            else if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::LocalVariableDeclaration) {
                localVariableDeclaration = LocalVariableDeclaration(child.m_generated_tokens[0].m_generated_tokens[0]);
                children.push_back(localVariableDeclaration);
            }
        }
        else if (child.m_type == TokenType::Expression) {
            expression = Expression(child);
            children.push_back(expression);
        }
        else if (child.m_type == TokenType::ForUpdate) {
            statementExpressionForUpdate = StatementExpression(child.m_generated_tokens[0]);
            children.push_back(statementExpressionForUpdate);
        }
        else if (child.m_type == TokenType::Statement) {
             statement= Statement(child);
             children.push_back(statement);
        }
    }

    return new ForStatementNode(children);
}

ForStatementNoShortIfNode* ForStatementNoShortIf(const Token& node) {
    LocalVariableDeclarationNode* localVariableDeclaration = nullptr;
    StatementExpressionNode* statementExpressionForInit = nullptr;
    StatementExpressionNode* statementExpressionForUpdate = nullptr;
    ExpressionNode* expression = nullptr;
    StatementNoShortIfNode* statementNoShortIf = nullptr;
   
    vector<ASTNode*> children;

    for (Token& child: node.m_generated_tokens) {
        if (child.m_type == TokenType::ForInit) {
            if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::StatementExpression) {
                statementExpressionForInit = StatementExpression(child.m_generated_tokens[0].m_generated_tokens[0]);
                children.push_back(statementExpressionForInit);
            }
            else if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::LocalVariableDeclaration) {
                localVariableDeclaration = LocalVariableDeclaration(child.m_generated_tokens[0].m_generated_tokens[0]);
                children.push_back(localVariableDeclaration);
            }
        }
        else if (child.m_type == TokenType::Expression) {
            expression = Expression(child);
            children.push_back(expression);
        }
        else if (child.m_type == TokenType::ForUpdate) {
            statementExpressionForUpdate = StatementExpression(child.m_generated_tokens[0]);
            children.push_back(statementExpressionForUpdate);
        }
        else if (child.m_type == TokenType::StatementNoShortIf) {
             statementNoShortIf = StatementNoShortIf(child);
             children.push_back(statementNoShortIf);
        }
    }

    return new ForStatementNoShortIfNode(children);
}

vector<InterfaceNode*> ExtendsInterfaces(const Token& node) {
    Token& child = node.m_generated_tokens[1];
    vector<InterfaceNode*> interfaces;
    while (child.m_generated_tokens.size() > 2) {
        interfaces.push_back(new InterfaceNode(ClassOrInterfaceType(child.m_generated_tokens[2].m_generated_tokens[0])));
        child = child.m_generated_tokens[0];
    }

    interfaces.push_back(new InterfaceNode(ClassOrInterfaceType(child.m_generated_tokens[1].m_generated_tokens[0])));
    reverse(interfaces.begin(), interfaces.end());

    return interfaces;
}

vector<InterfaceNode*> Interfaces(const Token& node) {
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

vector<ModifierNode*> Modifiers(const Token& node) {
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

FieldDeclarationNode* FieldDeclaration(const Token& node) {
    ModifiersNode* modifiers = Modifiers(node.m_generated_tokens[0]);
    TypeNode* type = Type(node.m_generated_tokens[1]);
    VariableDeclaratorNode* variableDeclarator = VariableDeclarator(node.m_generated_tokens[2].m_generated_tokens[0]);

    vector<ASTNode*> children;

    for (int i=0; i<modifiers.size(); i++) {
        children.push_back(modifiers[i]);
    }

    children.push_back(type, variableDeclarator);

    return new FieldDeclarationNode(children);
}

MethodDeclarationNode* MethodDeclaration(const Token& node) {
    MethodHeaderNode* methodHeader = MethodHeader(node.m_generated_tokens[0]);
    BlockNode* block = nullptr;

    vector<ASTNode*> children;
    children.push_back(methodHeader);

    if (node.m_generated_tokens[1].m_generated_tokens[0].m_type == TokenType::Block) {
        block = Block(node.m_generated_tokens[1].m_generated_tokens[0]);
        children.push_back(block);
    }
     
    return new MethodDeclarationNode(children);
}

MethodHeaderNode* MethodHeader(const Token& node) {
    ModifiersNode* modifiers = Modifiers(node.m_generated_tokens[0]);
    TypeNode* type = nullptr;
    vector<ASTNode*> children;

    for (int i=0; i<modifiers.size(); i++) {
        children.push_back(modifiers[i]);
    }

    if (node.m_generated_tokens[1].m_type == TokenType::Type) {
        type = Type(node.m_generated_tokens[1]);
        children.push_back(type);
    }
    
    MethodDeclaratorNode* methodDeclarator = MethodDeclarator(node.m_generated_tokens[2]);
    children.push_back(methodDeclarator);

    return new MethodHeaderNode(children);

}

MethodDeclaratorNode* MethodDeclarator(const Token& node) {
    string identifier = node.m_generated_tokens[0].m_lex;
    vector<FormalParameterNode*> formalParameters;

    vector<ASTNode*> children;

    if (node.m_generated_tokens[2].m_type == TokenType::FormalParameterList) {
        formalParameters = FormalParameterList(node.m_generated_tokens[2]);
        for (int i=0; i<formalParameters.size(); i++) {
            children.push_back(formalParameters[i]);
        }
    }

    return new MethodDeclaratorNode(identifier, children);
}

PrimaryNoNewArrayNode* PrimaryNoNewArray(const Token& node) {
    vector<ASTNode*> children;

    return new PrimaryNoNewArrayNode(children);
}

ArrayCreationExpressionNode* ArrayCreationExpression(const Token& node) {
    PrimitiveTypeNode* primitiveType = nullptr;
    NameNode* name = nullptr;

    vector<ASTNode*> children;
    ExpressionNode* expression = Expression(node.m_generated_tokens[2].m_generated_tokens[1]);
    children.push_back(expression);
    if (node.m_generated_tokens[1].m_type == TokenType::PrimitiveType) {
        primitiveType = PrimitiveType(node.m_generated_tokens[1]);
        children.push_back(primitiveType);
    }
    else {
        name = ClassOrInterfaceType(node.m_generated_tokens[1]);
        children.push_back(name);
    }

    return new ArrayCreationExpressionNode(children);
}

vector<ExpressionNode*> ArgumentList(const Token& node) {
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

ClassInstanceCreationExpressionNode* ClassInstanceCreationExpression(const Token& node) {
    ClassOrInterfaceTypeNode* classOrInterfaceType = ClassOrInterfaceType(node.m_generated_tokens[1]);
    vector<ExpressionNode*> argumentsList;
    vector<ASTNode*> children;
    childen.push_back(classOrInterfaceType);
    if (node.m_generated_tokens[3].m_type == TokenType::ArgumentList) {
        argumentList = ArgumentList(node.m_generated_tokens[3]);
        for (int i=0; i<argumentList.size(); i++) {
            children.push_back(argumentList[i]);
        }
    }

    return new ClassInstanceCreationExpressionNode(children);
}

ExpressionNode* Expression(const Token& node) {
    return AssignmentExpression(node.m_generated_tokens[0]);
}

AssignmentExpressionNode* AssignmentExpression(const Token& node) {
    if (node.m_generated_tokens[0].m_type == TokenType::Assignment) {
        return Assignment(node.m_generated_tokens[0]);
    }
    else {
        return ConditionalOrExpression(node.m_generated_tokens[0].m_generated_tokens[0]);
    }  
}

AssignmentNode* Assignment(const Token& node) {
    LeftHandSideNode* leftHandSide = LeftHandSide(node.m_generated_tokens[0]);
    AssignmentExpressionNode* assignmentExpression = AssignmentExpression(node.m_generated_tokens[2]);

    vector<ASTNode*> children = {leftHandSide, assignmentExpression};

    return new AssignmentNode(children);
}

LeftHandSideNode* LeftHandSide(const Token& node) {
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

ConditionalOrExpressionNode* ConditionalOrExpression(const Token& node) {
    vector<ConditionalAndExpressionNode*> conditionalAndExpressions;
    Token& child = node.m_generated_tokens[0];
    vector<ASTNode*> children;
    while (child.m_generated_tokens.size() > 1) {
        conditionalAndExpressions.push_back(ConditionalAndExpression(child.m_generated_tokens[2]));
        child = child.m_generated_tokens[0];
    }

    conditionalAndExpressions.push_back(ConditionalAndExpression(child.m_generated_tokens[0]));
    reverse(conditionalAndExpressions.begin(), conditionalAndExpressions.end());

    return new ConditionalOrExpressionNode(conditionalAndExpressions);
}

ConditionalAndExpressionNode* ConditionalAndExpression(const Token& node) {
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

InclusiveOrExpressionNode* InclusiveOrExpression(const Token& node) {
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

ExclusiveOrExpressionNode* ExclusiveOrExpression(const Token& node) {
    return AndExpression(node.m_generated_tokens[0]);
}

AndExpressionNode* AndExpression(const Token& node) {
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

EqualityExpressionNode* EqualityExpression(const Token& node) {
    RelationalExpressionNode* relationalExpression = nullptr;
    EqualityExpressionNode* equalityExpression = nullptr;
    int op = -1;

    vector<ASTNode*> children;

    if (node.m_generated_tokens.size() == 1) {
        children.push_back(RelationalExpression(node.m_generated_tokens[0]));
        return new EqualityExpressionNode((op, children);
    }

    if (node.m_generated_tokens[1].m_type == TokenType::T_EQUAL_EQUAL) {
        op = 0;
        children.push_back(EqualityExpression(node.m_generated_tokens[0]));
        children.push_back(RelationalExpression(node.m_generated_tokens[2]));
        return new EqualityExpressionNode(op, children);
    }
    else if (node.m_generated_tokens[1].m_type == TokenType::T_NOT_EQUAL) {
        op = 1;
        children.push_back(EqualityExpression(node.m_generated_tokens[0]));
        children.push_back(RelationalExpression(node.m_generated_tokens[2]));
        return new EqualityExpressionNode(op, children);
    }
}

RelationalExpressionNode* RelationalExpression(const Token& node) {
    RelationalExpressionNode* relationalExpression = nullptr;
    ShiftExpressionNode* shiftExpression = nullptr;
    ReferenceTypeNode* referenceType = nullptr;

    int op = -1;

     vector<ASTNode*> children;

    if (node.m_generated_tokens.size() == 1) {
        children.push_back(ShiftExpression(node.m_generated_tokens[0]));
        return new RelationalExpressionNode(op, children);
    }

    children.push_back(RelationalExpression(node.m_generated_tokens[0]));

    if (node.m_generated_tokens[1].m_type == TokenType::T_LESS) {
        op = 0;
        children.push_back(ShiftExpression(node.m_generated_tokens[2]));
        return new RelationalExpressionNode(op, children); 
    }
    else if (node.m_generated_tokens[1].m_type == TokenType::T_GREATER) {
        op = 1;
        children.push_back(ShiftExpression(node.m_generated_tokens[2]));
        return new RelationalExpressionNode(op, children); 
    }
    else if (node.m_generated_tokens[1].m_type == TokenType::T_LESS_EQUAL) {
        op = 2;
        children.push_back(ShiftExpression(node.m_generated_tokens[2]));
        return new RelationalExpressionNode(op, children); 
    }
    else if (node.m_generated_tokens[1].m_type == TokenType::T_GREATER_EQUAL) {
        op = 3;
        children.push_back(ShiftExpression(node.m_generated_tokens[2]));
        return new RelationalExpressionNode(op, children); 
    }
    else {
        op = 4;
        children.push_back(ReferenceType(node.m_generated_tokens[2]));
        return new RelationalExpressionNode(op, children);
    }
}

ShiftExpressionNode* ShiftExpression(const Token& node) {
    return AdditiveExpression(node.m_generated_tokens[0]); 
}

AdditiveExpressionNode* AdditiveExpression(const Token& node) {
    MultiplicativeExpression* multiplicativeExpression = nullptr;
    AdditiveExpression* additiveExpression = nullptr;
    int op = -1;

    vector<ASTNode*> children;

    if (node.m_generated_tokens.size() == 1) {
        children.push_back(MultiplicativeExpression(node.m_generated_tokens[0]));
        return new AdditiveExpressionNode(op, children);
    }

    children.push_back(AdditiveExpression(node.m_generated_tokens[0]));
    children.push_back(MultiplicativeExpression(node.m_generated_tokens[2]));

    if (node.m_generated_tokens[1].m_type == TokenType::T_STAR) {
        op = 0;
        return new AdditiveExpressionNode(op, children);
    }
    else if (node.m_generated_tokens[1].m_type == TokenType::T_SLASH) {
        op = 1;
        return new AdditiveExpressionNode(op, children);
    }

}

MultiplicativeExpressionNode* MultiplicativeExpression(const Token& node) {
    UnaryExpressionNode* unaryExpression = nullptr;
    MultiplicativeExpressionNode* multiplicativeExpression = nullptr;
    int op = -1;
    vector<ASTNode*> children;
    return new MultiplicativeExpressionNode(children);
}

FieldAccessNode* FieldAccess(const Token& node) {
    PrimaryNode* primaryNode = nullptr;
    string identifier = node.m_generated_tokens[2].m_lex;

    vector<ASTNode*> children;
    if (node.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::PrimaryNoNewArray) {
        children.push_back(PrimaryNoNewArray(node.m_generated_tokens[0].m_generated_tokens[0]);
    }
    else {
        children.push_back(ArrayCreationExpression(node.m_generated_tokens[0].m_generated_tokens[0]);
    }

    return new FieldAccessNode(identifier, children);
}


