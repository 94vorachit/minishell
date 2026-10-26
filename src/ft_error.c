#include "../includes/minishell.h"

void	ft_error(int error_code)
{
	(void)error_code;
	ft_putstr_fd("minishell: memory allocation failed\n", STDERR);
	exit(ERROR);
}
