#define _GNU_SOURCE

#include <ft_stdlib.h>
#include <ft_string.h>
#include <ft_unistd.h>
#include <signal.h>
#include <uio.h>

#include "exec.h"
#include "prompt.h"

#define MINISHELL_INPUT_BUFSIZE 8
#define MINISHELL_INPUT_DELIMITERS "\t\n\v\f\r "

/**
 * Splits input into arguments.
 */
static char **
sh_split(char *line, int *linec)
{
	int bufsize = MINISHELL_INPUT_BUFSIZE;
	char **tokens = ft_malloc(sizeof *tokens * bufsize);
	char *token = ft_strtok(line, MINISHELL_INPUT_DELIMITERS);
	int i = 0;
	while (token)
	{
		/**
		 * Environment variable expansion.
		 */
		char *expand = ft_strchr(token, '$');
		if (expand)
		{
			expand++;
			size_t etl = expand - token - 1;
			char *expanded = ft_getenv(expand);
			if (expanded)
			{
				size_t el = ft_strlen(expanded);
				tokens[i] = ft_malloc(sizeof *tokens * (el + etl));
				ft_memcpy(tokens[i], token, etl);
				ft_memcpy(tokens[i] + etl, expanded, el);
				tokens[i][el + etl] = '\0';
			}
			else
				tokens[i] = ft_strndup(token, etl);
		}
		else
			tokens[i] = ft_strdup(token);

		/**
		 * '~' expansion.
		 */
		if (*tokens[i] == '~')
		{
			char *home_expand = ft_getenv("HOME");
			size_t hl = ft_strlen(home_expand);
			size_t tl = ft_strlen(tokens[i]);
			char *tmp = ft_malloc(sizeof *tmp * (hl + tl - 1));
			ft_memcpy(tmp, home_expand, hl);
			ft_memcpy(tmp + hl, tokens[i] + 1, tl - 1);
			tmp[hl + tl - 1] = '\0';
			ft_free(tokens[i]);
			tokens[i] = tmp;
		}

		i++;
		if (i >= bufsize)
		{
			bufsize += MINISHELL_INPUT_BUFSIZE;
			char **t;
			if (!(t = ft_malloc(sizeof *t * bufsize)))
				exit(EXIT_FAILURE);
			for (int j = 0; j < bufsize - MINISHELL_INPUT_BUFSIZE; j++)
				t[j] = tokens[j];
			ft_free(tokens);
			tokens = t;
		}
		token = ft_strtok(NULL, MINISHELL_INPUT_DELIMITERS);
	}
	tokens[i] = NULL;
	*linec = i;
	return (tokens);
}

static void
print_version(void)
{
	uputs("minishell, version 0.1\n");
	exit(EXIT_SUCCESS);
}

static void
print_usage(void)
{
	uputs("Usage: minishell [-vh]\n");
	uputs("\n\t-v\tPrint version\n");
	exit(EXIT_SUCCESS);
}

static void
sigint_shell()
{
	uputc('\n');
	prompt();
}

int
main(int argc, char **argv)
{
	char ch;
	char *shell = argv[0];
	while ((ch = ft_getopt(argc, argv, "v")) != -1)
	{
		if (ch == 'v')
			print_version();
		else if (ch == 'h')
			print_usage();
		else
			print_usage();
	}
	argc -= g_optind;
	argv += g_optind;

	ft_setenv("SHELL", shell, 1);
	while (1)
	{
		signal(SIGINT, sigint_shell);
		prompt();
		char *line = NULL;
		if (ugetl(&line) == -1)
		{
			uputc('\n');
			exit(EXIT_FAILURE);
		}
		if (!line)
		{
			uputc('\n');
			exit(EXIT_SUCCESS);
		}
		int lc;
		char **lv = sh_split(line, &lc);
		ft_free(line);
		if (!lv)
		{
			char **lvp = lv;
			while (*lvp)
			{
				ft_free(*lvp);
				lvp++;
			}
			ft_free(lv);
			exit(EXIT_FAILURE);
		}
		int status = sh_exec(lc, lv);
		char **lvp = lv;
		while (*lvp)
		{
			ft_free(*lvp);
			lvp++;
		}
		ft_free(lv);
		if (status == -42)
			exit(EXIT_SUCCESS);
	}
}
