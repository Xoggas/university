#include "dynamic_tests.h"

#pragma comment(lib, "lib1.lib")

HMODULE library;

typedef int (__stdcall *Sort_stdcall)(int* arr, int len, int* pos, int* neg, int* neg_count);

typedef int (__cdecl *Sort_cdecl)(int* arr, int len, int* pos, int* neg, int* neg_count);

static void sort_stdcall_explicit_test()
{
    auto sort = (Sort_stdcall)GetProcAddress(library, "Sort_stdcall_explicit");

    test_set test_set("Sort_stdcall_explicit");

    test_set.got_pos_count = sort(test_set.arr, test_set.test_size, test_set.got_pos_res,
                                  test_set.got_neg_res, &test_set.got_neg_count);

    test_set.validate();
}

static void sort_cdecl_explicit_test()
{
    auto sort = (Sort_cdecl)GetProcAddress(library, "Sort_cdecl_explicit");

    test_set test_set("Sort_cdecl_explicit");

    test_set.got_pos_count = sort(test_set.arr, test_set.test_size, test_set.got_pos_res,
                                  test_set.got_neg_res, &test_set.got_neg_count);

    test_set.validate();
}

static void sort_stdcall_implicit_test()
{
    auto sort = (Sort_stdcall)GetProcAddress(library, "_Sort_stdcall_implicit@20");

    test_set test_set("Sort_stdcall_implicit");

    test_set.got_pos_count = sort(test_set.arr, test_set.test_size, test_set.got_pos_res,
                                  test_set.got_neg_res, &test_set.got_neg_count);

    test_set.validate();
}

static void sort_cdecl_implicit_test()
{
    auto sort = (Sort_cdecl)GetProcAddress(library, "Sort_cdecl_implicit");

    test_set test_set("Sort_cdecl_implicit");

    test_set.got_pos_count = sort(test_set.arr, test_set.test_size, test_set.got_pos_res, test_set.got_neg_res,
                                  &test_set.got_neg_count);

    test_set.validate();
}

void run_dynamic_tests()
{
    library = LoadLibraryA("lib1.dll");

    if (library == nullptr)
    {
        std::cerr << "Failed to load library\n";
        return;
    }

    sort_stdcall_explicit_test();
    sort_cdecl_explicit_test();
    sort_stdcall_implicit_test();
    sort_cdecl_implicit_test();

    FreeLibrary(library);
}
