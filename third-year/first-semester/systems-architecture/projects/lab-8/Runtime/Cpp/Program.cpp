#include "dynamic_tests/dynamic_tests.h"
#include "nested_asm/nested_asm.h"
#include "raw_cpp/raw_cpp.h"
#include "static_tests/static_tests.h"

void run_test(std::string test_name, int (*f)(int*, int, int*, int*, int*))
{
    int test_sizes[] = {2500, 5000, 7500, 10000, 12500, 15000, 17500, 20000, 22500, 25000};

    std::cout << test_name << "\n";

    for (auto& test_size : test_sizes)
    {
        test_set test_set(test_name, test_size);

        auto start_time = GetTickCount();
        
        test_set.got_pos_count = f(test_set.arr, test_set.test_size, test_set.got_pos_res, test_set.got_neg_res,
                                   &test_set.got_neg_count);

        auto elapsed_time = GetTickCount() - start_time;

        std::cout << "Size: " << test_size << " Total time: " << elapsed_time << " ms\n";
    }

    std::cout << "\n";
}

int main(int argc, char** argv)
{
    if (argc == 1)
    {
        std::cout << "No arguments were passed\n";
        return 0;
    }

    if (std::strcmp(argv[1], "run_static_tests") == 0)
    {
        run_static_tests();
    }

    if (std::strcmp(argv[1], "run_asm_test") == 0)
    {
        run_nested_asm_test();
    }

    if (std::strcmp(argv[1], "run_dynamic_tests") == 0)
    {
        run_dynamic_tests();
    }

    if (std::strcmp(argv[1], "run_raw_cpp_test") == 0)
    {
        run_raw_cpp_test();
    }

    if (std::strcmp(argv[1], "time_tests") == 0)
    {
        run_test("Sort_asm", Sort_asm);
        run_test("Sort_nested_asm", Sort_nested_asm);
        run_test("Sort_raw_cpp", Sort_raw_cpp);
    }
}


