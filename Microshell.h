#ifndef MICROSHELL_H
# define MICROSHELL_H

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

void    fatal(void);
void    ft_putstr_fd2(char *str, char *arg);
void    ft_exec(char **av, int i, int tmp_fd, char **env);

#endif