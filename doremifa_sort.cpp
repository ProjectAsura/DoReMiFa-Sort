/*
    DoReMiFaSort
    English  : http://doremifasort.web.fc2.com/doremifasort_eng.html
    Japanese : http://doremifasort.web.fc2.com/doremifasort.html
*/

#include "doremifa_sort.h"
#include <emmintrin.h>

#pragma optimize("t", on)

static void __stdcall SwapUU(void* p1, void* p2)
{
    char* c1 = (char*)p1;
    char* c2 = (char*)p2;
    char cdata;
    size_t      sztRecLength = sizeof(sort_record);
    __m128i     XmmTemp;
    uint32_t    dwTemp;

    while (sztRecLength >= 16)
    {
        XmmTemp = _mm_loadu_si128((__m128i*) c1);
        _mm_storeu_si128((__m128i*) c1, _mm_loadu_si128((__m128i*) c2));
        _mm_storeu_si128((__m128i*) c2, XmmTemp);
        c1 += 16;
        c2 += 16;
        sztRecLength -= 16;
    }

    if (sztRecLength >= 8)
    {
        XmmTemp = _mm_loadl_epi64((__m128i*) c1);
        _mm_storel_epi64((__m128i*) c1, _mm_loadl_epi64((__m128i*) c2));
        _mm_storel_epi64((__m128i*) c2, XmmTemp);
        c1 += 8;
        c2 += 8;
        sztRecLength -= 8;
    }

    if (sztRecLength >= 4)
    {
        dwTemp = *((uint32_t*)c1);
        *((int*)c1) = *((uint32_t*)c2);
        *((uint32_t*)c2) = dwTemp;
        c1 += 4;
        c2 += 4;
        sztRecLength -= 4;
    }

    while (sztRecLength--)
    {
        cdata = *c1;
        *c1++ = *c2;
        *c2++ = cdata;
    }
}

static void __stdcall CopyRec(void* pDest, void* pSrc)
{
    char* cDst = (char*)pDest;
    char* cSrc = (char*)pSrc;
    char  cdata;
    size_t sztRecLength = sizeof(sort_record);

    while (sztRecLength >= 16)
    {
        _mm_storeu_si128((__m128i*) cDst, _mm_loadu_si128((__m128i*) cSrc));
        cDst += 16;
        cSrc += 16;
        sztRecLength -= 16;
    }
    if (sztRecLength >= 8)
    {
        _mm_storel_epi64((__m128i*) cDst, _mm_loadl_epi64((__m128i*) cSrc));
        cDst += 8;
        cSrc += 8;
        sztRecLength -= 8;
    }
    if (sztRecLength >= 4)
    {
        *((int*)cDst) = *((uint32_t*)cSrc);
        cDst += 4;
        cSrc += 4;
        sztRecLength -= 4;
    }
    while (sztRecLength--)
    {
        cdata   = *cDst;
        *cDst++ = *cSrc;
        *cSrc++ = cdata;
    }
}

static void __stdcall DualSort(sort_record* p1, sort_record* p2)
{
    if (p1->key > p2->key)
        SwapUU(p1, p2);
}

static void __stdcall TriSort(sort_record* p1, sort_record* p2, sort_record* p3)
{
    DualSort(p1, p2);
    DualSort(p2, p3);
    DualSort(p1, p2);
}

static void __stdcall InsertionP(sort_record* pTop, sort_record* pBtm)
{
    sort_record   TempData;
    sort_record*  ScanPtr;
    sort_record*  CopyPtr;

    ScanPtr = pBtm - 1;
    while (ScanPtr >= pTop)
    {
        if ((ScanPtr + 1)->key < ScanPtr->key)
        {
            CopyRec(&TempData, ScanPtr); CopyPtr = ScanPtr;
            do {
                CopyRec(CopyPtr, CopyPtr + 1); CopyPtr++;
            } while ((CopyPtr < pBtm) && ((CopyPtr + 1)->key < TempData.key));
            CopyRec(CopyPtr, &TempData);
        }
        ScanPtr--;
    }
}

uint32_t __stdcall DoReMiFaSortP(sort_record* pTop, sort_record* pBtm, bool* pAbort)
{
    sort_record* nPtrL;
    sort_record* nPtrR;
    sort_record* nPivotL;
    sort_record* nPivotR;
    sort_record* nTemp;
    sort_record* RecRe;
    sort_record* RecMe;
    sort_record* RecFa;
    sort_record  Pivot;

    if (pBtm > pTop)
    {
        if ((pBtm - pTop) > 48)	// 48 is best much value at win32 and x64
        {
            // Do-Re-Mi-Fa-Sort
            RecMe = (pBtm - pTop) / 2 + pTop;
            RecRe = (RecMe - pTop) / 2 + pTop;
            RecFa = (pBtm - RecMe) / 2 + RecMe;
            TriSort(pTop, RecRe, RecMe);
            TriSort(RecRe, RecMe, RecFa);
            TriSort(RecMe, RecFa, pBtm);
            DualSort(pTop, RecRe);
            DualSort(RecRe, RecMe);
            SwapUU(RecMe, pBtm);
            
            //Pivot = *pBtm;
            CopyRec(&Pivot, pBtm);
            
            // adjust pivot range
            nPivotR = pBtm;
            while ((nPivotR > pTop) && (nPivotR[-1].key == Pivot.key))
            {
                nPivotR--;
            }

            if (nPivotR == pTop)
                return 0;   // All records same data

            nPtrR   = nPivotR;
            nPivotL = pTop - 1;
            nPtrL   = pTop - 1;

            for (;;)
            {
                while ((++nPtrL)->key < Pivot.key);

                while (Pivot.key < (--nPtrR)->key)
                    if (nPtrR == pTop) break;

                if (nPtrL >= nPtrR)
                    break;

                SwapUU(nPtrL, nPtrR);

                if (nPtrL->key == Pivot.key)
                {
                    nPivotL++;
                    SwapUU(nPivotL, nPtrL);
                }
                if (Pivot.key == nPtrR->key)
                {
                    nPivotR--;
                    SwapUU(nPtrR, nPivotR);
                }
            }

            SwapUU(nPtrL, pBtm);
            nPtrR = nPtrL - 1;
            nPtrL = nPtrL + 1;

            for (nTemp = pTop; nTemp <= nPivotL; nTemp++, nPtrR--)
            {
                SwapUU(nTemp, nPtrR);
            }

            for (nTemp = pBtm - 1; nTemp >= nPivotR; nTemp--, nPtrL++)
            {
                SwapUU(nPtrL, nTemp);
            }

            if (*pAbort)
                return 1223;

            if (nPtrR > pTop)
                DoReMiFaSortP(pTop, nPtrR, pAbort);

            if (nPtrL < pBtm)
                DoReMiFaSortP(nPtrL, pBtm, pAbort);

        }
        else
        {
            InsertionP(pTop, pBtm);
        }
    }
    return 0;
}

uint32_t __stdcall doremifa_sort(sort_record* pArray, size_t pRecCount, bool* pAbort)
{
    auto result = 0u;

    if (pRecCount > 1)
        result = DoReMiFaSortP(pArray, &pArray[pRecCount - 1], pAbort);

    return result;
}
