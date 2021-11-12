#include "unicodeutf8.h"
#include "common.h"

uint32_t getByteNumOfEncodeUtf8(int value)
{
    ASSERT(value > 0,"Could not encode value < 0!");
    if (value < 0x7f)
    {
        return 1;
    }
    if (value < 0x7ff)
    {
        return 2;
    }
    if (value < 0xffff)
    {
        return 3;
    }
    if (value < 0x10ffff)
    {
        return 4;
    }
    return 0;
}
//按照utf8编码Unicode字符并写入缓冲区
uint8_t encodeUtf8(uint8_t* buf,int value)
{
    if (value < 0x7f)                           //位于 0111 1111内
    {
        *buf=value & 0x7f;
        return 1;
    }
    else if (value < 0x7ff)                     //位于0111 1111 1111内
    {
        *buf=0xc0 | ((value >> 6) & 0x1f);      //右移6位，再与 1 1111 取与，再与 110 0 0000 取或补前缀
        buf++;
        *buf=0x80 | (value & 0x3f);             //先与 11 1111 取与，再与 10 00 0000 取或补前缀
        return 2;
    }
    else if (value < 0xffff)                    //位于1111 1111 1111 1111内
    {
        *buf=0xe0 | ((value & 0xf000)>>12);     //先与1111 0000 0000 0000取与提取前四位，再右移12位，与1110 0000 取或补前缀
        buf++;
        *buf=0x80 | ((value & 0xfc0)>>6);       //先与0000 1111 1100 0000取与提取，右移6位，再与10 00 0000取或补前缀
        buf++;
        *buf=0x80 | value & 0x3f;               //先与0000 0000 0011 1111取与提取后六位，再与10 00 0000取或补前缀
        return 3;
    }
    else if (value < 0x10ffff)                  //位于0001 0000 1111 1111 1111 1111内
    {
        *buf=0xf0 | ((value & 0x1c0000)>>18);   //先与0001 1100 0000 0000 0000 0000取与提取前三位，再右移18位，与1111 0000 取或补前缀
        buf++;
        *buf=0x80 | ((value & 0x3f000)>>12);    //先与0000 0011 1111 0000 0000 0000取与提取，右移12位，再与10 00 0000取或补前缀
        buf++;
        *buf=0x80 | ((value & 0xfc0) >> 6);     //先与0000 0000 0000 1111 1100 0000取与提取，右移6位，再与10 00 0000取或补前缀
        buf++;
        *buf=0x80 | (value & 0x3f);             //先与0000 0000 0000 0000 0011 1111取与提取后6位，再与10 00 0000取或补前缀
        return 4;
    }
    NOT_REACHED();
    return 0;
}
//返回UTF-8解码出的字节数
uint32_t getByteNumOfDecodeUtf8(uint8_t byte)
{   
    if ((byte & 0b11000000)==0b10000000)
    {
        return 0;                           //如果为后面字节，退出，并返回0
    }
    if ((byte & 0b11100000)==0b11000000)
    {
        return 2;
    }
    if ((byte & 0b11110000)==0b11100000)
    {
        return 3;
    }
    if ((byte & 0b11111000)==0b11110000)
    {
        return 4;
    }
    return 1;
}
