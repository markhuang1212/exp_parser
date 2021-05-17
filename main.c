#define _GNU_SOURCE

#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>

#define BUFF_SIZE 16384

char *buff = NULL;
int pos = 0;
int pos_max = 0;

int parseExp(double *);

void skipSpace()
{
    while (isspace(buff[pos]) != 0)
    {
        pos--;
    }
}

int parseNumber(double *value)
{
    int i = 0;
    while (isdigit(buff[pos - i]) != 0 || buff[pos - i] == '.')
    {
        i++;
    }

    if (i == 0)
    {
        return -1;
    }

    pos -= i;
    char num_buff[16];
    strncpy(num_buff, buff + pos + 1, i);

    *value = strtod(num_buff, NULL);

    return 0;
}

/**
 * SSExp = (Exp)
 *       = number
 * 
 */
int parseSSExp(double *result)
{
    int ret;
    double r;
    skipSpace();
    if (buff[pos] == ')')
    {
        pos--;
        skipSpace();
        ret = parseExp(&r);
        if (ret == -1)
        {
            return -1;
        }
        skipSpace();
        if (buff[pos] != '(')
        {
            return -1;
        }
        *result = r;
        pos--;
        return 0;
    }
    else
    {
        ret = parseNumber(&r);
        if (ret == -1)
        {
            return -1;
        }
        else
        {
            *result = r;
            return 0;
        }
    }
}

/**
 * SExp = SExp * SSExp
 *      = SExp / SSExp
 *      = SSExp
 */
int parseSExp(double *result)
{
    int ret;
    int op = 0;

    skipSpace();
    double r1;
    double r2;

    ret = parseSSExp(&r1);
    if (ret != 0)
    {
        return -1;
    }

    skipSpace();

    if (buff[pos] == '*')
    {
        op = 1;
    }
    else if (buff[pos] == '/')
    {
        op = -1;
    }
    else
    {
        *result = r1;
        return 0;
    }

    pos--;

    ret = parseSExp(&r2);
    if (ret == -1)
    {
        return -1;
    }

    if (op == 1)
    {
        *result = r2 * r1;
    }
    if (op == -1)
    {
        *result = r2 / r1;
    }

    return 0;
}

/**
 * Exp = Exp + SExp
 *     = Exp - SExp
 *     = SExp
 * 
 */
int parseExp(double *result)
{
    int ret;
    int op = 0;

    skipSpace();
    double r1;
    double r2;

    ret = parseSExp(&r1);

    if (ret != 0)
    {
        return -1;
    }

    skipSpace();

    if (buff[pos] == '+')
    {
        op = 1;
    }
    else if (buff[pos] == '-')
    {
        op = -1;
    }
    else
    {
        *result = r1;
        return 0;
    }

    pos--;
    skipSpace();

    ret = parseExp(&r2);

    if (ret != 0)
    {
        return -1;
    }

    *result = r2 + op * r1;
    return 0;
}

int main()
{
    int ret;
    buff = (char *)malloc(sizeof(char) * BUFF_SIZE);
    size_t LIMIT_SIZE = BUFF_SIZE;

    while (1)
    {
        ret = getline(&buff, &LIMIT_SIZE, stdin);

        if (ret == -1)
        {
            printf("Readline Error\n");
            printf("%s \n", strerror(errno));
            return 1;
        }

        pos = ret - 1;

        double result;
        ret = parseExp(&result);

        if (ret == -1)
        {
            return 1;
        }

        printf("result: %lf \n", result);

        memset(buff, 0, BUFF_SIZE);
    }

    return 0;
}
