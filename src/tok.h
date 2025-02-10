#ifndef TOK_H
#define TOK_H

#include "types.h"

void skip_comment()
{
        if (*src == '/' && *(src + 1) == '*')
        {
                src += 2;
                while (*src && !(*src == '*' && *(src + 1) == '/'))
                {
                        src++;
                }
                if (*src)
                {
                        src += 2;
                }
        }
}

void skip_whitespace()
{
        while (*src == ' ' || *src == '\t' || *src == '\n' || *src == '\r') src++;
}

void next()
{
        num=0;
        skip_whitespace();
        skip_comment();
        memset(id,0,sizeof(id));
        if (*src == '\'')
        {
                ++src;
                tok=TOK_NUM;
                num=*src;
                ++src;
                return;
        }
        else if (*src == '"')
        {
                ++src;
                int x = 0;
                while (*src != '"')
                {
                        if (*src == '\\')
                        {
                                ++src;
                                switch (*src)
                                {
                                        case 'n':
                                                id[x]='\n';
                                                break;
                                        case 'r':
                                                id[x]='\r';
                                                break;
                                        case 'a':
                                                id[x]=7;
                                                break;
                                        case 'b':
                                                id[x]=8;
                                                break;
                                        case '0':
                                                id[x]=0;
                                                break;
                                        case 'e':
                                                id[x]=27;
                                                break;
                                }
                                ++x;
                                ++src;
                        }
                        else
                        {
                                id[x++]=*(src++);
                        }
                }

                tok=TOK_STR;
                ++src;
                return;
        }
        else if (isalpha(*src))
        {
                int i = 0;
                while (isalnum(*src) && i < 31) {
                        id[i++] = *src++;
                } id[i] = '\0';
                for (int j = 0; j < 31; ++j)
                {
                        if (strcmp(keys[j].text,id)==0)
                        {
                                tok=keys[j].tok;
                                return;
                        }
                }
                tok = TOK_IDE;
                return;
        }
        else if (*src >= '0' && *src <= '9')
        {
                while (*src >= '0' && *src <= '9')
                {
                        num = num * 10 + (*src-'0');
                        ++src;
                }
                tok=TOK_NUM;
                return;
        }
        else if (*src == '!' && *(src+1) == '=')
        {
                src += 2;
                tok = TOK_NEQ;
                return;
        }
        else if (*src == '=' && *(src+1) == '=')
        {
                src += 2;
                tok = TOK_EQ;
                return;
        }

        tok = *src++;
}

void skip_til(char x)
{
        while (tok != x && *src && tok) next();
}

#endif