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
//解码unicode码点，已检测到‘\uXXXX’的'u'
static void parseUnicodeCodePoint(Parser* parser,ByteBuffer* buf)
{
    uint32_t idx = 0;
    int value = 0;
    uint8_t digit = 0;
    char temp;                      //缓存从内存中读取的parser->curChar
    for (uint32_t idx=0; idx < 4; idx++)
    {
        getNextChar(parser);        //跳过第一个字符‘u’
        temp = parser->curChar;
        if (temp == '\0')
        {
            LEX_ERROR(parser,"unterminted code!unicode在开始就为0,输入意外终止！");
        }
        if (temp >= '0' && temp <= '9')
        {
            digit = temp -'0';
        }
        else if (temp >= 'a' && temp <= 'f')
        {
            digit = temp -'f' + 10;
        }
        else if (temp >= 'A' && temp <= 'F')
        {
            digit = temp - 'F' + 10;
        }
        else 
        {
            LEX_ERROR(parser,"invalid unicode! 输入的unicode码不在编码范围内！");
        }
        value = (value<<4) | digit;         //先右移四位，再填充
    }
    uint32_t byteNum = getByteNumOfEncodeUtf8(value);
    ASSERT(byteNum != 0, "utf8 encode byte must between 1 - 4!");
    ByteBufferFillWrite(parser->vm, buf, 0, byteNum);               //先对目标地址填0,保证存在足够空间
    encodeUtf8(buf->datas + buf->count - byteNum, value);           //buf->datas指向bytebuffer缓冲区的首位，加上buf->conut得到了储存目前位置，，在减去之前提前申请的部分，就得到了写入的地址
}
//解析字符串,开始于目前的parser->curChar='\"'
static void parseString(Parser* parser)
{
    ByteBuffer str;
    ByteBufferInit(&str);
    while (true)
    {
        getNextChar(parser);
        if (parser->curChar == '\0')
        {
            LEX_ERROR(parser, "unterminated String! 未遇到另一 \" 就终止，输入不完整！");
        }
        if (parser->curChar == '"')
        {
            parser->curToken.type = TOKEN_STRING;
            break;
        }
        if (parser->curChar == '%')
        {
            if (!matchNextChar(parser,'('))
            {
                LEX_ERROR(parser, " \'%%\' should by follow \'(\'! ");
            }
            if (parser->interpolationExpectRightParenNum>0)
            {
                COMPILE_ERROR(parser,"sorry, now didn`t support more interplate!");
            }
            parser ->interpolationExpectRightParenNum =1;
            parser ->curToken.type = TOKEN_INTERPOLATION;
            break;
        }

        if (parser->curChar=='\\')
        {
            getNextChar(parser);
            switch (parser->curChar)
            {
            case '0':
                ByteBufferAdd(parser->vm,&str, '\0');
                break;
            case 'a':
                ByteBufferAdd(parser->vm,&str, '\a');
                break;
            case 'b':
                ByteBufferAdd(parser->vm,&str, '\b');
                break;
            case 'f':
                ByteBufferAdd(parser->vm,&str, '\f');
                break;
            case 'n':
                ByteBufferAdd(parser->vm,&str, '\n');
                break;
            case 'r':
                ByteBufferAdd(parser->vm,&str, '\r');
                break;
            case 't':
                ByteBufferAdd(parser->vm,&str, '\t');
                break;
            case 'u':
                parseUnicodeCodePoint(parser, &str);
                break;
            case '"':
                ByteBufferAdd(parser->vm,&str, '"');
                break;
            case '\\':
                ByteBufferAdd(parser->vm,&str, '\\');
                break;
            default:
                LEX_ERROR(parser, "unsupport escape \\%c", parser->curChar);
                break;
            }
        }
        else
        {
            ByteBufferAdd(parser->vm, &str, parser->curChar);
        }
    }
    ByteBufferClear(parser->vm, &str); 
    
}