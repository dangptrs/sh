#include <ft_stdlib.h>
#include <ft_string.h>
#include <ft.h>

#ifdef DEBUG
#include <stdio.h>
#endif /* !DEBUG */

#include "env.h"

/**
 * Count the amount of elements in char **environ;
 */
static size_t
arr_size(char **arr)
{
	size_t i = 0;
	while (arr[i])
		i++;
	return (i);
}

char *
lenv_getenv(const char *name)
{
	char **env = g_env;
	size_t l = ft_strchrnul(name, '=') - name;
	if (l && !name[l] && env)
	{
		env = g_env;
		while (*env)
		{
			if (!ft_strncmp(name, *env, l) && l[*env] == '=')
			{
#ifdef DEBUG
				fprintf(stderr, "DEBUG: lenv_getenv(\"%s\"): \"%s\"\n",
						name, *env + l + 1);
#endif /* !DEBUG */
				return (*env + l + 1);
			}
			env++;
		}
	}
#ifdef DEBUG
	fprintf(stderr, "DEBUG: lenv_getenv(\"%s\"): (null)\n", name);
#endif /* !DEBUG */
	return (NULL);
}

/**
 * Check if key exists in environment table, unset and recreate the table.
 */
void
lenv_unset(char *key)
{
#ifdef DEBUG
	fprintf(stderr, "DEBUG: lenv_unset(\"%s\")\n", key);
#endif /* !DEBUG */
	int size = arr_size(g_env);
	int i = 0;
	while (i <= size)
	{
		size_t ks = ft_strlen(key);
		if (!ft_strncmp(key, g_env[i], ks) && g_env[i][ks] == '=')
			break;
		i++;
	}
	if (i == size)
		return;
	char **env = malloc(sizeof (*env) * (size - 1));
	int j = 0;
	while (j < size)
	{
		if (j != i)
			env[j] = g_env[j];
#ifdef DEBUG
		else
			fprintf(stderr, "DEBUG: g_env[%d] = \"%s\": SKIPPED\n", j, g_env[j]);
#endif /* !DEBUG */
		j++;
	}
	size = j;
	env[size] = NULL;
	free(g_env);
	g_env = env;
#ifdef DEBUG
	fprintf(stderr, "DEBUG: g_env: [0] = \"%s\" [%d] = \"%s\" [%d] = \"%s\"\n",
			g_env[0], size - 1, g_env[size - 1], size, g_env[size]);
#endif /* !DEBUG */
}

/**
 * Add to back or replace the environment variable
 */
void
lenv_set(char *key, char *val)
{
	size_t ks = strlen(key);
	size_t vs = strlen(val);
	char *var = calloc(1, sizeof (*var) * (ks + 1 + vs));
	memcpy(var, key, ks);
	var[ks] = '=';
	memcpy(var + ks + 1, val, vs);
	var[ks + 1 + vs] = '\0';
	int size = arr_size(g_env) + 1;
	if (!(g_env = realloc(g_env, sizeof (*g_env) * size)))
		exit(1);
	g_env[size - 1] = var;
	g_env[size] = NULL;
#ifdef DEBUG
	fprintf(stderr, "DEBUG: lenv_set(\"%s\", \"%s\"): var = \"%s\"\n", key, val, var);
	fprintf(stderr, "DEBUG: g_env: [0] = \"%s\" [%d] = \"%s\" [%d] = \"%s\"\n",
			g_env[0], size - 1, g_env[size - 1], size, g_env[size]);
#endif /* !DEBUG */
}

/**
 * Initialization of local environment table.
 * Note: This will utilize a global variable because we will need it in order
 * to be able to make a clean exit from a program once we encounter a error
 * during runtime.
 * Also for convenience because fuck 42's subjects anyway.
 */
void
lenv_init(void)
{
	extern char	**environ;
	int size = arr_size(environ);
	if (!(g_env = malloc(sizeof (char *) * (size + 1))))
		exit(1);
	int i = 0;
	while (i < size)
	{
		g_env[i] = ft_strdup(environ[i]);
		i++;
	}
	g_env[size] = NULL;
#ifdef DEBUG
	fprintf(stderr, "DEBUG: lenv_init(): [0] = \"%s\" [%d] = \"%s\" [%d] = \"%s\"\n",
			g_env[0], size - 1, g_env[size - 1], size, g_env[size]);
#endif /* !DEBUG */
}

/**
 * Destroy the allocated environment table.
 */
void
lenv_deinit(void)
{
	char **env = g_env;
	while (*env)
	{
		free(*env);
		env++;
	}
	free(g_env);
#ifdef DEBUG
	fprintf(stderr, "DEBUG: lenv_deinit(): OK\n");
#endif /* !DEBUG */
}
