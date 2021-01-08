/*
    DoReMiFaSort
    English  : http://doremifasort.web.fc2.com/doremifasort_eng.html
    Japanese : http://doremifasort.web.fc2.com/doremifasort.html
*/
#pragma once

#include <cstdint>

struct sort_record
{
    uint32_t    key;
    void*       value;
};

uint32_t __stdcall doremifa_sort(sort_record* pArray, size_t pRecCount, bool* pAbort);
