.386
.model flat, stdcall
option casemap: none

include windows.inc
include kernel32.inc
include msvcrt.inc
includelib kernel32.lib
includelib msvcrt.lib

.data
    fmt_string db "Length: %d",10,0        ; форматирующая строка
    a dw -1,5,6,7,-3,-4                    ; массив чисел (a[i] - 2 байта)
    n dd 6                                 ; длина массива (4 байта)

.code
count proc
    push ebp                               ; сохраняем ebp в стек      
    mov ebp, esp                           ; ebp = esp
    add ebp, 8                             ; сдвигаем указатель на стек на 8, чтобы он указывал на массив

    push ecx                               ; сохраняем ecx
    push edx                               ; сохраняем edx 

    mov eax, 0                             ; eax = 0 (счётчик положительных чисел)
    mov edx, [ebp]                         ; edx = *a
    mov ecx, [ebp+4]                       ; ecx = i = n

    filter_loop:
        mov bx, word ptr [edx]             ; bx = a[i]
        mov si, 0                          ; si = 0 (для сравнения)
        cmp bx, si                         ; сравниваем a[i] и 0
        jle continue                       ; если a[i] <= 0, то перейти к continue
        inc eax                            ; иначе увеличиваем счётчик eax

        continue:
            add edx, 2                     ; сдвигаем вперёд указатель на элемент массива а
            loop filter_loop               ; переходим на следующую итерацию

    pop edx                                ; возвращаем edx из стека
    pop ecx                                ; возвращаем ecx из стека
    pop ebp                                ; возвращаем ebp из стека
    ret 8                                  ; очищаем два аргумента, оба размером в 4 байта
count endp

start:
    push n                                 ; помещаем количество элементов массива в стек
    push offset a                          ; помещаем указатель на массив а в стек 
    call count                             ; вызываем процедуру, результат будет в eax

    push eax                               ; кладём количество положительных элементов в стек
    push offset fmt_string                 ; кладём указатель на форматирующую строку в стек
    call crt_printf                        ; вызываем функцию вывода

    push 0                                 ; помещаем exit code в стек
    call ExitProcess                       ; выходим из программы
end start