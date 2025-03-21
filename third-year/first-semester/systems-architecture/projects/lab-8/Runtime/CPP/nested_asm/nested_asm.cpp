#include "nested_asm.h"

void InsertionSort(int* array, int count)
{
    __asm
        {
        push esi
        push ecx
        push eax
        push ebx
        push edx

        mov eax, 1
        cmp count, eax
        jle sort_end

        mov esi, array
        add esi, 4
        mov ecx, 1

        i_loop:
        mov eax, count
        cmp ecx, eax
        jl j_loop
        jmp sort_end

        j_loop:
        push ecx
        push esi

        j_loop_first_check:
        mov eax, 0
        cmp ecx, eax
        jg j_loop_second_check
        jmp j_loop_end

        j_loop_second_check:
        mov eax, [esi]
        mov ebx, [esi-4]
        cmp ebx, eax
        jg j_loop_iteration
        jmp j_loop_end

        j_loop_iteration:
        mov eax, [esi]
        mov ebx, [esi-4]
        mov edx, eax
        mov [esi], ebx
        mov [esi-4], edx
        dec ecx
        sub esi, 4
        jmp j_loop_first_check

        j_loop_end:
        pop esi
        pop ecx
        inc ecx
        add esi, 4
        jmp i_loop

        sort_end:
        pop edx
        pop ebx
        pop eax
        pop ecx
        pop esi
        }
}

int Sort_nested_asm(int* arr, int len, int* pos_res, int* neg_res, int* neg_count)
{
    int pos_count = 0;

    __asm
        {
        push esi
        push ecx
        push ebx
        push edx

        mov eax, 0
        push eax

        mov esi, arr
        mov ecx, len
        mov ebx, pos_res

        select_positive_loop:
        mov eax, 0
        cmp dword ptr [esi], eax
        jge select_positive_loop_add
        jmp select_positive_loop_skip

        select_positive_loop_add:
        mov eax, [esi]
        mov dword ptr [ebx], eax
        add ebx, 4
        inc dword ptr [esp]

        select_positive_loop_skip:
        add esi, 4
        loop select_positive_loop

        mov esi, arr
        mov ecx, len
        mov ebx, neg_res
        mov edx, neg_count

        select_negative_loop:
        mov eax, 0
        cmp dword ptr [esi], eax
        jl select_negative_loop_add
        jmp select_negative_loop_skip

        select_negative_loop_add:
        mov eax, [esi]
        mov dword ptr [ebx], eax
        add ebx, 4
        inc dword ptr [edx]

        select_negative_loop_skip:
        add esi, 4
        loop select_negative_loop

        pop eax
        mov pos_count, eax
        pop edx
        pop ebx
        pop ecx
        pop esi
        }

    InsertionSort(pos_res, pos_count);

    InsertionSort(neg_res, *neg_count);

    return pos_count;
}

void run_nested_asm_test()
{
    test_set test_set("Sort_nested_asm");

    test_set.got_pos_count = Sort_nested_asm(test_set.arr, test_set.test_size, test_set.got_pos_res,
                                             test_set.got_neg_res, &test_set.got_neg_count);

    test_set.validate();
}
