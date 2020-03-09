#ifndef __ENV_H__
#define __ENV_H__

#define ENV extern char **environ

ENV;

char *lenv_getenv(const char *);

int lenv_setenv(const char *, const char *, int);
int lenv_unsetenv(const char *);

#endif /* !__ENV_H__ */
