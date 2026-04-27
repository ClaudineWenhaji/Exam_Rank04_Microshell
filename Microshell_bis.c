/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Microshell_ter.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clwenhaj <clwenhaj@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 14:23:59 by clwenhaj          #+#    #+#             */
/*   Updated: 2026/04/27 15:06:39 by clwenhaj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"

void	fatal(void)
{
	write(2, "error: fatal\n", 13);
	exit(1);
}

void	ft_putstr_fd2(char *str, char *arg)
{
	while (*str)
		write(2, str++, 1);
	if (arg)
		while (*arg)
			write(2, arg++, 1);
	write(2, "\n", 1);
}

void	ft_exec(char **av, int i, int tmp_fd, char **env)
{
	dup2(tmp_fd, STDIN_FILENO);
	close(tmp_fd);
	av[i] = NULL;
	execve(av[0], av, env);
	ft_putstr_fd2("error: cannot execute ", av[0]);
	exit(1);
}

int	main(int ac, char **av, char **env)
{
	int	i = 1;
	int	fd[2];
	int	tmp_fd = dup(STDIN_FILENO);

	if (ac > 1)
	{
		while (av[i])
		{
			int	j = 0;
			char	**cmd = &av[i];
	
			while (cmd[j] && strcmp(cmd[j], ";") && strcmp(cmd[j], "|"))
				j++;
			if (j > 0)
			{
				if (strcmp(cmd[0], "cd") == 0)
				{
					if (j != 2)
						ft_putstr_fd2("error: cd: bad arguments", NULL);
					else if (chdir(cmd[1]) != 0)
						ft_putstr_fd2("error: cd: cannot change directory to ", cmd[1]);
				}
				else if (cmd[j] == NULL || strcmp(cmd[j], ";") == 0)
				{
					if (fork() == 0)
						ft_exec(cmd, j, tmp_fd, env);
					else
					{
						close(tmp_fd);
						while (waitpid(-1, NULL, 0) > 0)
							;
						tmp_fd = dup(STDIN_FILENO);
					}
				}
				else if (strcmp(cmd[j], "|") == 0)
				{
					pipe(fd);
					if (fork() == 0)
					{
						dup2(fd[1], STDOUT_FILENO);
						close(fd[0]);
						close(fd[1]);
						ft_exec(cmd, j, tmp_fd, env);
					}
					else
					{
						close(fd[1]);
						close(tmp_fd);
						tmp_fd = fd[0];
					}
				}
			}
			i += j;
			if (av[i] && (!strcmp(av[i], ";") || !strcmp(av[i], "|")))
				i++;
		}
		close(tmp_fd);
	}
	return (0);
}
