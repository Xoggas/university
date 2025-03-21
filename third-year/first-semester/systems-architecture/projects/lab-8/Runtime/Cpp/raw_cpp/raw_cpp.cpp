#include "raw_cpp.h"

static void insertion_sort(int arr[], const int len)
{
    for (int i = 1; i < len; i++)
    {
        int key = arr[i];
        int j = i - 1;

        while (j >= 0 && arr[j] > key)
        {
            arr[j + 1] = arr[j];
            j--;
        }

        arr[j + 1] = key;
    }
}

int Sort_raw_cpp(int* arr, int len, int* pos_res, int* neg_res, int* neg_count)
{
    int pos_count = 0;

    for (int i = 0; i < len; i++)
    {
        const auto value = arr[i];

        if (value >= 0)
        {
            pos_res[pos_count++] = value;
        }
    }

    for (int i = 0; i < len; i++)
    {
        const auto value = arr[i];

        if (value < 0)
        {
            neg_res[(*neg_count)++] = value;
        }
    }

    insertion_sort(pos_res, pos_count);

    insertion_sort(neg_res, *neg_count);

    return pos_count;
}

void run_raw_cpp_test()
{
    test_set test_set("raw_cpp");

    test_set.got_pos_count = Sort_raw_cpp(test_set.arr, test_set.test_size, test_set.got_pos_res, test_set.got_neg_res,
                                          &test_set.got_neg_count);

    test_set.validate();
}
