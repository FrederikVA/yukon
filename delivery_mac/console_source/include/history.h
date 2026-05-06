#ifndef HISTORY_H
#define HISTORY_H

void recordUndoState(void);
int undoMove(void);
int redoMove(void);
void clearHistory(void);

#endif
