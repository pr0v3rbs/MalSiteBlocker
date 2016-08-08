#pragma once

#include <string.h>

#define ITEM_SIZE 39

struct DictionaryTree
{
    DictionaryTree() { memset(nextNode, 0, sizeof(struct DictionaryTree*) * ITEM_SIZE); isLast = false; }
    struct DictionaryTree *nextNode[ITEM_SIZE]; // a-z, '-', '.', '/', 0-9
    bool isLast;
};

void InitTree();

void InsertString(char* str);

bool CheckString(char* str);