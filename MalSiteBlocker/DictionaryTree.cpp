#include "DictionaryTree.h"

struct DictionaryTree *dHead;

void InitTree()
{
    dHead = new DictionaryTree();
}

void InsertString(char* str)
{
    struct DictionaryTree *current = dHead;

    while (true)
    {
        if (*str >= 'a') // alphabet
        {
            if (!current->nextNode[*str - 'a'])
            {
                current->nextNode[*str - 'a'] = new DictionaryTree();
            }

            current = current->nextNode[*str - 'a'];
        }
        else // dot
        {
            if (!current->nextNode[26])
            {
                current->nextNode[26] = new DictionaryTree();
            }

            current = current->nextNode[26];
        }

        if (!(*++str))
        {
            current->isLast = true;
            break;
        }
    }
}

void ClearTree()
{

}

bool CheckString(char* str)
{

}