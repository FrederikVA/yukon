#ifndef PRINTER_H
#define PRINTER_H

void printToTerminal();
void printBoard();
void printLastCommand();
void printMessage();
void printInputPrompt();
void refreshScreen();
void printHelp();

extern char lastCommand[100]; 
extern char message[200];

#endif