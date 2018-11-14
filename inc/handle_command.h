#ifndef __HANDLE_COMMAND_H__
#define __HANDLE_COMMAND_H__

void Handle_Command(char *s);
void params_fill(char *s, unsigned int *params);
int params_count(unsigned int *params);
int params_integer(char k, unsigned int *params);

#endif
