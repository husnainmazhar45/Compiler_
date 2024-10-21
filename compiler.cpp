#include <iostream>
#include <vector>
#include <map>
#include <cctype>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

enum TokenType
{
    T_INT,
    T_FLOAT,
    T_ID,
    T_NUM,
    T_FLOAT_NUM,
    T_IF,
    T_ELSE,
    T_RETURN,
    T_ASSIGN,
    T_PLUS,
    T_MINUS,
    T_NUL,
    T_MUL,
    T_DIV,
    T_LPAREN,
    T_RPAREN,
    T_LBRACE,
    T_RBRACE,
    T_SEMICOLON,
    T_GT,
    T_EOF
};

struct Token
{
    TokenType type;
    string value;
};

class Parser
{
private:
    vector<Token> tokens;
    size_t pos;

public:
    Parser(const vector<Token> &tokens)
    {
        this->tokens = tokens;
        this->pos = 0;
    }

    void parseProgram()
    {
        while (tokens[pos].type != T_EOF)
        {
            parseStatement();
        }
        cout << "Parsing completed successfully! No Syntax Error" << endl;
    }

    void parseStatement()
    {
        if (tokens[pos].type == T_INT)
        {
            parseDeclaration();
        }
        else if (tokens[pos].type == T_FLOAT)
        {
            parseDeclaration();
        }
        else if (tokens[pos].type == T_ID)
        {
            parseAssignment();
        }
        else if (tokens[pos].type == T_IF)
        {
            parseIfStatement();
        }
        else if (tokens[pos].type == T_RETURN)
        {
            parseReturnStatement();
        }
        else if (tokens[pos].type == T_LBRACE)
        {
            parseBlock();
        }
        else
        {
            cout << "Syntax error: unexpected token " << tokens[pos].value << " on line " << pos + 1 << endl;
            exit(1);
        }
    }

    void parseBlock()
    {
        expect(T_LBRACE);
        while (tokens[pos].type != T_RBRACE && tokens[pos].type != T_EOF)
        {
            parseStatement();
        }
        expect(T_RBRACE);
    }

    void parseDeclaration()
    {
        if (tokens[pos].type == T_INT || tokens[pos].type == T_FLOAT)
        {
            expect(tokens[pos].type); // Expect int or float
            expect(T_ID);
            expect(T_SEMICOLON);
        }
    }

    void parseAssignment()
    {
        expect(T_ID);
        expect(T_ASSIGN);
        parseExpression();
        expect(T_SEMICOLON);
    }

    void parseIfStatement()
    {
        expect(T_IF);
        expect(T_LPAREN);
        parseExpression();
        expect(T_RPAREN);
        parseStatement();
        if (tokens[pos].type == T_ELSE)
        {
            expect(T_ELSE);
            parseStatement();
        }
    }

    void parseReturnStatement()
    {
        expect(T_RETURN);
        parseExpression();
        expect(T_SEMICOLON);
    }

    void parseExpression()
    {
        parseTerm();
        while (tokens[pos].type == T_PLUS || tokens[pos].type == T_MINUS)
        {
            pos++;
            parseTerm();
        }
        if (tokens[pos].type == T_GT)
        {
            pos++;
            parseExpression();
        }
    }

    void parseTerm()
    {
        parseFactor();
        while (tokens[pos].type == T_MUL || tokens[pos].type == T_DIV)
        {
            pos++;
            parseFactor();
        }
    }

    void parseFactor()
    {
        if (tokens[pos].type == T_NUM || tokens[pos].type == T_FLOAT_NUM || tokens[pos].type == T_ID)
        {
            pos++;
        }
        else if (tokens[pos].type == T_LPAREN)
        {
            expect(T_LPAREN);
            parseExpression();
            expect(T_RPAREN);
        }
        else
        {
            cout << "Syntax error: unexpected token " << tokens[pos].value << endl;
            exit(1);
        }
    }

    void expect(TokenType type)
    {
        if (tokens[pos].type == type)
        {
            pos++;
        }
        else
        {
            cout << "Syntax error: expected token " << type << " but found " << tokens[pos].value << " on line " << pos + 1 << endl;
            exit(1);
        }
    }
};

class lexer
{
private:
    string src;
    size_t pos;
    size_t line;

public:
    lexer(const string &src)
    {
        this->src = src;
        this->pos = 0;
        this->line = 1;
    }

    string consumeNumber()
    {
        size_t start = pos;
        while (pos < src.size() && (isdigit(src[pos]) || src[pos] == '.'))
            pos++;
        return src.substr(start, pos - start);
    }

    string consumeWord()
    {
        size_t start = pos;
        while (pos < src.size() && isalnum(src[pos]))
            pos++;
        return src.substr(start, pos - start);
    }

    vector<Token> tokenizer()
    {
        vector<Token> tokens;
        while (pos < src.size())
        {
            char current = src[pos];
            if (isspace(current))
            {
                if (current == '\n') line++; // Increment line number on new line
                pos++;
                continue;
            }
            if (isdigit(current))
            {
                string number = consumeNumber();
                // Check if the number contains a decimal point
                if (number.find('.') != string::npos)
                    tokens.push_back(Token{T_FLOAT_NUM, number}); // Float number
                else
                    tokens.push_back(Token{T_NUM, number}); // Integer number
                continue;
            }
            if (isalpha(current))
            {
                string word = consumeWord();
                if (word == "int")
                    tokens.push_back(Token{T_INT, word});
                else if (word == "float")
                    tokens.push_back(Token{T_FLOAT, word});
                else if (word == "if")
                    tokens.push_back(Token{T_IF, word});
                else if (word == "else")
                    tokens.push_back(Token{T_ELSE, word});
                else if (word == "return")
                    tokens.push_back(Token{T_RETURN, word});
                else
                    tokens.push_back(Token{T_ID, word});
                continue;
            }

            // Start of the switch case for handling special characters
            switch (current)
            {
            case '=':
                tokens.push_back(Token{T_ASSIGN, "="});
                break;
            case '+':
                tokens.push_back(Token{T_PLUS, "+"});
                break;
            case '-':
                tokens.push_back(Token{T_MINUS, "-"});
                break;
            case '*':
                tokens.push_back(Token{T_MUL, "*"});
                break;
            case '/':
                tokens.push_back(Token{T_DIV, "/"});
                break;
            case '(':
                tokens.push_back(Token{T_LPAREN, "("});
                break;
            case ')':
                tokens.push_back(Token{T_RPAREN, ")"});
                break;
            case '{':
                tokens.push_back(Token{T_LBRACE, "{"});
                break;
            case '}':
                tokens.push_back(Token{T_RBRACE, "}"});
                break;
            case ';':
                tokens.push_back(Token{T_SEMICOLON, ";"});
                break;
            case '>':
                tokens.push_back(Token{T_GT, ">"});
                break;
            default:
                cout << "Line " << line << ": Unexpected character: " << current << endl;
                exit(1);
            }
            pos++;
        }
        tokens.push_back(Token{T_EOF, ""});
        return tokens;
    }
};

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cout << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    // Open the file
    ifstream file(argv[1]);
    if (!file)
    {
        cout << "Error: Cannot open file " << argv[1] << endl;
        return 1;
    }

    // Read file content into a string
    stringstream buffer;
    buffer << file.rdbuf();
    string input = buffer.str();

    // Close the file
    file.close();

    // Lexical analysis
    lexer Lexer(input);
    vector<Token> tokens = Lexer.tokenizer();

    // Parsing
    Parser parser(tokens);
    parser.parseProgram();

    return 0;
}
