#include "astNodes.h"
#include "token.h"
#include "astProcedures.h"

using namespace std;

TypeNode* TypeCreate(const Token& node) {
    if (node.m_generated_tokens[0].m_type == TokenType::PrimitiveType) {
        return PrimitiveTypeCreate(node.m_generated_tokens[0]);
    }
    else {
        return ReferenceTypeCreate(node.m_generated_tokens[0]);
    }
}

PrimitiveTypeNode* PrimitiveTypeCreate(const Token& node) {
    if (node.m_generated_tokens[0].m_type == TokenType::NumericType) {
        return new PrimitiveTypeNode(node.m_generated_tokens[0].m_generated_tokens[0].m_lex);
    }
    
    return new PrimitiveTypeNode(node.m_generated_tokens[0].m_lex);
}

ReferenceTypeNode* ReferenceTypeCreate(const Token& node) {
    if (node.m_generated_tokens[0].m_type == TokenType::ClassOrInterfaceType) {
        return ClassOrInterfaceTypeCreate(node.m_generated_tokens[0]);
    }
    else {
        return ArrayTypeCreate(node.m_generated_tokens[0]);
    }    
}

NameNode* ClassOrInterfaceTypeCreate(const Token& node) {
    return NameCreate(node.m_generated_tokens[0]);
}

ArrayTypeNode* ArrayTypeCreate(const Token& node) {
   PrimitiveTypeNode* primitiveTypeNode = nullptr;
   NameNode* nameNode = nullptr;
   if (node.m_generated_tokens[0].m_type == TokenType::PrimitiveType) {
        primitiveTypeNode = PrimitiveTypeCreate(node.m_generated_tokens[0]);
        vector<ASTNode*> children = {primitiveTypeNode};
        return new ArrayTypeNode(children);
    }
    else {
        vector<ASTNode*> children = {nameNode};
        return new ArrayTypeNode(children);
    }
}

NameNode* NameCreate(const Token& node) {
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
            name = NameCreate(child.m_generated_tokens[1]);
            children.push_back(name);
        }
        else if (child.m_type == TokenType::ImportDeclarations) {
            importDeclarations = ImportDeclarationsCreate(child);
            for (int i=0; i<importDeclarations.size(); i++) {
                children.push_back(importDeclarations[i]);
            }
        }
        else if (child.m_type == TokenType::TypeDeclarations) {
            if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::ClassDeclaration) {
                classDeclarationNode = ClassDeclarationCreate(child.m_generated_tokens[0].m_generated_tokens[0]);
                children.push_back(classDeclarationNode);
            }
            else if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::InterfaceDeclaration) {
                interfaceDeclarationNode = InterfaceDeclarationCreate(child.m_generated_tokens[0].m_generated_tokens[0]);
                children.push_back(interfaceDeclarationNode);
            }
        }
    }

    return new CompilationUnitNode(children);
}

vector<ImportDeclarationNode*> ImportDeclarationsCreate(const Token& node) {
    Token& child = node.m_generated_tokens[0];
    vector<ImportDeclarationNode*> importDeclarations;
    while (child.m_generated_tokens.size() > 1) {
        importDeclarations.push_back(ImportDeclarationCreate(child.m_generated_tokens[1]));
        child = child.m_generated_tokens[0];
    }

    importDeclarations.push_back(ImportDeclarationCreate(child.m_generated_tokens[0]));
    reverse(importDeclarations.begin(), importDeclarations.end());

    return importDeclarations;
}

ImportDeclarationNode* ImportDeclarationCreate(const Token& node) {
    if (node.m_generated_tokens[0].m_type == TokenType::SingleTypeImportDeclaration) {
        vector<ASTNode*> children = {NameCreate(node.m_generated_tokens[0].m_generated_tokens[1])};
        return new ImportDeclarationNode(0, children);
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::TypeImportOnDemandDeclaration) {
        vector<ASTNode*> children = {NameCreate(node.m_generated_tokens[0].m_generated_tokens[1])};
        return new ImportDeclarationNode(1, children);
    }
}

ClassDeclarationNode* ClassDeclarationCreate(const Token& node) {
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
            modifiers = ModifiersCreate(child);
            for (int i=0; i<modifiers.size(); i++) {
                children.push_back(modifiers[i]);
            }
        }
        else if (child.m_type == TokenType::T_IDENTIFIER) {
            identifier = child.m_lex;
        }
        else if (child.m_type == TokenType::Super) {
            name = ClassOrInterfaceTypeCreate(Classchild.m_generated_tokens[1].m_generated_tokens[0]);
            children.push_back(name);
        }
        else if (child.m_type == TokenType::Interfaces) {
            interfaces = InterfacesCreate(child);
            for (int i=0; i<interfaces.size(); i++) {
                children.push_back(interfaces[i]);
            }
        }
        else if (child.m_type == TokenType::ClassBody) {
            if (child.m_generated_tokens[1] == TokenType::ClassBodyDeclarations) {
                classBodyDeclarations = ClassBodyDeclarationsCreate(child.m_generated_tokens[1]);
                for (int i=0; i<classBodyDeclarations.size(); i++) {
                    children.push_back(classBodyDeclarations[i]); 
                }
            }
        }
    }

    return new ClassDeclarationNode(identifier, children);
}

InterfaceDeclarationNode* InterfaceDeclarationCreate(const Token& node) {
    vector<ModifierNode*> modifiers;
    string identifier;
    NameNode* name = nullptr;
    vector<InterfaceNode*> interfaces;
    vector<MethodHeaderNode*> methodHeaders;
    vector<ASTNode*> children;
    for (Token& child: node.m_generated_tokens) {
        if (child.m_type == TokenType::Modifiers) {
            modifiers = ModifiersCreate(child);
            for (int i=0; i<modifiers.size(); i++) {
                children.push_back(modifiers[i]);
            }
        }
        else if (child.m_type == TokenType::T_IDENTIFIER) {
            identifier = child.m_lex;
        }
        else if (child.m_type == TokenType::ExtendsInterfaces) {
            interfaces = ExtendsInterfacesCreate(child);
            for (int i=0; i<interfaces.size(); i++) {
                children.push_back(interfaces[i]);
            }
        }
        else if (child.m_type == TokenType::ClassBody) {
            if (child.m_generated_tokens[1] == TokenType::InterfaceMemberDeclarations) {
                interfaceMemberDeclarations = InterfaceMemberDeclarationsCreate(child.m_generated_tokens[1]);
                for (int i=0; i<interfaceMemberDeclarations.size(); i++) {
                    children.push_back(interfaceMemberDeclarations[i]);
                }
            }
        }
    }

    return new InterfaceDeclarationNode(identifier, children);

}

vector<MethodHeaderNode*> InterfaceMemberDeclarationsCreate(const Token& node) {
    vector<MethodHeaderNode*> methodHeaders;
    Token& child = node.m_generated_tokens[0];
    while (child.m_generated_tokens.size() > 1) {
        methodHeaders.push_back(MethodHeaderCreate(child.m_generated_tokens[1].m_generated_tokens[0].m_generated_tokens[0]));
        child = child.m_generated_tokens[0];
    }

    methodHeaders.push_back(MethodHeaderCreate(child.m_generated_tokens[1].m_generated_tokens[0].m_generated_tokens[0]));
    reverse(methodHeaders.begin(), methodHeaders.end());

    return methodHeaders;
}

vector<ClassBodyDeclarationNode*> ClassBodyDeclarationsCreate(const Token& node) {
    Token& child = node.m_generated_tokens[0];
    vector<ClassBodyDeclarationNode*> classBodyDeclarations;
    while (child.m_generated_tokens.size() > 1) {
        if (child.m_generated_tokens[1].m_generated_tokens[0].m_type == TokenType::ClassMemberDeclaration) {
            if (child.m_generated_tokens[1].m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::FieldDeclaration) {
                classBodyDeclarations.push_back(FieldDeclarationCreate(child.m_generated_tokens[1].m_generated_tokens[0].m_generated_tokens[0]);
            }
            else if (child.m_generated_tokens[1].m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::MethodDeclaration) {
                classBodyDeclarations.push_back(MethodDeclarationCreate(child.m_generated_tokens[1].m_generated_tokens[0].m_generated_tokens[0]);
            }
        }
        else if (child.m_generated_tokens[1].m_generated_tokens[0].m_type == TokenType::ConstructorDeclaration) {
            classBodyDeclarations.push_back(ConstructorDeclarationCreate(child.m_generated_tokens[1].m_generated_tokens[0]);
        }
        child = child.m_generated_tokens[0];
    }

    if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::ClassMemberDeclaration) {
        if (child.m_generated_tokens[0].m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::FieldDeclaration) {
            classBodyDeclarations.push_back(FieldDeclarationCreate(child.m_generated_tokens[0].m_generated_tokens[0].m_generated_tokens[0]);
        }
        else if (child.m_generated_tokens[0].m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::MethodDeclaration) {
            classBodyDeclarations.push_back(MethodDeclarationCreate(child.m_generated_tokens[0].m_generated_tokens[0].m_generated_tokens[0]);
        }
    }
    else if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::ConstructorDeclaration) {
        classBodyDeclarations.push_back(ConstructorDeclarationCreate(child.m_generated_tokens[0].m_generated_tokens[0]);
    }

    reverse(classBodyDeclarations.begin(), classBodyDeclarations.end());

    return classBodyDeclarations;
}

ConstructorDeclarationNode* ConstructorDeclarationCreate(const Token& node) {
    vector<ASTNode*> children;
    vector<ModifierNode*> modifiers = ModifiersCreate(node.m_generated_tokens[0]);
    for (int i=0; i<modifiers.size(); i++) {
        children.push_back(modifiers[i]);
    }
    ConstructorDeclaratorNode* constructorDeclaratorNode = ConstructorDeclaratorCreate(node.m_generated_tokens[1]);
    children.push_back(constructorDeclaratorNode);
    vector<BlockStatementNode*> blockStatements;
    if (node.m_generated_tokens[2].m_generated_tokens[1].m_type == TokenType::BlockStatements) {
        blockStatements = BlockStatementsCreate(node.m_generated_tokens[2].m_generated_tokens[1]);
        for (int i=0; i<blockStatements.size(); i++) {
            children.push_back(blockStatements[i]);
        }  
    }

    return new ConstructorDeclarationNode(children);
}

ConstructorDeclaratorNode* ConstructorDeclaratorCreate(const Token& node) {
    string simpleName = node.m_generated_tokens[0].m_generated_tokens[0].m_lex;
    vector<FormalParameterNode*> formalParameters;
    vector<ASTNode*> children;
    if (node.m_generated_tokens[0].m_generated_tokens[2].m_type == TokenType::FormalParameterList) {
        formalParameters = FormalParameterListCreate(node.m_generated_tokens[0].m_generated_tokens[2]);
        for (int i=0; i<formalParameters.size(); i++) {
            children.push_back(formalParameters[i]);
        }
    }

    return new ConstructorDeclaratorNode(simpleName, children);
}

vector<FormalParameterNode*> FormalParameterListCreate(const Token& node) {
    Token& child = node.m_generated_tokens[0];
    vector<FormalParameterNode*> formalParameters;
    while (child.m_generated_tokens.size() > 1) {
        formalParameters.push_back(FormalParameterCreate(child.m_generated_tokens[1]));
        child = child.m_generated_tokens[0];
    }

    formalParameters.push_back(FormalParameterCreate(child.m_generated_tokens[0]));
    reverse(formalParameters.begin(), formalParameters.end());

    return formalParameters;
}

FormalParameterNode* FormalParameterCreate(const Token& node) {
    TypeNode* type = TypeCreate(node.m_generated_tokens[0]);
    string identifier = node.m_generated_tokens[1].m_generated_tokens[0].m_lex;
    vector<ASTNode*> children = {type};

    return new FormalParameterNode(identifier, children);
}

vector<BlockStatementNode*> BlockStatementsCreate(const Token& node) {
    Token& child = node.m_generated_tokens[0];
    vector<BlockStatementNode*> blockStatements;
    while (child.m_generated_tokens.size() > 1) {
        if (child.m_generated_tokens[1].m_generated_tokens[0].m_type == TokenType::LocalVariableDeclarationStatement) {
            blockStatements.push_back(LocalVariableDeclarationCreate(child.m_generated_tokens[1].m_generated_tokens[0].m_generated_tokens[0]);
        }
        else if (child.m_generated_tokens[1].m_generated_tokens[0].m_type == TokenType::Statement) {
            blockStatements.push_back(StatementCreate(child.m_generated_tokens[1].m_generated_tokens[0]);
        }
        child = child.m_generated_tokens[0];
    }

    if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::LocalVariableDeclarationStatement) {
        blockStatements.push_back(LocalVariableDeclarationCreate(child.m_generated_tokens[0].m_generated_tokens[0].m_generated_tokens[0]);
    }
    else if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::Statement) {
        blockStatements.push_back(StatementCreate(child.m_generated_tokens[0].m_generated_tokens[0]);
    }

    reverse(blockStatements.begin(), blockStatements.end());

    return blockStatements;

}

LocalVariableDeclarationNode* LocalVariableDeclarationCreate(const Token& node) {
    TypeNode* type = TypeCreate(node.m_generated_tokens[0]);
    VariableDeclaratorNode* variableDeclarator = VariableDeclaratorCreate(node.m_generated_tokens[1]);
    vector<ASTNode*> children = {type, variableDeclarator};

    return new LocalVariableDeclarationNode(children);
}

VariableDeclaratorNode* VariableDeclaratorCreate(const Token& node) {
    string identifier = node.m_generated_tokens[0];
    ExpressionNode* expression = nullptr;
    vector<ASTNode*> children;
    if (node.m_generated_tokens.size() > 1) {
        expression = ExpressionCreate(node.m_generated_tokens[2].m_generated_tokens[0]);
        children.push_back(expression);
    }

    return new VariableDeclaratorNode(identifier, children);
}

StatementNode* StatementCreate(const Token& node) {
    StatementNode* statementNode = nullptr;
    if (node.m_generated_tokens[0].m_type == TokenType::StatementWithoutTrailingSubstatement) {
        statementNode = StatementWithoutTrailingSubstatementCreate(node.m_generated_tokens[0]); 
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::IfThenStatement) {
        statementNode = IfThenStatementCreate(node.m_generated_tokens[0]);
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::IfThenElseStatement) {
        statementNode = IfThenElseStatementCreate(node.m_generated_tokens[0]);
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::WhileStatement) {
        statementNode = WhileStatementCreate(node.m_generated_tokens[0]);
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::ForStatement) {
        statementNode = ForStatementCreate(node.m_generated_tokens[0]);
    }

    return statementNode;
}

StatementNoShortIfNode* StatementNoShortIfCreate(const Token& node) {
    StatementNoShortIfNode* statementNoShortIfNode = nullptr;
    if (node.m_generated_tokens[0].m_type == TokenType::StatementWithoutTrailingSubstatement) {
        statementNoShortIfNode = StatementWithoutTrailingSubstatementCreate(node.m_generated_tokens[0]);
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::IfThenElseStatementNoShortIf) {
        statementNoShortIfNode = IfThenElseStatementNoShortIfCreate(node.m_generated_tokens[0]);
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::WhileStatementNoShortIf) {
        statementNoShortIfNode = WhileStatementNoShortIfCreate(node.m_generated_tokens[0]);
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::ForStatementNoShortIf) {
        statementNoShortIfNode = ForStatementNoShortIfCreate(node.m_generated_tokens[0]);
    }

    return statementNoShortIfNode;
}

StatementWithoutTrailingSubstatementNode* StatementWithoutTrailingSubstatementCreate(const Token& node) {
    StatementWithoutTrailingSubstatementNode* statementWithoutTrailingSubstatementNode = nullptr;
    if (node.m_generated_tokens[0].m_type == TokenType::Block) {
        if (node.m_generated_tokens[0].m_generated_tokens[1].m_type == TokenType::BlockStatements) {
            statementWithoutTrailingSubstatementNode = new BlockNode(BlockStatementsCreate(node.m_generated_tokens[0].m_generated_tokens[1]));
        }
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::ExpressionStatement) {
        statementWithoutTrailingSubstatementNode = StatementExpressionCreate(node.m_generated_tokens[0].m_generated_tokens[0]);
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::ReturnStatement) {
        statementWithoutTrailingSubstatementNode = ReturnStatementCreate(node.m_generated_tokens[0]);
    }

    return statementWithoutTrailingSubstatementNode;
}

ReturnStatementNode* ReturnStatementCreate(const Token& node) {
    ExpressionNode* expression = nullptr;
    vector<ASTNode*> children;
    if (node.m_generated_tokens[1].m_type == TokenType::Expression) {
        expression = ExpressionCreate(node.m_generated_tokens[1]);
        children.push_back(expression);
    }

    return new ReturnStatementNode(children);
}

StatementExpressionNode* StatementExpressionCreate(const Token& node) {
    StatementExpressionNode* statementExpressionNode = nullptr;
    if (node.m_generated_tokens[0].m_type == TokenType::Assignment) {
        statementExpressionNode = AssignmentCreate(node.m_generated_tokens[0]);
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::MethodInvocation) {
        statementExpressionNode = MethodInvocation(node.m_generated_tokens[0]);
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::ClassInstanceCreationExpression) {
        statementExpressionNode = ClassInstanceCreationExpressionCreate(node.m_generated_tokens[0]);
    }

    return statementExpressionNode;
}

IfThenStatementNode* IfThenStatementCreate(const Token& node) {
    ExpressionNode* expression = ExpressionCreate(node.m_generated_tokens[2]);
    StatementNode* statement = StatementCreate(node.m_generated_tokens[4]);

    vector<ASTNode*> children = {expression, statement};

    return new IfThenStatementNode(children);
}

IfThenElseStatementNode* IfThenElseStatementCreate(const Token& node) {
    ExpressionNode* expression = ExpressionCreate(node.m_generated_tokens[2]);
    StatementNoShortIfNode* statementNoShortIf = StatementNoShortIfCreate(node.m_generated_tokens[4]);
    StatementNode* statement = StatementCreate(node.m_generated_tokens[6]);

    vector<ASTNode*> children = {expression, statementNoShortIf, statement};
    
    return IfThenElseStatementNode(children);
}

IfThenElseStatementNoShortIfNode* IfThenElseStatementNoShortIfCreate(const Token& node) {
    ExpressionNode* expression = ExpressionCreate(node.m_generated_tokens[2]);
    StatementNoShortIfNode* statementNoShortIf1 = StatementNoShortIfCreate(node.m_generated_tokens[4]);
    StatementNoShortIfNode* statementNoShortIf2 = StatementNoShortIfCreate(node.m_generated_tokens[6]);

    vector<ASTNode*> children = {expression, statementNoShortIf1, statementNoShortIf2};

    return IfThenElseStatementNode(children);
}

WhileStatementNode* WhileStatementCreate(const Token& node) {
    ExpressionNode* expression = ExpressionCreate(node.m_generated_tokens[2]);
    StatementNode* statement = StatementCreate(node.m_generated_tokens[4]);

    vector<ASTNode*> children = {expression, statement};

    return new WhileStatementNode(children);

}

WhileStatementNoShortIfNode* WhileStatementNoShortIfCreate(const Token& node) {
    ExpressionNode* expression = ExpressionCreate(node.m_generated_tokens[2]);
    StatementNoShortIfNode* statementNoShortIf = StatementNoShortIfCreate(node.m_generated_tokens[4]);

    vector<ASTNode*> children = {expression, statementNoShortIf};

    return WhileStatementNoShortIfNode(children);
}

ForStatementNode* ForStatementCreate(const Token& node) {
    LocalVariableDeclarationNode* localVariableDeclaration = nullptr;
    StatementExpressionNode* statementExpressionForInit = nullptr;
    StatementExpressionNode* statementExpressionForUpdate = nullptr;
    ExpressionNode* expression = nullptr;
    StatementNode* statement = nullptr;

    vector<ASTNode*> children;

    for (Token& child: node.m_generated_tokens) {
        if (child.m_type == TokenType::ForInit) {
            if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::StatementExpression) {
                statementExpressionForInit = StatementExpressionCreate(child.m_generated_tokens[0].m_generated_tokens[0]);
                children.push_back(statementExpressionForInit);
            }
            else if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::LocalVariableDeclaration) {
                localVariableDeclaration = LocalVariableDeclarationCreate(child.m_generated_tokens[0].m_generated_tokens[0]);
                children.push_back(localVariableDeclaration);
            }
        }
        else if (child.m_type == TokenType::Expression) {
            expression = ExpressionCreate(child);
            children.push_back(expression);
        }
        else if (child.m_type == TokenType::ForUpdate) {
            statementExpressionForUpdate = StatementExpressionCreate(child.m_generated_tokens[0]);
            children.push_back(statementExpressionForUpdate);
        }
        else if (child.m_type == TokenType::Statement) {
             statement= StatementCreate(child);
             children.push_back(statement);
        }
    }

    return new ForStatementNode(children);
}

ForStatementNoShortIfNode* ForStatementNoShortIfCreate(const Token& node) {
    LocalVariableDeclarationNode* localVariableDeclaration = nullptr;
    StatementExpressionNode* statementExpressionForInit = nullptr;
    StatementExpressionNode* statementExpressionForUpdate = nullptr;
    ExpressionNode* expression = nullptr;
    StatementNoShortIfNode* statementNoShortIf = nullptr;
   
    vector<ASTNode*> children;

    for (Token& child: node.m_generated_tokens) {
        if (child.m_type == TokenType::ForInit) {
            if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::StatementExpression) {
                statementExpressionForInit = StatementExpressionCreate(child.m_generated_tokens[0].m_generated_tokens[0]);
                children.push_back(statementExpressionForInit);
            }
            else if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::LocalVariableDeclaration) {
                localVariableDeclaration = LocalVariableDeclarationCreate(child.m_generated_tokens[0].m_generated_tokens[0]);
                children.push_back(localVariableDeclaration);
            }
        }
        else if (child.m_type == TokenType::Expression) {
            expression = ExpressionCreate(child);
            children.push_back(expression);
        }
        else if (child.m_type == TokenType::ForUpdate) {
            statementExpressionForUpdate = StatementExpressionCreate(child.m_generated_tokens[0]);
            children.push_back(statementExpressionForUpdate);
        }
        else if (child.m_type == TokenType::StatementNoShortIf) {
             statementNoShortIf = StatementNoShortIfCreate(child);
             children.push_back(statementNoShortIf);
        }
    }

    return new ForStatementNoShortIfNode(children);
}

vector<InterfaceNode*> ExtendsInterfacesCreate(const Token& node) {
    Token& child = node.m_generated_tokens[1];
    vector<InterfaceNode*> interfaces;
    while (child.m_generated_tokens.size() > 2) {
        interfaces.push_back(new InterfaceNode(ClassOrInterfaceTypeCreate(child.m_generated_tokens[2].m_generated_tokens[0])));
        child = child.m_generated_tokens[0];
    }

    interfaces.push_back(new InterfaceNode(ClassOrInterfaceTypeCreate(child.m_generated_tokens[1].m_generated_tokens[0])));
    reverse(interfaces.begin(), interfaces.end());

    return interfaces;
}

vector<InterfaceNode*> InterfacesCreate(const Token& node) {
    Token& child = node.m_generated_tokens[0];
    vector<InterfaceNode*> interfaces;
    while (child.m_generated_tokens.size() > 1) {
        interfaces.push_back(new InterfaceNode(ClassOrInterfaceTypeCreate(child.m_generated_tokens[1].m_generated_tokens[0])));
        child = child.m_generated_tokens[0];
    }

    interfaces.push_back(new InterfaceNode(ClassOrInterfaceTypeCreate(child.m_generated_tokens[0].m_generated_tokens[0])));
    reverse(interfaces.begin(), interfaces.end());

    return interfaces;
}

vector<ModifierNode*> ModifiersCreate(const Token& node) {
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

FieldDeclarationNode* FieldDeclarationCreate(const Token& node) {
    vector<ModifierNode*> modifiers = ModifiersCreate(node.m_generated_tokens[0]);
    TypeNode* type = TypeCreate(node.m_generated_tokens[1]);
    VariableDeclaratorNode* variableDeclarator = VariableDeclaratorCreate(node.m_generated_tokens[2].m_generated_tokens[0]);

    vector<ASTNode*> children;

    for (int i=0; i<modifiers.size(); i++) {
        children.push_back(modifiers[i]);
    }

    children.push_back(type, variableDeclarator);

    return new FieldDeclarationNode(children);
}

MethodDeclarationNode* MethodDeclarationCreate(const Token& node) {
    MethodHeaderNode* methodHeader = MethodHeaderCreate(node.m_generated_tokens[0]);
    BlockNode* block = nullptr;

    vector<ASTNode*> children;
    children.push_back(methodHeader);

    if (node.m_generated_tokens[1].m_generated_tokens[0].m_type == TokenType::Block) {
        block = Block(node.m_generated_tokens[1].m_generated_tokens[0]);
        children.push_back(block);
    }
     
    return new MethodDeclarationNode(children);
}

MethodHeaderNode* MethodHeaderCreate(const Token& node) {
    ModifiersNode* modifiers = ModifiersCreate(node.m_generated_tokens[0]);
    TypeNode* type = nullptr;
    vector<ASTNode*> children;

    for (int i=0; i<modifiers.size(); i++) {
        children.push_back(modifiers[i]);
    }

    if (node.m_generated_tokens[1].m_type == TokenType::Type) {
        type = TypeCreate(node.m_generated_tokens[1]);
        children.push_back(type);
    }
    
    MethodDeclaratorNode* methodDeclarator = MethodDeclaratorCreate(node.m_generated_tokens[2]);
    children.push_back(methodDeclarator);

    return new MethodHeaderNode(children);

}

MethodDeclaratorNode* MethodDeclaratorCreate(const Token& node) {
    string identifier = node.m_generated_tokens[0].m_lex;
    vector<FormalParameterNode*> formalParameters;

    vector<ASTNode*> children;

    if (node.m_generated_tokens[2].m_type == TokenType::FormalParameterList) {
        formalParameters = FormalParameterListCreate(node.m_generated_tokens[2]);
        for (int i=0; i<formalParameters.size(); i++) {
            children.push_back(formalParameters[i]);
        }
    }

    return new MethodDeclaratorNode(identifier, children);
}

PrimaryNoNewArrayNode* PrimaryNoNewArrayCreate(const Token& node) {
    vector<ASTNode*> children;

    return new PrimaryNoNewArrayNode(children);
}

ArrayCreationExpressionNode* ArrayCreationExpressionCreate(const Token& node) {
    PrimitiveTypeNode* primitiveType = nullptr;
    NameNode* name = nullptr;

    vector<ASTNode*> children;
    ExpressionNode* expression = ExpressionCreate(node.m_generated_tokens[2].m_generated_tokens[1]);
    children.push_back(expression);
    if (node.m_generated_tokens[1].m_type == TokenType::PrimitiveType) {
        primitiveType = PrimitiveTypeCreate(node.m_generated_tokens[1]);
        children.push_back(primitiveType);
    }
    else {
        name = ClassOrInterfaceTypeCreate(node.m_generated_tokens[1]);
        children.push_back(name);
    }

    return new ArrayCreationExpressionNode(children);
}

vector<ExpressionNode*> ArgumentListCreate(const Token& node) {
    Token& child = node.m_generated_tokens[0];
    vector<ExpressionNode*> expressions;
    while (child.m_generated_tokens.size() > 1) {
        expressions.push_back(ExpressionCreate(child.m_generated_tokens[2]));
        child = child.m_generated_tokens[0];
    }

    expressions.push_back(ExpressionCreate(child.m_generated_tokens[0]));
    reverse(expressions.begin(), expressions.end());

    return expressions;
}

ClassInstanceCreationExpressionNode* ClassInstanceCreationExpressionCreate(const Token& node) {
    ClassOrInterfaceTypeNode* classOrInterfaceType = ClassOrInterfaceTypeCreate(node.m_generated_tokens[1]);
    vector<ExpressionNode*> argumentsList;
    vector<ASTNode*> children;
    childen.push_back(classOrInterfaceType);
    if (node.m_generated_tokens[3].m_type == TokenType::ArgumentList) {
        argumentList = ArgumentListCreate(node.m_generated_tokens[3]);
        for (int i=0; i<argumentList.size(); i++) {
            children.push_back(argumentList[i]);
        }
    }

    return new ClassInstanceCreationExpressionNode(children);
}

ExpressionNode* ExpressionCreate(const Token& node) {
    return AssignmentExpression(node.m_generated_tokens[0]);
}

AssignmentExpressionNode* AssignmentExpression(const Token& node) {
    if (node.m_generated_tokens[0].m_type == TokenType::Assignment) {
        return AssignmentCreate(node.m_generated_tokens[0]);
    }
    else {
        return ConditionalOrExpressionCreate(node.m_generated_tokens[0].m_generated_tokens[0]);
    }  
}

AssignmentNode* AssignmentCreate(const Token& node) {
    LeftHandSideNode* leftHandSide = LeftHandSideCreate(node.m_generated_tokens[0]);
    AssignmentExpressionNode* assignmentExpression = AssignmentExpression(node.m_generated_tokens[2]);

    vector<ASTNode*> children = {leftHandSide, assignmentExpression};

    return new AssignmentNode(children);
}

LeftHandSideNode* LeftHandSideCreate(const Token& node) {
    LeftHandSideNode* leftHandSideNode = nullptr;
    if (node.m_generated_tokens[0].m_type == TokenType::Name) {
        statementExpressionNode = NameCreate(node.m_generated_tokens[0]);
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::FieldAccess) {
        statementExpressionNode = FieldAccessCreate(node.m_generated_tokens[0]);
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

ConditionalOrExpressionNode* ConditionalOrExpressionCreate(const Token& node) {
    vector<ConditionalAndExpressionNode*> conditionalAndExpressions;
    Token& child = node.m_generated_tokens[0];
    vector<ASTNode*> children;
    while (child.m_generated_tokens.size() > 1) {
        conditionalAndExpressions.push_back(ConditionalAndExpressionCreate(child.m_generated_tokens[2]));
        child = child.m_generated_tokens[0];
    }

    conditionalAndExpressions.push_back(ConditionalAndExpressionCreate(child.m_generated_tokens[0]));
    reverse(conditionalAndExpressions.begin(), conditionalAndExpressions.end());

    return new ConditionalOrExpressionNode(conditionalAndExpressions);
}

ConditionalAndExpressionNode* ConditionalAndExpressionCreate(const Token& node) {
    vector<InclusiveOrExpressionNode*> inclusiveOrExpressions;
    Token& child = node.m_generated_tokens[0];
    while (child.m_generated_tokens.size() > 1) {
        inclusiveOrExpressions.push_back(InclusiveOrExpressionCreate(child.m_generated_tokens[2]));
        child = child.m_generated_tokens[0];
    }

    inclusiveOrExpressions.push_back(InclusiveOrExpressionCreate(child.m_generated_tokens[0]));
    reverse(inclusiveOrExpressions.begin(), inclusiveOrExpressions.end());

    return new ConditionalAndExpressionNode(inclusiveOrExpressions);
}

InclusiveOrExpressionNode* InclusiveOrExpressionCreate(const Token& node) {
    vector<ExclusiveOrExpressionNode*> exclusiveOrExpressions;
    Token& child = node.m_generated_tokens[0];
    while (child.m_generated_tokens.size() > 1) {
        exclusiveOrExpressions.push_back(ExclusiveOrExpressionCreate(child.m_generated_tokens[2]));
        child = child.m_generated_tokens[0];
    }

    exclusiveOrExpressions.push_back(ExclusiveOrExpressionCreate(child.m_generated_tokens[0]));
    reverse(exclusiveOrExpressions.begin(), exclusiveOrExpressions.end());

    return new InclusiveOrExpressionNode(exclusiveOrExpressions);
}

ExclusiveOrExpressionNode* ExclusiveOrExpressionCreate(const Token& node) {
    return AndExpressionCreate(node.m_generated_tokens[0]);
}

AndExpressionNode* AndExpressionCreate(const Token& node) {
    vector<EqualityExpressionNode*> equalityExpressions;
    Token& child = node.m_generated_tokens[0];
    while (child.m_generated_tokens.size() > 1) {
        equalityExpressions.push_back(EqualityExpressionCreate(child.m_generated_tokens[2]));
        child = child.m_generated_tokens[0];
    }

    equalityExpressions.push_back(EqualityExpressionCreate(child.m_generated_tokens[0]));
    reverse(equalityExpressions.begin(), equalityExpressions.end());

    return new AndExpressionNode(equalityExpressions);
}

EqualityExpressionNode* EqualityExpressionCreate(const Token& node) {
    RelationalExpressionNode* relationalExpression = nullptr;
    EqualityExpressionNode* equalityExpression = nullptr;
    int op = -1;

    vector<ASTNode*> children;

    if (node.m_generated_tokens.size() == 1) {
        children.push_back(RelationalExpressionCreate(node.m_generated_tokens[0]));
        return new EqualityExpressionNode((op, children);
    }

    if (node.m_generated_tokens[1].m_type == TokenType::T_EQUAL_EQUAL) {
        op = 0;
        children.push_back(EqualityExpressionCreate(node.m_generated_tokens[0]));
        children.push_back(RelationalExpressionCreate(node.m_generated_tokens[2]));
        return new EqualityExpressionNode(op, children);
    }
    else if (node.m_generated_tokens[1].m_type == TokenType::T_NOT_EQUAL) {
        op = 1;
        children.push_back(EqualityExpressionCreate(node.m_generated_tokens[0]));
        children.push_back(RelationalExpressionCreate(node.m_generated_tokens[2]));
        return new EqualityExpressionNode(op, children);
    }
}

RelationalExpressionNode* RelationalExpressionCreate(const Token& node) {
    RelationalExpressionNode* relationalExpression = nullptr;
    ShiftExpressionNode* shiftExpression = nullptr;
    ReferenceTypeNode* referenceType = nullptr;

    int op = -1;

     vector<ASTNode*> children;

    if (node.m_generated_tokens.size() == 1) {
        children.push_back(ShiftExpressionCreate(node.m_generated_tokens[0]));
        return new RelationalExpressionNode(op, children);
    }

    children.push_back(RelationalExpressionCreate(node.m_generated_tokens[0]));

    if (node.m_generated_tokens[1].m_type == TokenType::T_LESS) {
        op = 0;
        children.push_back(ShiftExpressionCreate(node.m_generated_tokens[2]));
        return new RelationalExpressionNode(op, children); 
    }
    else if (node.m_generated_tokens[1].m_type == TokenType::T_GREATER) {
        op = 1;
        children.push_back(ShiftExpressionCreate(node.m_generated_tokens[2]));
        return new RelationalExpressionNode(op, children); 
    }
    else if (node.m_generated_tokens[1].m_type == TokenType::T_LESS_EQUAL) {
        op = 2;
        children.push_back(ShiftExpressionCreate(node.m_generated_tokens[2]));
        return new RelationalExpressionNode(op, children); 
    }
    else if (node.m_generated_tokens[1].m_type == TokenType::T_GREATER_EQUAL) {
        op = 3;
        children.push_back(ShiftExpressionCreate(node.m_generated_tokens[2]));
        return new RelationalExpressionNode(op, children); 
    }
    else {
        op = 4;
        children.push_back(ReferenceTypeCreate(node.m_generated_tokens[2]));
        return new RelationalExpressionNode(op, children);
    }
}

AdditiveExpressionNode* ShiftExpressionCreate(const Token& node) {
    return AdditiveExpressionCreate(node.m_generated_tokens[0]); 
}

AdditiveExpressionNode* AdditiveExpressionCreate(const Token& node) {
    MultiplicativeExpressionNode* multiplicativeExpression = nullptr;
    AdditiveExpressionNode* additiveExpression = nullptr;
    int op = -1;

    vector<ASTNode*> children;

    if (node.m_generated_tokens.size() == 1) {
        children.push_back(MultiplicativeExpressionCreate(node.m_generated_tokens[0]));
        return new AdditiveExpressionNode(op, children);
    }

    children.push_back(AdditiveExpressionCreate(node.m_generated_tokens[0]));
    children.push_back(MultiplicativeExpressionCreate(node.m_generated_tokens[2]));

    if (node.m_generated_tokens[1].m_type == TokenType::T_STAR) {
        op = 0;
        return new AdditiveExpressionNode(op, children);
    }
    else if (node.m_generated_tokens[1].m_type == TokenType::T_SLASH) {
        op = 1;
        return new AdditiveExpressionNode(op, children);
    }

}

MultiplicativeExpressionNode* MultiplicativeExpressionCreate(const Token& node) {
    UnaryExpressionNode* unaryExpression = nullptr;
    MultiplicativeExpressionNode* multiplicativeExpression = nullptr;
    int op = -1;
    vector<ASTNode*> children;
    return new MultiplicativeExpressionNode(children);
}

FieldAccessNode* FieldAccessCreate(const Token& node) {
    PrimaryNode* primaryNode = nullptr;
    string identifier = node.m_generated_tokens[2].m_lex;

    vector<ASTNode*> children;
    if (node.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::PrimaryNoNewArray) {
        children.push_back(PrimaryNoNewArrayCreate(node.m_generated_tokens[0].m_generated_tokens[0]));
    }
    else {
        children.push_back(ArrayCreationExpressionCreate(node.m_generated_tokens[0].m_generated_tokens[0]));
    }

    return new FieldAccessNode(identifier, children);
}


