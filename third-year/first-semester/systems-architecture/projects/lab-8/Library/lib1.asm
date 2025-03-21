.386
.model flat, stdcall
option casemap: none

.code
    DllMain proc hlnstDLL:DWORD, reason:DWORD, unused:DWORD
        mov eax, 1 
        ret
    DllMain endp

    InsertionSort proc array: DWORD, count: DWORD
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
            ret 8
    InsertionSort endp

    Sort_stdcall_implicit proc stdcall
        push ebp
        mov ebp, esp

        push esi
        push ecx
        push ebx
        push edx
        
        mov eax, 0
        push eax

        mov esi, [ebp+8]
        mov ecx, [ebp+12]
        mov ebx, [ebp+16]

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
        
        mov esi, [ebp+8]
        mov ecx, [ebp+12]
        mov ebx, [ebp+20]
        mov edx, [ebp+24]

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

        mov edx, [ebp+24]
        push dword ptr [edx]
        push dword ptr [ebp+20]
        call InsertionSort

        mov eax, [esp]
        push dword ptr [esp]
        push dword ptr [ebp+16]
        call InsertionSort

        pop eax
        pop edx
        pop ebx
        pop ecx
        pop esi
        pop ebp
        ret 20
    Sort_stdcall_implicit endp

    Sort_cdecl_implicit proc c
        push ebp
        mov ebp, esp

        push esi
        push ecx
        push ebx
        push edx
        
        mov eax, 0
        push eax

        mov esi, [ebp+8]
        mov ecx, [ebp+12]
        mov ebx, [ebp+16]

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
        
        mov esi, [ebp+8]
        mov ecx, [ebp+12]
        mov ebx, [ebp+20]
        mov edx, [ebp+24]

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

        mov edx, [ebp+24]
        push dword ptr [edx]
        push dword ptr [ebp+20]
        call InsertionSort

        mov eax, [esp]
        push dword ptr [esp]
        push dword ptr [ebp+16]
        call InsertionSort

        pop eax
        pop edx
        pop ebx
        pop ecx
        pop esi
        pop ebp
        ret
    Sort_cdecl_implicit endp

    Sort_fastcall_implicit proc fastcall
        push edx
        push ecx
        push esi
        push ebx

        mov eax, 0
        push eax

        mov esi, [esp+12]
        mov ecx, [esp+16]
        mov ebx, [esp+24]

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
        
        mov esi, [esp+12]
        mov ecx, [esp+16]
        mov ebx, [esp+28]
        mov edx, [esp+32]

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

        mov edx, [esp+32]
        push dword ptr [edx]
        push dword ptr [esp+32]
        call InsertionSort

        mov eax, [esp]
        push dword ptr [esp]
        push dword ptr [esp+28]
        call InsertionSort

        pop eax
        pop edx
        pop ebx
        pop ecx
        pop esi
        ret 12
    Sort_fastcall_implicit endp

    Sort_stdcall_explicit proc stdcall arr: DWORD, len: DWORD, pos_res: DWORD, neg_res: DWORD, neg_count: DWORD
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

        mov edx, neg_count
        push dword ptr [edx]
        push neg_res
        call InsertionSort

        mov eax, [esp]
        push dword ptr [esp]
        push pos_res
        call InsertionSort

        pop eax
        pop edx
        pop ebx
        pop ecx
        pop esi
        ret 20
    Sort_stdcall_explicit endp

    Sort_cdecl_explicit proc c arr: DWORD, len: DWORD, pos_res: DWORD, neg_res: DWORD, neg_count: DWORD
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

        mov edx, neg_count
        push dword ptr [edx]
        push neg_res
        call InsertionSort

        mov eax, [esp]
        push dword ptr [esp]
        push pos_res
        call InsertionSort

        pop eax
        pop edx
        pop ebx
        pop ecx
        pop esi
        ret
    Sort_cdecl_explicit endp

end DllMain