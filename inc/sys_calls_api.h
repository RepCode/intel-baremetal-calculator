#ifndef SYS_CALLS_API_H
#define SYS_CALLS_API_H

#include "commonHelpers.h"

dword __sys_call_finish_task__();
dword __sys_call_monitor_print_at__(byte* string, dword length, dword index);
dword __sys_call_get_digits__(void* buffer, dword sizeBytes);
byte __sys_call_read_byte__(dword address);
dword __sys_call_go_hlt__();
dword __sys_call_get_last_key__(byte*);
dword __sys_call_add_number__(qword*);
dword __sys_call_make_double_fault__();

#endif
