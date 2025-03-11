.386
.model flat, stdcall
option casemap: none

include windows.inc
include kernel32.inc
include msvcrt.inc
includelib kernel32.lib
includelib msvcrt.lib

.data
fmt_string db "%d", 0
new_line_string db 13, 10, 0
total_count_string db "Total: %d", 13, 10, 0

.code  
output proc                                   ; output(a)
    push ecx                                  ; сохраняем значение ecx

    mov eax, dword ptr [esp+8]                ; получаем значение а
    mov ecx, 12                               ; записываем 12 в регистр-счётчик

    print_loop:
        dec ecx                               ; ecx--;
        bt eax, ecx                           ; получаем значение бита по i-тому индексу
        mov edx, 0                            ; помещаем значение 0 в регистр edx
        adc edx, 0                            ; складываем значение регистра edx с флагом CF
        inc ecx                               ; ecx++;

        push eax                              ; запоминаем значение ax
        push ecx                              ; запоминаем значение ecx

        push edx                              ; помещаем значение бита в стек
        push offset fmt_string                ; помещаем адрес форматирующей строки в стек
        call crt_printf                       ; вызываем функцию вывода
        add esp, 4*2                          ; очищаем стек
            
        pop ecx                               ; восстанавливаем значение ecx
        pop eax                               ; восстанавливаем значение ax
        loop print_loop                       ; зацикливаем перебор битов

        push offset new_line_string           ; помещаем адрес форматирующей строки в стек
        call crt_printf                       ; вызываем функцию вывода
        add esp, 4                            ; очищаем стек

        pop ecx                               ; восстанавливаем значение ecx
        ret 4                                 ; возвращаемся, удаляя из стека двухбайтовый аргумент
output endp

count_bits proc
    push ecx                                  ; сохраняем значение cx

    mov eax, dword ptr [esp+8]                ; получаем значение а
    mov ecx, 12                               ; устанавливаем значение счётчика на 12
    mov edx, 0                                ; устанавливаем значение счётчика битов в 0        

    i_loop:
        dec ecx                               ; cx--;
        bt eax, ecx                           ; получаем значение бита по i-тому индексу
        adc edx, 0                            ; складываем значение регистра dx с флагом CF
        inc ecx                               ; cx++;
        loop i_loop                           ; зацикливаем перебор битов

    pop ecx                                   ; восстанавливаем значение cx 
    ret 4                                     ; возвращаемся, удаляя из стека двухбайтовый аргумент
count_bits endp

start:
    xor ecx, ecx                              ; обнуляем значение ecx
    mov ecx, 111111111111b                    ; заполняем счётчик 12-ю битами
    mov ebx, 0                                ; обнуляем счётчик выведенных чисел

    i_loop:
        push ecx                              ; помещаем значение 
        call count_bits                       ; вызываем функцию подсчёта битов в числе, результат будет в edx

        mov eax, 3                            ; помещаем ожидаемое кол-во битов-единичек в eax
        cmp edx, eax                          ; сравениваем подсчитанное кол-во битов с ожидаемым значением
        jne continue                          ; если edx != 3, то перейти к continue

    print:
        inc ebx                               ; увеличиваем количество выведенных чисел
        push ebx                              ; запоминаем ebx
        push ecx                              ; помещаем число с 3-мя битами-единичками в стек
        call output                           ; выводим его
        pop ebx                               ; восстанавливаем ebx

    continue:
        loop i_loop                           ; зацикливаем перебор чисел

    push ebx                                  ; помещаем подсчитанное количество чисел в стек
    push offset total_count_string            ; помещаем адрес форматирующей строки в стек
    call crt_printf                           ; вызываем функцию вывода
    add esp, 4*2                              ; очищаем стек

    push 0                                    ; помещаем exit code в стек
    call ExitProcess                          ; выходим из программы
end start