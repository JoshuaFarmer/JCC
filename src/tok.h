#ifndef TOK_H
#define TOK_H

#include "types.h"
int counter = 0;

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
                while ((isalnum(*src) || *src == '_') && i < 31) {
                        id[i++] = *src++;
                } id[i] = '\0';
                for (int j = 0; j < sizeof(keys)/sizeof(struct KEYWORD); ++j)
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
        else if (*src == '>' && *(src+1) == '=')
        {
                src += 2;
                tok = TOK_GEQ;
                return;
        }
        else if (*src == '<' && *(src+1) == '=')
        {
                src += 2;
                tok = TOK_LEQ;
                return;
        }
        else if (*src == '&' && *(src+1) == '&')
        {
                src += 2;
                tok = TOK_CHAIN;
                return;
        }
        else if (*src == '|' && *(src+1) == '|')
        {
                src += 2;
                tok = TOK_CHAIN_OR;
                return;
        }

        else if (*src == '#')
        {
                ++src;
                if (strncmp(src,"include",7) == 0)
                {
                        src += 7;
                        while (*src && *src != '<') ++src;
                        if (!*src) return;
                        ++src;
                        int x = 0;
                        while (*src != '>' && *src)
                        {
                                id[x++] = *(src++);
                        }
                        if (!*src) return;
                        FILE * f = fo;
                        FILE * fs = sf;
                        char *oldpath1=mainPath;
                        char *oldpath2=outPath;
                        char *parent1 =dirname(strdup(oldpath1));
                        char *parent2 =dirname(strdup(oldpath2));
                        char *oldsrc=src;


                        char path[1024];
                        char Id[512];
                        snprintf(Id, sizeof(Id), "%s/%s", parent1, id);
                        snprintf(path, sizeof(path), "%s/inc.%d.s", parent2, counter);
                        emit("\t%%include \"%s/inc.%d.s\"\n", parent2, counter++);
                        free(parent1);
                        free(parent2);
                        is_included = true;
                        use_eax = 1;
                        compiler(Id,path);
                        is_included = false;
                        mainPath=oldpath1;
                        outPath=oldpath2;
                        fo=f;
                        sf=fs;
                        src=oldsrc;
                        tok = ';';
                        ++src;
                        next();
                        return;
                }
        }

        tok = *src++;
}

void skip_til(char x)
{
        while (tok != x && *src && tok) next();
}

#endif