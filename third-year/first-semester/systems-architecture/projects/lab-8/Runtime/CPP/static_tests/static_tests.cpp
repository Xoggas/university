#include "static_tests.h"

#pragma comment(lib, "lib1.lib")

extern "C" __declspec(dllimport) int __stdcall Sort_stdcall_explicit(int* arr, int len, int* pos, int* neg,
                                                                     int* neg_count);

extern "C" __declspec(dllimport) int __cdecl Sort_cdecl_explicit(int* arr, int len, int* pos, int* neg,
                                                                 int* neg_count);

extern "C" __declspec(dllimport) int __stdcall Sort_stdcall_implicit(int* arr, int len, int* pos, int* neg,
                                                                     int* neg_count);

extern "C" __declspec(dllimport) int __cdecl Sort_cdecl_implicit(int* arr, int len, int* pos, int* neg,
                                                                 int* neg_count);

int Sort_asm(int* arr, int len, int* pos, int* neg, int* neg_count)
{
    return Sort_stdcall_explicit(arr, len, pos, neg, neg_count);
}

static void sort_stdcall_explicit_test()
{
    test_set test_set("Sort_stdcall_explicit");

    test_set.got_pos_count = Sort_stdcall_explicit(test_set.arr, test_set.test_size, test_set.got_pos_res,
                                                   test_set.got_neg_res, &test_set.got_neg_count);

    test_set.validate();
}

static void sort_cdecl_explicit_test()
{
    test_set test_set("Sort_cdecl_explicit");

    test_set.got_pos_count = Sort_cdecl_explicit(test_set.arr, test_set.test_size, test_set.got_pos_res,
                                                 test_set.got_neg_res, &test_set.got_neg_count);

    test_set.validate();
}

static void sort_stdcall_implicit_test()
{
    test_set test_set("Sort_stdcall_implicit");

    test_set.got_pos_count = Sort_stdcall_implicit(test_set.arr, test_set.test_size, test_set.got_pos_res,
                                                   test_set.got_neg_res, &test_set.got_neg_count);

    test_set.validate();
}

static void sort_cdecl_implicit_test()
{
    test_set test_set("Sort_cdecl_implicit");

    test_set.got_pos_count = Sort_cdecl_implicit(test_set.arr, test_set.test_size, test_set.got_pos_res,
                                                 test_set.got_neg_res, &test_set.got_neg_count);

    test_set.validate();
}

void run_static_tests()
{
    sort_stdcall_explicit_test();
    sort_cdecl_explicit_test();
    sort_stdcall_implicit_test();
    sort_cdecl_implicit_test();
}
