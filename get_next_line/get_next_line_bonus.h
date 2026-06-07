#ifndef GET_NEXT_LINE_BONUS_H
# define GET_NEXT_LINE_BONUS_H

# include <stdlib.h>
# include <unistd.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif

char	*get_next_line(int fd);
char	*ft_strjointrim(char **save);
char	*ft_strjoinfree(char *buf, char *save);
int		is_error(int fd, char *buf);
char	*all_free(char *b, char *s);
ssize_t	is_newline(char *buf);
char	*ft_strdupgnl(char *s);
ssize_t	ft_strlengnl(char *s);
char	*ft_cleangnl(char *s);
int		ft_if_null_free(char **s);

#endif
