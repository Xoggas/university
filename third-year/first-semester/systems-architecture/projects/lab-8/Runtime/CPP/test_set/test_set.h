#pragma once

#include <cassert>
#include <fstream>
#include <iostream>
#include <random>

#define ARRAY_LENGTH 25000

class test_set
{
public:
    std::string test_name;
    int test_size;
    int arr[ARRAY_LENGTH];

    int expected_pos_res[ARRAY_LENGTH];
    int expected_neg_res[ARRAY_LENGTH];
    int expected_pos_count = 0;
    int expected_neg_count = 0;

    int got_pos_res[ARRAY_LENGTH];
    int got_neg_res[ARRAY_LENGTH];
    int got_pos_count = 0;
    int got_neg_count = 0;

    explicit test_set(std::string test_name, int test_size = 20) : test_name(std::move(test_name)), test_size(test_size)
    {
        read_numbers_from_file();
        memset(expected_pos_res, 0, test_size * sizeof(int));
        memset(expected_neg_res, 0, test_size * sizeof(int));

        for (int i = 0; i < test_size; i++)
        {
            const auto value = arr[i];

            if (value >= 0)
            {
                expected_pos_res[expected_pos_count++] = value;
            }
        }

        std::sort(expected_pos_res, expected_pos_res + expected_pos_count);

        for (int i = 0; i < test_size; i++)
        {
            const auto value = arr[i];

            if (value < 0)
            {
                expected_neg_res[expected_neg_count++] = value;
            }
        }

        std::sort(expected_neg_res, expected_neg_res + expected_neg_count);

        memset(got_pos_res, 0, test_size * sizeof(int));
        memset(got_neg_res, 0, test_size * sizeof(int));
    }

    void validate() const
    {
        std::cout << "Validating " << test_name << "\n";

        assert(got_pos_count == expected_pos_count && "got_pos_count != expected_pos_count\n");

        for (int i = 0; i < expected_pos_count; i++)
        {
            assert(expected_pos_res[i] == got_pos_res[i] && "pos array is not the same\n");
        }

        assert(got_neg_count == expected_neg_count && "got_neg_count != expected_neg_count\n");

        for (int i = 0; i < expected_neg_count; i++)
        {
            assert(expected_neg_res[i] == got_neg_res[i] && "neg array is not the same\n");
        }

        std::cout << test_name << " is valid\n\n";
    }

private:
    void read_numbers_from_file()
    {
        std::ifstream file("test_data.txt");

        if (!file)
        {
            std::cerr << "Couldn't read the file\n";

            return;
        }

        int count = 0;

        while (count < test_size && file >> arr[count])
        {
            count++;
        }

        file.close();
    }
};
