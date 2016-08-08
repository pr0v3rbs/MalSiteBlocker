#include "DictionaryTree.h"

struct DictionaryTree *dHead;

void InitTree()
{
    dHead = new DictionaryTree();
}

inline char Convert(char inChar)
{
    if (inChar >= 'a') // alphabet 97 ~ 122 : 26
        return inChar - 'a';
    else // '-', '.', '/', 0~9  : 13
        return (26 + inChar - '-');
}

void InsertString(char* str)
{
    struct DictionaryTree *current = dHead;
    char temChar;

    while (true)
    {
        temChar = Convert(*str);

        if (!current->nextNode[temChar])
            current->nextNode[temChar] = new DictionaryTree();
        current = current->nextNode[temChar];

        if (!(*++str))
        {
            current->isLast = true;
            break;
        }
    }
}

bool CheckString(char* str)
{
    bool result = false;
    struct DictionaryTree *current = dHead;
    char temChar;
    char *testStr = str;

    while (testStr && !result)
    {
        str = testStr;
        testStr = NULL;

        while (true)
        {
            if ((temChar = Convert(*str)) == 27 && !testStr)    // index of '.' from local array
                testStr = str + 1;

            // fail
            if (!current->nextNode[temChar])
            {
                current = dHead;

                if (testStr)
                    break;

                while (*str && *str != '.')
                    ++str;

                if (*str && !testStr)
                    testStr = str + 1;

                break;
            }

            current = current->nextNode[temChar];

            if (!(*++str))
            {
                result = current->isLast;
                break;
            }
        }
    }

    return result;
}