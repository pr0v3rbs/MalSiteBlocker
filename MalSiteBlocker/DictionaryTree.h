#pragma once

#include <string.h>

struct DictionaryTree
{
    DictionaryTree() { memset(nextNode, 0, sizeof(struct DictionaryTree*) * 27); isLast = false; }
    struct DictionaryTree *nextNode[27];
    bool isLast;
};

void InitTree();

void InsertString(char* str);

void ClearTree();

bool CheckString(char* str);