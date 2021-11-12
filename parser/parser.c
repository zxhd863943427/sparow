#include "common.h"
#include "parser.h"
#include "utils.h"
#include "unicodeutf8.h"
#include <string.h>
#include <string.h>
#include <ctype.h>

struct keywordToken
{
    char* keyword;
    uint8_t length;
    TokenType token;
};

struct keywordToken keywordsToken[]=
{
    {"var",     3,  TOKEN_VAR},
    {"fun",     3,  TOKEN_FUN},
    {"if",      2,  TOKEN_IF},
    {"else",    4,  TOKEN_ELSE},
    {"true",    4,  TOKEN_TRUE}, 
    {"true",    4,  TOKEN_TRUE}, 
    {"while",   5,  TOKEN_WHILE}, 
    {"for",     3,  TOKEN_FOR}, 
    {"break",   5,  TOKEN_BREAK},
    {"continue", 8, TOKEN_CONTINUE}, 
    {"return",  6,  TOKEN_RETURN},
    {"null",    4,  TOKEN_NULL},
    {"class",   5,  TOKEN_CLASS}, 
    {"is",      2,  TOKEN_IS}, 
    {"static",  6,  TOKEN_STATIC},
    {"this",    4,  TOKEN_THIS}, 
    {"super",   5,  TOKEN_SUPER},
    {"import",  6,  TOKEN_IMPORT}, 
    {NULL,      0,  TOKEN_UNKNOWN}
};
//对于文本类型，判断是否为关键字，是则返回对应token，否则则返回token_id，即为标志名
static TokenType idOrkeyword(const char* start, uint32_t length)
{
    uint32_t idx = 0;
    while (*(keywordsToken[idx].keyword)!=NULL)
    {
        if (keywordsToken[idx].length == length &&\
            memcmp(keywordsToken[idx].keyword, start, length) == 0)
        {
            return keywordsToken[idx].token;
        }
        idx++;
    }
    return TOKEN_ID;
     
}
//查看下一个字符
char lookAheadChar(Parser* parser)
{
    return *(parser->nextCharPtr);
}
//将下一个字符更新为词法分析器当前字符
static void getNextChar(Parser* parser)
{
    parser->curChar = * parser->nextCharPtr;
    parser->nextCharPtr++;
}
//查看下一字符是否为期望字符，如果是则更新词法分析器当前字符并返回true，否则返回false
static bool matchNextChar(Parser* parser, char expectedChar)
{
    if (lookAheadChar(parser)==expectedChar)
    {
        getNextChar(parser);
        return true;
    }
    else
    {
        return false;
    }
}

static void skipBlanks(Parser* parser)
{
    while (isspace(parser->curChar))
    {
        if (parser->curChar == '\n')
        {
            parser->curToken.lineNo++;
        }
        getNextChar(parser);
    }
}
//解析标志符
static void parseId(Parser* parser, TokenType type)
{
    while (isalnum(parser->curChar) || parser->curChar=='_')
    {
        getNextChar(parser);
    }
    //(parser->nextCharPtr指向第一个不合法字符
    uint32_t length = (uint32_t)((parser->nextCharPtr-1) - parser->curToken.start);
    if (type != TOKEN_UNKNOWN)
    {
        parser->curToken.type=type;
    }
    else
    {
        parser->curToken.type=idOrkeyword(parser->curToken.start,length);
    }
    parser->curToken.length=length;
}
//解码unicode码点
static void parseUnicodeCodePoint(Parser* parser,ByteBuffer* buf)
{
    uint32_t idx = 0;
    int value = 0;
    uint8_t digit = 0;
    while(idx<4)
    {
        idx++;      //跳过第一个字符'u'
        
    }
}