/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Microshell_bis.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clwenhaj <clwenhaj@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 12:46:26 by clwenhaj          #+#    #+#             */
/*   Updated: 2026/04/27 15:07:08 by clwenhaj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"

void	fatal()
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
	if (dup2(tmp_fd,STDIN_FILENO) == -1)
		fatal();
	if (close(tmp_fd) == -1)
		fatal();
	av[i] = NULL;
	execve(av[0], av, env);
	ft_putstr_fd2("error: cannot execute ", av[0]);
	exit(1);
}

int	main(int ac, char **av, char **env)
{
	int	i = 1;
	int	fd[2];
	int	tmp_fd;

	if ((tmp_fd = dup(STDIN_FILENO)) == -1)
		fatal();
	if (ac > 1)
	{
		while (av[i])
		{
			char	**cmd = &av[i];
			int	j = 0;

			while (cmd[j] && strcmp(cmd[j], "|") && strcmp(cmd[j], ";"))
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
					pid_t	pid = fork();
					if (pid < 0)
						fatal();
					if (pid == 0)
						ft_exec(cmd, j, tmp_fd, env);
					else
					{
						if (close(tmp_fd) == -1)
							fatal();
						while (waitpid(-1, NULL, 0) > 0)
							;
						if ((tmp_fd = dup(STDIN_FILENO)) == -1)
							fatal();
					}
				}
				else if (strcmp(cmd[j], "|") == 0)
				{
					if (pipe(fd) == -1)
						fatal();
					pid_t	pid = fork();
					if (pid < 0)
						fatal();
					if (pid == 0)
					{
						if (dup2(fd[1], STDOUT_FILENO) == -1)
							fatal();
						if (close(fd[1]) || close(fd[0]))
							fatal();
						ft_exec(cmd, j, tmp_fd, env);
					}
					else
					{
						if (close(fd[1]) || close(tmp_fd))
							fatal();
						if ((tmp_fd = fd[0]) == -1)
							fatal();
					}
				}
			}
			i += j;
			if (av[i] && (!strcmp(av[i], ";") || !strcmp(av[i], "|")))
				i++;
		}
		if (close(tmp_fd) == -1)
			fatal();
	}
	return (0);
}
