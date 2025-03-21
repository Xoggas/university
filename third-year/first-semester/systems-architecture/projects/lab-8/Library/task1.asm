.386
.model flat, stdcall
option casemap: none

include windows.inc
include kernel32.inc
include msvcrt.inc
includelib kernel32.lib
includelib msvcrt.lib

.data
    array dd 10, 0, -1, 5, 6, 7, 8, -2, -5, -4
    len dd 10
    neg_res dd 10 dup(0)
    pos_res dd 10 dup(0)
    neg_count dd 0
    pos_count dd 0

.code  
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

    mov esi, [ebp]
    mov ecx, [ebp+4]
    mov ebx, [ebp+8]

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
    
    mov esi, [ebp]
    mov ecx, [ebp+4]
    mov ebx, [ebp+12]
    mov edx, [ebp+16]

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

    mov edx, [ebp+16]
    push dword ptr [edx]
    push dword ptr [ebp+12]
    call InsertionSort

    mov eax, [esp]
    push dword ptr [esp]
    push dword ptr [ebp+8]
    call InsertionSort

    pop edx
    pop ebx
    pop ecx
    pop esi
    pop ebp
    ret 20
Sort_stdcall_implicit endp  

PrintArray proc stdcall array: DWORD, len: DWORD
    push eax
    push ecx
PrintArray endp

    push 0        
    call ExitProcess
end start