#ifndef MONITOR_HANDLER_H
#define MONITOR_HANDLER_H

#define DONT_JUMP_LINE   0
#define JUMP_LINE   1

#define TEXT_COLOR  (byte)(0b00000011)
#define ERROR_TEXT_COLOR  (byte)(0x1F)

void MonitorPrint(byte* string, dword length, dword type);
void MonitorJumpLine();
void MonitorPrintAt(byte* string, dword length, dword index);
void PrintDevideZeroError();
void PrintGPError();
void PrintUDError();
void PrintDFError();

#endif