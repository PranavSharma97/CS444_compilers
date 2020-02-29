#include "astProcedures.h"
#include<algorithm>
#include<iostream>
#include<assert.h>

using namespace std;

TypeNode* TypeCreate(Token node) {
    assert(node.m_type == Type);
    if (node.m_generated_tokens[0].m_type == TokenType::PrimitiveType) {
        return PrimitiveTypeCreate(node.m_generated_tokens[0]);
    }
    else {
        return ReferenceTypeCreate(node.m_generated_tokens[0]);
    }
}

PrimitiveTypeNode* PrimitiveTypeCreate(Token node) {
    assert(node.m_type == PrimitiveType);
    if (node.m_generated_tokens[0].m_type == TokenType::NumericType) {
        return new PrimitiveTypeNode(node.m_generated_tokens[0].m_generated_tokens[0].m_lex);
    }
    
    return new PrimitiveTypeNode(node.m_generated_tokens[0].m_lex);
}

ReferenceTypeNode* ReferenceTypeCreate(Token node) {
    assert(node.m_type == ReferenceType);
    if (node.m_generated_tokens[0].m_type == TokenType::ClassOrInterfaceType) {
        return ClassOrInterfaceTypeCreate(node.m_generated_tokens[0]);
    }
    else {
        return ArrayTypeCreate(node.m_generated_tokens[0]);
    }    
}

NameNode* ClassOrInterfaceTypeCreate(Token node) {
    assert(node.m_type == ClassOrInterfaceType);
    return NameCreate(node.m_generated_tokens[0]);
}

ArrayTypeNode* ArrayTypeCreate(Token node) {
   assert(node.m_type == ArrayType);
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

NameNode* NameCreate(Token node) {
    assert(node.m_type == Name);
    Token child = node.m_generated_tokens[0];
    vector<string> identifiers;
    while (child.m_generated_tokens[0].m_type == TokenType::QualifiedName) {
        identifiers.push_back(child.m_generated_tokens[0].m_generated_tokens[2].m_lex);
        child = child.m_generated_tokens[0];
    }

    identifiers.push_back(child.m_generated_tokens[0].m_generated_tokens[0].m_lex);
    reverse(identifiers.begin(), identifiers.end());

    return new NameNode(identifiers);
}

CompilationUnitNode* CompilationUnitCreate(Token node) {

    assert(node.m_type == Goal);

    node = node.m_generated_tokens[1];

    cout<<"\n\n"<<"IN COMPILATION UNIT\n\n";

    cout<<"Token Types of root and children: \n";
    cout<<node.m_type<<endl;
    cout<<node.m_generated_tokens[0].m_type<<endl;
    cout<<node.m_generated_tokens[0].m_generated_tokens[0].m_type;

    NameNode* name = nullptr;
    vector<ImportDeclarationNode*> importDeclarations;
    ClassDeclarationNode* classDeclarationNode = nullptr;
    InterfaceDeclarationNode* interfaceDeclarationNode = nullptr;
    vector<ASTNode*> children;
    for (Token child: node.m_generated_tokens) {
        if (child.m_type == TokenType::PackageDeclaration) {
            cout<<"Package Declaration"<<endl;
            name = NameCreate(child.m_generated_tokens[1]);
            children.push_back(name);
        }
        else if (child.m_type == TokenType::ImportDeclarations) {
            cout<<"Import Declarations"<<endl;
            importDeclarations = ImportDeclarationsCreate(child);
            for (int i=0; i<importDeclarations.size(); i++) {
                children.push_back(importDeclarations[i]);
            }
        }
        else if (child.m_type == TokenType::TypeDeclarations) {
            if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::ClassDeclaration) {
                cout<<"\nClass Declaration"<<endl;
                classDeclarationNode = ClassDeclarationCreate(child.m_generated_tokens[0].m_generated_tokens[0]);
                children.push_back(classDeclarationNode);
            }
            else if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::InterfaceDeclaration) {
                cout<<"\nInterface Declarations"<<endl;
                interfaceDeclarationNode = InterfaceDeclarationCreate(child.m_generated_tokens[0].m_generated_tokens[0]);
                children.push_back(interfaceDeclarationNode);
            }
        }
    }

    return new CompilationUnitNode(children);
}

void printChildrenOfNodes(ASTNode* astNode) {
    cout<<"Node: ";
    cout<<astNode->type();
    cout<<"\nChildren Size: "<<astNode->children.size()<<endl;
    vector<ASTNode*> children = astNode->children;

    for (int i=0; i<children.size(); i++) {
        cout<<children[i]->type()<<" ";
    }


}

vector<ImportDeclarationNode*> ImportDeclarationsCreate(Token node) {
    assert(node.m_type == ImportDeclarations);
    Token child = node.m_generated_tokens[0];
    vector<ImportDeclarationNode*> importDeclarations;
    while (child.m_generated_tokens.size() > 1) {
        importDeclarations.push_back(ImportDeclarationCreate(child.m_generated_tokens[1]));
        child = child.m_generated_tokens[0];
    }

    importDeclarations.push_back(ImportDeclarationCreate(child.m_generated_tokens[0]));
    reverse(importDeclarations.begin(), importDeclarations.end());

    return importDeclarations;
}

ImportDeclarationNode* ImportDeclarationCreate(Token node) {
    assert(node.m_type == ImportDeclaration);
    if (node.m_generated_tokens[0].m_type == TokenType::SingleTypeImportDeclaration) {
        vector<ASTNode*> children = {NameCreate(node.m_generated_tokens[0].m_generated_tokens[1])};
        return new ImportDeclarationNode(0, children);
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::TypeImportOnDemandDeclaration) {
        vector<ASTNode*> children = {NameCreate(node.m_generated_tokens[0].m_generated_tokens[1])};
        return new ImportDeclarationNode(1, children);
    }
}

ClassDeclarationNode* ClassDeclarationCreate(Token node) {
    assert(node.m_type == ClassDeclaration);
    vector<ModifierNode*> modifiers;
    string identifier;
    NameNode* name = nullptr;
    vector<InterfaceNode*> interfaces;
    vector<FieldDeclarationNode*> fieldDeclarations;
    vector<MethodDeclarationNode*> methodDeclarations;
    vector<ClassBodyDeclarationNode*> classBodyDeclarations; 
    vector<ASTNode*> children;
    for (Token child: node.m_generated_tokens) {
        if (child.m_type == TokenType::Modifiers) {
            cout<<"\nModifiers class"<<endl;
            modifiers = ModifiersCreate(child);
            for (int i=0; i<modifiers.size(); i++) {
                children.push_back(modifiers[i]);
            }
        }
        else if (child.m_type == TokenType::T_IDENTIFIER) {
            cout<<"\nClassName"<<endl;
            identifier = child.m_lex;
        }
        else if (child.m_type == TokenType::Super) {
            cout<<"\nSuper class"<<endl;
            name = ClassOrInterfaceTypeCreate(child.m_generated_tokens[1].m_generated_tokens[0]);
            children.push_back(name);
        }
        else if (child.m_type == TokenType::Interfaces) {
            cout<<"\nInterfaces implemented"<<endl;
            interfaces = InterfacesCreate(child);
            for (int i=0; i<interfaces.size(); i++) {
                children.push_back(interfaces[i]);
            }
        }
        else if (child.m_type == TokenType::ClassBody) {
            cout<<"\nClassBody"<<endl;
            if (child.m_generated_tokens[1].m_type == TokenType::ClassBodyDeclarations) {
                cout<<"\nClassBodyDeclarations"<<endl;
                classBodyDeclarations = ClassBodyDeclarationsCreate(child.m_generated_tokens[1]);
                for (int i=0; i<classBodyDeclarations.size(); i++) {
                    children.push_back(classBodyDeclarations[i]); 
                }
            }
        }
    }

    return new ClassDeclarationNode(identifier, children);
}

InterfaceDeclarationNode* InterfaceDeclarationCreate(Token node) {
    assert(node.m_type == InterfaceDeclaration);
    vector<ModifierNode*> modifiers;
    string identifier;
    NameNode* name = nullptr;
    vector<InterfaceNode*> interfaces;
    vector<MethodDeclarationNode*> methodHeaders;
    vector<ASTNode*> children;
    for (Token child: node.m_generated_tokens) {
        if (child.m_type == TokenType::Modifiers) {
            cout<<"\nInterface Modifiers"<<endl;
            modifiers = ModifiersCreate(child);
            for (int i=0; i<modifiers.size(); i++) {
                children.push_back(modifiers[i]);
            }
        }
        else if (child.m_type == TokenType::T_IDENTIFIER) {
            cout<<"\nInterface name"<<endl;
            identifier = child.m_lex;
        }
        else if (child.m_type == TokenType::ExtendsInterfaces) {
            cout<<"\nInterfaces extended"<<endl;
            interfaces = ExtendsInterfacesCreate(child);
            for (int i=0; i<interfaces.size(); i++) {
                children.push_back(interfaces[i]);
            }
        }
        else if (child.m_type == TokenType::InterfaceBody) {
            cout<<"\nInterface body"<<endl;
            if (child.m_generated_tokens[1].m_type == TokenType::InterfaceMemberDeclarations) {
                cout<<"\nInterfaceMemberDeclarations"<<endl;
                methodHeaders = InterfaceMemberDeclarationsCreate(child.m_generated_tokens[1]);
                for (int i=0; i<methodHeaders.size(); i++) {
                    children.push_back(methodHeaders[i]);
                }
            }
        }
    }

    return new InterfaceDeclarationNode(identifier, children);

}

vector<MethodDeclarationNode*> InterfaceMemberDeclarationsCreate(Token node) {
    assert(node.m_type == InterfaceMemberDeclarations);
    vector<MethodDeclarationNode*> methodHeaders;
    Token child = node.m_generated_tokens[0];
    while (child.m_generated_tokens.size() > 1) {
        methodHeaders.push_back(MethodDeclarationCreate(child.m_generated_tokens[1].m_generated_tokens[0].m_generated_tokens[0]));
        child = child.m_generated_tokens[0];
    }

    methodHeaders.push_back(MethodDeclarationCreate(child.m_generated_tokens[0].m_generated_tokens[0].m_generated_tokens[0]));
    reverse(methodHeaders.begin(), methodHeaders.end());

    return methodHeaders;
}

vector<ClassBodyDeclarationNode*> ClassBodyDeclarationsCreate(Token node) {
    assert(node.m_type == ClassBodyDeclarations); 
    Token child = node.m_generated_tokens[0];
    vector<ClassBodyDeclarationNode*> classBodyDeclarations;
    while (child.m_generated_tokens.size() > 1) {
        if (child.m_generated_tokens[1].m_generated_tokens[0].m_type == TokenType::ClassMemberDeclaration) {
            if (child.m_generated_tokens[1].m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::FieldDeclaration) {
                classBodyDeclarations.push_back(FieldDeclarationCreate(child.m_generated_tokens[1].m_generated_tokens[0].m_generated_tokens[0]));
            }
            else if (child.m_generated_tokens[1].m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::MethodDeclaration) {
                classBodyDeclarations.push_back(MethodDeclarationCreate(child.m_generated_tokens[1].m_generated_tokens[0].m_generated_tokens[0]));
            }
        }
        else if (child.m_generated_tokens[1].m_generated_tokens[0].m_type == TokenType::ConstructorDeclaration) {
            classBodyDeclarations.push_back(ConstructorDeclarationCreate(child.m_generated_tokens[1].m_generated_tokens[0]));
        }
        child = child.m_generated_tokens[0];
    }

    if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::ClassMemberDeclaration) {
        if (child.m_generated_tokens[0].m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::FieldDeclaration) {
            classBodyDeclarations.push_back(FieldDeclarationCreate(child.m_generated_tokens[0].m_generated_tokens[0].m_generated_tokens[0]));
        }
        else if (child.m_generated_tokens[0].m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::MethodDeclaration) {
            classBodyDeclarations.push_back(MethodDeclarationCreate(child.m_generated_tokens[0].m_generated_tokens[0].m_generated_tokens[0]));
        }
    }
    else if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::ConstructorDeclaration) {
        classBodyDeclarations.push_back(ConstructorDeclarationCreate(child.m_generated_tokens[0].m_generated_tokens[0]));
    }

    reverse(classBodyDeclarations.begin(), classBodyDeclarations.end());

    return classBodyDeclarations;
}

ConstructorDeclarationNode* ConstructorDeclarationCreate(Token node) {
    assert(node.m_type == ConstructorDeclaration); 
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

ConstructorDeclaratorNode* ConstructorDeclaratorCreate(Token node) {
    assert(node.m_type == ConstructorDeclarator);
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

vector<FormalParameterNode*> FormalParameterListCreate(Token node) {
    assert(node.m_type == FormalParameterList);
    Token child = node.m_generated_tokens[0];
    vector<FormalParameterNode*> formalParameters;
    while (child.m_generated_tokens.size() > 1) {
        formalParameters.push_back(FormalParameterCreate(child.m_generated_tokens[1]));
        child = child.m_generated_tokens[0];
    }

    formalParameters.push_back(FormalParameterCreate(child.m_generated_tokens[0]));
    reverse(formalParameters.begin(), formalParameters.end());

    return formalParameters;
}

FormalParameterNode* FormalParameterCreate(Token node) {
    assert(node.m_type == FormalParameter);
    TypeNode* type = TypeCreate(node.m_generated_tokens[0]);
    string identifier = node.m_generated_tokens[1].m_generated_tokens[0].m_lex;
    vector<ASTNode*> children = {type};

    return new FormalParameterNode(identifier, children);
}

vector<BlockStatementNode*> BlockStatementsCreate(Token node) {
    assert(node.m_type == BlockStatements);
    Token child = node.m_generated_tokens[0];
    vector<BlockStatementNode*> blockStatements;
    while (child.m_generated_tokens.size() > 1) {
        if (child.m_generated_tokens[1].m_generated_tokens[0].m_type == TokenType::LocalVariableDeclarationStatement) {
            blockStatements.push_back(LocalVariableDeclarationCreate(child.m_generated_tokens[1].m_generated_tokens[0].m_generated_tokens[0]));
        }
        else if (child.m_generated_tokens[1].m_generated_tokens[0].m_type == TokenType::Statement) {
            blockStatements.push_back(StatementCreate(child.m_generated_tokens[1].m_generated_tokens[0]));
        }
        child = child.m_generated_tokens[0];
    }

    if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::LocalVariableDeclarationStatement) {
        blockStatements.push_back(LocalVariableDeclarationCreate(child.m_generated_tokens[0].m_generated_tokens[0].m_generated_tokens[0]));
    }
    else if (child.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::Statement) {
        blockStatements.push_back(StatementCreate(child.m_generated_tokens[0].m_generated_tokens[0]));
    }

    reverse(blockStatements.begin(), blockStatements.end());

    return blockStatements;

}

LocalVariableDeclarationNode* LocalVariableDeclarationCreate(Token node) {
    assert(node.m_type == LocalVariableDeclaration);
    TypeNode* type = TypeCreate(node.m_generated_tokens[0]);
    VariableDeclaratorNode* variableDeclarator = VariableDeclaratorCreate(node.m_generated_tokens[1]);
    vector<ASTNode*> children = {type, variableDeclarator};

    return new LocalVariableDeclarationNode(children);
}

VariableDeclaratorNode* VariableDeclaratorCreate(Token node) {
    assert(node.m_type == VariableDeclarator);
    string identifier = node.m_generated_tokens[0].m_generated_tokens[0].m_lex;
    ExpressionNode* expression = nullptr;
    vector<ASTNode*> children;
    if (node.m_generated_tokens.size() > 1) {
        expression = ExpressionCreate(node.m_generated_tokens[2].m_generated_tokens[0]);
        children.push_back(expression);
    }

    return new VariableDeclaratorNode(identifier, children);
}

StatementNode* StatementCreate(Token node) {
    assert(node.m_type == Statement);
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

StatementNoShortIfNode* StatementNoShortIfCreate(Token node) {
    assert(node.m_type == StatementNoShortIf);
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

StatementWithoutTrailingSubstatementNode* StatementWithoutTrailingSubstatementCreate(Token node) {
    assert(node.m_type == StatementWithoutTrailingSubstatement);
    StatementWithoutTrailingSubstatementNode* statementWithoutTrailingSubstatementNode = nullptr;
    if (node.m_generated_tokens[0].m_type == TokenType::Block) {
        if (node.m_generated_tokens[0].m_generated_tokens[1].m_type == TokenType::BlockStatements) {
            vector<ASTNode*> children;
            vector<BlockStatementNode*> blockStatements = BlockStatementsCreate(node.m_generated_tokens[0].m_generated_tokens[1]);
            for (int i=0; i<blockStatements.size(); i++) {
                children.push_back(blockStatements[i]);
            }
            statementWithoutTrailingSubstatementNode = new BlockNode(children);
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

ReturnStatementNode* ReturnStatementCreate(Token node) {
    assert(node.m_type == ReturnStatement);
    ExpressionNode* expression = nullptr;
    vector<ASTNode*> children;
    if (node.m_generated_tokens[1].m_type == TokenType::Expression) {
        expression = ExpressionCreate(node.m_generated_tokens[1]);
        children.push_back(expression);
    }

    return new ReturnStatementNode(children);
}

StatementExpressionNode* StatementExpressionCreate(Token node) {
    assert(node.m_type == StatementExpression);
    StatementExpressionNode* statementExpressionNode = nullptr;
    if (node.m_generated_tokens[0].m_type == TokenType::Assignment) {
        statementExpressionNode = AssignmentCreate(node.m_generated_tokens[0]);
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::MethodInvocation) {
        //statementExpressionNode = MethodInvocation(node.m_generated_tokens[0]);

    }
    else if (node.m_generated_tokens[0].m_type == TokenType::ClassInstanceCreationExpression) {
        statementExpressionNode = ClassInstanceCreationExpressionCreate(node.m_generated_tokens[0]);
    }

    return statementExpressionNode;
}

IfThenStatementNode* IfThenStatementCreate(Token node) {
    assert(node.m_type == IfThenStatement);
    ExpressionNode* expression = ExpressionCreate(node.m_generated_tokens[2]);
    StatementNode* statement = StatementCreate(node.m_generated_tokens[4]);

    vector<ASTNode*> children = {expression, statement};

    return new IfThenStatementNode(children);
}

IfThenElseStatementNode* IfThenElseStatementCreate(Token node) {
    assert(node.m_type == IfThenElseStatement);
    ExpressionNode* expression = ExpressionCreate(node.m_generated_tokens[2]);
    StatementNoShortIfNode* statementNoShortIf = StatementNoShortIfCreate(node.m_generated_tokens[4]);
    StatementNode* statement = StatementCreate(node.m_generated_tokens[6]);

    vector<ASTNode*> children = {expression, statementNoShortIf, statement};
    
    return new IfThenElseStatementNode(children);
}

IfThenElseStatementNoShortIfNode* IfThenElseStatementNoShortIfCreate(Token node) {
    assert(node.m_type == IfThenElseStatementNoShortIf);
    ExpressionNode* expression = ExpressionCreate(node.m_generated_tokens[2]);
    StatementNoShortIfNode* statementNoShortIf1 = StatementNoShortIfCreate(node.m_generated_tokens[4]);
    StatementNoShortIfNode* statementNoShortIf2 = StatementNoShortIfCreate(node.m_generated_tokens[6]);

    vector<ASTNode*> children = {expression, statementNoShortIf1, statementNoShortIf2};

    return new IfThenElseStatementNoShortIfNode(children);
}

WhileStatementNode* WhileStatementCreate(Token node) {
    assert(node.m_type == WhileStatement);
    ExpressionNode* expression = ExpressionCreate(node.m_generated_tokens[2]);
    StatementNode* statement = StatementCreate(node.m_generated_tokens[4]);

    vector<ASTNode*> children = {expression, statement};

    return new WhileStatementNode(children);

}

WhileStatementNoShortIfNode* WhileStatementNoShortIfCreate(Token node) {
    assert(node.m_type == WhileStatementNoShortIf);
    ExpressionNode* expression = ExpressionCreate(node.m_generated_tokens[2]);
    StatementNoShortIfNode* statementNoShortIf = StatementNoShortIfCreate(node.m_generated_tokens[4]);

    vector<ASTNode*> children = {expression, statementNoShortIf};

    return new WhileStatementNoShortIfNode(children);
}

ForStatementNode* ForStatementCreate(Token node) {
    assert(node.m_type == ForStatement);
    LocalVariableDeclarationNode* localVariableDeclaration = nullptr;
    StatementExpressionNode* statementExpressionForInit = nullptr;
    StatementExpressionNode* statementExpressionForUpdate = nullptr;
    ExpressionNode* expression = nullptr;
    StatementNode* statement = nullptr;

    vector<ASTNode*> children;

    for (Token child: node.m_generated_tokens) {
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

ForStatementNoShortIfNode* ForStatementNoShortIfCreate(Token node) {
    assert(node.m_type == ForStatementNoShortIf);
    LocalVariableDeclarationNode* localVariableDeclaration = nullptr;
    StatementExpressionNode* statementExpressionForInit = nullptr;
    StatementExpressionNode* statementExpressionForUpdate = nullptr;
    ExpressionNode* expression = nullptr;
    StatementNoShortIfNode* statementNoShortIf = nullptr;
   
    vector<ASTNode*> children;

    for (Token child: node.m_generated_tokens) {
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

vector<InterfaceNode*> ExtendsInterfacesCreate(Token node) {
    assert(node.m_type == ExtendsInterfaces);
    Token child = node.m_generated_tokens[1];
    vector<InterfaceNode*> interfaces;
    while (child.m_generated_tokens.size() > 2) {
        vector<ASTNode*> children;
        children.push_back(ClassOrInterfaceTypeCreate(child.m_generated_tokens[2].m_generated_tokens[0]));
        interfaces.push_back(new InterfaceNode(children));
        child = child.m_generated_tokens[0];
    }

    vector<ASTNode*> children;
    children.push_back(ClassOrInterfaceTypeCreate(child.m_generated_tokens[1].m_generated_tokens[0]));
    interfaces.push_back(new InterfaceNode(children));

    reverse(interfaces.begin(), interfaces.end());

    return interfaces;
}

vector<InterfaceNode*> InterfacesCreate(Token node) {
    assert(node.m_type == Interfaces);
    Token child = node.m_generated_tokens[0];
    vector<InterfaceNode*> interfaces;
    while (child.m_generated_tokens.size() > 1) {
        vector<ASTNode*> children;
        children.push_back(ClassOrInterfaceTypeCreate(child.m_generated_tokens[2].m_generated_tokens[0]));
        interfaces.push_back(new InterfaceNode(children));
        child = child.m_generated_tokens[0];
    }

    vector<ASTNode*> children;
    children.push_back(ClassOrInterfaceTypeCreate(child.m_generated_tokens[0].m_generated_tokens[0]));
    interfaces.push_back(new InterfaceNode(children));
    reverse(interfaces.begin(), interfaces.end());

    return interfaces;
}

vector<ModifierNode*> ModifiersCreate(Token node) {
    assert(node.m_type == Modifiers);
    Token child = node.m_generated_tokens[0];
    vector<ModifierNode*> modifiers;
    while (child.m_generated_tokens.size() > 1) {
        modifiers.push_back(new ModifierNode(child.m_generated_tokens[1].m_lex));
        child = child.m_generated_tokens[0];
    }

    modifiers.push_back(new ModifierNode(child.m_generated_tokens[0].m_lex));
    reverse(modifiers.begin(), modifiers.end());

    return modifiers;
}

FieldDeclarationNode* FieldDeclarationCreate(Token node) {
    assert(node.m_type == FieldDeclaration);
    vector<ModifierNode*> modifiers = ModifiersCreate(node.m_generated_tokens[0]);
    TypeNode* type = TypeCreate(node.m_generated_tokens[1]);
    VariableDeclaratorNode* variableDeclarator = VariableDeclaratorCreate(node.m_generated_tokens[2].m_generated_tokens[0]);

    vector<ASTNode*> children;

    for (int i=0; i<modifiers.size(); i++) {
        children.push_back(modifiers[i]);
    }

    children.push_back(type);
    children.push_back(variableDeclarator);

    return new FieldDeclarationNode(children);
}

MethodDeclarationNode* MethodDeclarationCreate(Token node) {
    assert(node.m_type == MethodDeclaration || node.m_type == MethodHeader);
    
    MethodHeaderNode* methodHeader = nullptr;
    if (node.m_type == TokenType::MethodHeader) {
        methodHeader = MethodHeaderCreate(node);
    }
    else {
        methodHeader = MethodHeaderCreate(node.m_generated_tokens[0]);
    }
    BlockNode* block = nullptr;

    vector<BlockStatementNode*> blockStatements;

    vector<ASTNode*> children;
    children.push_back(methodHeader);

    if (node.m_generated_tokens.size() == 1) {
        return new MethodDeclarationNode(children);
    }

    vector<ASTNode*> childrenOfBlock;

    if (node.m_generated_tokens[1].m_generated_tokens[0].m_type == TokenType::Block) {
        if (node.m_generated_tokens[1].m_generated_tokens[0].m_generated_tokens[1].m_type == TokenType::BlockStatements) {
            blockStatements = BlockStatementsCreate(node.m_generated_tokens[1].m_generated_tokens[0]);
            for (int i=0; i<blockStatements.size(); i++) {
                childrenOfBlock.push_back(blockStatements[i]);
            }
            block = new BlockNode(childrenOfBlock);
            children.push_back(block);        
        }
    }
     
    return new MethodDeclarationNode(children);
}

MethodHeaderNode* MethodHeaderCreate(Token node) {
    assert(node.m_type == MethodHeader);
    vector<ModifierNode*> modifiers = ModifiersCreate(node.m_generated_tokens[0]);
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

MethodDeclaratorNode* MethodDeclaratorCreate(Token node) {
    assert(node.m_type == MethodDeclarator);
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

PrimaryNoNewArrayNode* PrimaryNoNewArrayCreate(Token node) {
    assert(node.m_type == PrimaryNoNewArray);
    vector<ASTNode*> children;

    return new PrimaryNoNewArrayNode(children);
}

ArrayCreationExpressionNode* ArrayCreationExpressionCreate(Token node) {
    assert(node.m_type == ArrayCreationExpression);
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

vector<ExpressionNode*> ArgumentListCreate(Token node) {
    assert(node.m_type == ArgumentList);
    Token child = node.m_generated_tokens[0];
    vector<ExpressionNode*> expressions;
    while (child.m_generated_tokens.size() > 1) {
        expressions.push_back(ExpressionCreate(child.m_generated_tokens[2]));
        child = child.m_generated_tokens[0];
    }

    expressions.push_back(ExpressionCreate(child.m_generated_tokens[0]));
    reverse(expressions.begin(), expressions.end());

    return expressions;
}

ClassInstanceCreationExpressionNode* ClassInstanceCreationExpressionCreate(Token node) {
    assert(node.m_type == ClassInstanceCreationExpression);
    ClassOrInterfaceTypeNode* classOrInterfaceType = ClassOrInterfaceTypeCreate(node.m_generated_tokens[1]);
    vector<ExpressionNode*> argumentList;
    vector<ASTNode*> children;
    children.push_back(classOrInterfaceType);
    if (node.m_generated_tokens[3].m_type == TokenType::ArgumentList) {
        argumentList = ArgumentListCreate(node.m_generated_tokens[3]);
        for (int i=0; i<argumentList.size(); i++) {
            children.push_back(argumentList[i]);
        }
    }

    return new ClassInstanceCreationExpressionNode(children);
}

ExpressionNode* ExpressionCreate(Token node) {
    assert(node.m_type == Expression);
    return AssignmentExpressionCreate(node.m_generated_tokens[0]);
}

AssignmentExpressionNode* AssignmentExpressionCreate(Token node) {
    assert(node.m_type == Assignment);
    if (node.m_generated_tokens[0].m_type == TokenType::Assignment) {
        return AssignmentCreate(node.m_generated_tokens[0]);
    }
    else {
        return ConditionalOrExpressionCreate(node.m_generated_tokens[0].m_generated_tokens[0]);
    }  
}

AssignmentNode* AssignmentCreate(Token node) {
    assert(node.m_type == Assignment);
    LeftHandSideNode* leftHandSide = LeftHandSideCreate(node.m_generated_tokens[0]);
    AssignmentExpressionNode* assignmentExpression = AssignmentExpressionCreate(node.m_generated_tokens[2]);

    vector<ASTNode*> children = {leftHandSide, assignmentExpression};

    return new AssignmentNode(children);
}

LeftHandSideNode* LeftHandSideCreate(Token node) {
    assert(node.m_type == LeftHandSide);
    LeftHandSideNode* leftHandSideNode = nullptr;
    if (node.m_generated_tokens[0].m_type == TokenType::Name) {
        leftHandSideNode = NameCreate(node.m_generated_tokens[0]);
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::FieldAccess) {
        leftHandSideNode = FieldAccessCreate(node.m_generated_tokens[0]);
    }
    else if (node.m_generated_tokens[0].m_type == TokenType::ArrayAccess) {
        leftHandSideNode = ArrayAccessCreate(node.m_generated_tokens[0]);
    }
    
    return leftHandSideNode;
}

ArrayAccessNode* ArrayAccessCreate(Token node) {
    assert(node.m_type == ArrayAccess);
    vector<ASTNode*> children;
    if (node.m_generated_tokens[0].m_generated_tokens[0].m_type == TokenType::ArrayCreationExpression) {
        children.push_back(ArrayCreationExpressionCreate(node.m_generated_tokens[0].m_generated_tokens[0]));
    }
    else {
        children.push_back(PrimaryNoNewArrayCreate(node.m_generated_tokens[0].m_generated_tokens[0]));
    }

    return new ArrayAccessNode(children);
}

ConditionalOrExpressionNode* ConditionalOrExpressionCreate(Token node) {
    assert(node.m_type == ConditionalOrExpression);
    vector<ASTNode*> conditionalAndExpressions;
    Token child = node.m_generated_tokens[0];
    vector<ASTNode*> children;
    while (child.m_generated_tokens.size() > 1) {
        conditionalAndExpressions.push_back(ConditionalAndExpressionCreate(child.m_generated_tokens[2]));
        child = child.m_generated_tokens[0];
    }

    conditionalAndExpressions.push_back(ConditionalAndExpressionCreate(child.m_generated_tokens[0]));
    reverse(conditionalAndExpressions.begin(), conditionalAndExpressions.end());

    return new ConditionalOrExpressionNode(conditionalAndExpressions);
}

ConditionalAndExpressionNode* ConditionalAndExpressionCreate(Token node) {
    assert(node.m_type == ConditionalAndExpression);
    vector<ASTNode*> inclusiveOrExpressions;
    Token child = node.m_generated_tokens[0];
    while (child.m_generated_tokens.size() > 1) {
        inclusiveOrExpressions.push_back(InclusiveOrExpressionCreate(child.m_generated_tokens[2]));
        child = child.m_generated_tokens[0];
    }

    inclusiveOrExpressions.push_back(InclusiveOrExpressionCreate(child.m_generated_tokens[0]));
    reverse(inclusiveOrExpressions.begin(), inclusiveOrExpressions.end());

    return new ConditionalAndExpressionNode(inclusiveOrExpressions);
}

InclusiveOrExpressionNode* InclusiveOrExpressionCreate(Token node) {
    assert(node.m_type == InclusiveOrExpression);
    vector<ASTNode*> exclusiveOrExpressions;
    Token child = node.m_generated_tokens[0];
    while (child.m_generated_tokens.size() > 1) {
        exclusiveOrExpressions.push_back(ExclusiveOrExpressionCreate(child.m_generated_tokens[2]));
        child = child.m_generated_tokens[0];
    }

    exclusiveOrExpressions.push_back(ExclusiveOrExpressionCreate(child.m_generated_tokens[0]));
    reverse(exclusiveOrExpressions.begin(), exclusiveOrExpressions.end());

    return new InclusiveOrExpressionNode(exclusiveOrExpressions);
}

ExclusiveOrExpressionNode* ExclusiveOrExpressionCreate(Token node) {
    assert(node.m_type == ExclusiveOrExpression);
    return AndExpressionCreate(node.m_generated_tokens[0]);
}

AndExpressionNode* AndExpressionCreate(Token node) {
    assert(node.m_type == AndExpression);

    vector<ASTNode*> equalityExpressions;
    Token child = node.m_generated_tokens[0];
    while (child.m_generated_tokens.size() > 1) {
        equalityExpressions.push_back(EqualityExpressionCreate(child.m_generated_tokens[2]));
        child = child.m_generated_tokens[0];
    }

    equalityExpressions.push_back(EqualityExpressionCreate(child.m_generated_tokens[0]));
    reverse(equalityExpressions.begin(), equalityExpressions.end());

    return new AndExpressionNode(equalityExpressions);
}

EqualityExpressionNode* EqualityExpressionCreate(Token node) {
    assert(node.m_type == EqualityExpression);

    RelationalExpressionNode* relationalExpression = nullptr;
    EqualityExpressionNode* equalityExpression = nullptr;
    int op = -1;

    vector<ASTNode*> children;

    if (node.m_generated_tokens.size() == 1) {
        children.push_back(RelationalExpressionCreate(node.m_generated_tokens[0]));
        return new EqualityExpressionNode(op, children);
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

RelationalExpressionNode* RelationalExpressionCreate(Token node) {
    assert(node.m_type == RelationalExpression);

    RelationalExpressionNode* relationalExpression = nullptr;
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

AdditiveExpressionNode* ShiftExpressionCreate(Token node) {
    assert(node.m_type == ShiftExpression);
    return AdditiveExpressionCreate(node.m_generated_tokens[0]); 
}

AdditiveExpressionNode* AdditiveExpressionCreate(Token node) {
    assert(node.m_type == AdditiveExpression);
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

MultiplicativeExpressionNode* MultiplicativeExpressionCreate(Token node) {
    assert(node.m_type == MultiplicativeExpression);
    UnaryExpressionNode* unaryExpression = nullptr;
    MultiplicativeExpressionNode* multiplicativeExpression = nullptr;
    int op = -1;
    vector<ASTNode*> children;
    return new MultiplicativeExpressionNode(children);
}

FieldAccessNode* FieldAccessCreate(Token node) {
    assert(node.m_type == FieldAccess);
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


