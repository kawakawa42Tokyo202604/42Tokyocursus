#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H

# include <stdlib.h>
# include <unistd.h>
# include <limits.h>
# include <stdio.h>

# ifndef END
#  define END '\n'
# endif

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif

# ifndef OPEN_MAX
#  define OPEN_MAX 1024
# endif

typedef struct s_stack
{
	int				value;
	int				index;
	struct s_stack	*next;
	struct s_stack	*prev;
}					t_stack;

typedef enum e_strategy
{
	SIMPLE,
	MEDIUM,
	COMPLEX,
	ADAPTIVE
}	t_strategy;

typedef struct s_stats
{
	int			sa;
	int			sb;
	int			ss;
	int			pa;
	int			pb;
	int			ra;
	int			rb;
	int			rr;
	int			rra;
	int			rrb;
	int			rrr;
	int			total;
}				t_stats;

typedef struct s_config
{
	t_strategy	strategy;
	int			bench;
	int			start_idx;
	t_stats		stats;
	int			*array;
	int			*tmp;
}				t_config;

/**push_swap.c**/
int		is_sorted(t_stack *stack);
int		parse_and_fill_array(int argc, char **argv, t_config *config, int size);
int		fill_one_arg(char *arg, t_config *config, int *k);
int		is_flag(char *arg);
/**ft_libft**/
size_t	ft_strlen(const char *str);
size_t	ft_strlcpy(char *dst, const char *src, size_t size);
char	*ft_strdup(const char *s);
void	ft_bzero(void *s, size_t n);
void	*ft_memset(void *s, int c, size_t n);
long	ft_atoi(const char *nptr);
char	**ft_split(char const *s);
int		count_total_elements(int argc, char **argv, int start_idx);
char	*ft_substr(char const *s, unsigned int start, size_t len);
int		ft_strcmp(const char *s1, const char *s2);
int		count_words(char const *s);
/**gnl**/
char	*get_next_line(int fd);
char	*get_line_from_save(char *save);
char	*update_save(char *save);
char	*ft_strchr(const char *s, int c);
char	*ft_strjoin(char *s1, const char *s2);
char	*ft_free(char *s);
/**ft_lst.c**/
t_stack	*ft_lstnew(int val, int i);
void	ft_lstadd_back(t_stack **lst, t_stack *new);
t_stack	*ft_lstlast(t_stack *lst);
void	free_stack(t_stack **stack);
/**ft_swap.c**/
void	ft_swap(t_stack **stack);
void	sa(t_stack **stack_a, t_config *config);
void	sb(t_stack **stack_b, t_config *config);
void	ss(t_stack **stack_a, t_stack **stack_b, t_config *config);
/**ft_push.c**/
void	ft_push(t_stack **dst, t_stack **src);
int		pa(t_stack **stack_a, t_stack **stack_b, t_config *config);
int		pb(t_stack **stack_a, t_stack **stack_b, t_config *config);
/**ft_reverse_rotate.c**/
void	ft_reverse_rotate(t_stack **stack);
void	rra(t_stack **stack_a, t_config *config);
void	rrb(t_stack **stack_b, t_config *config);
void	rrr(t_stack **stack_a, t_stack **stack_b, t_config *config);
/**ft_rotate.c**/
void	ft_rotate(t_stack **stack);
void	ra(t_stack **stack_a, t_config *config);
void	rb(t_stack **stack_b, t_config *config);
void	rr(t_stack **stack_a, t_stack **stack_b, t_config *config);
/**ft_strategy_main.c**/
void	excute_strategy(t_stack **stack_a, t_config *config, int size);
/**ft_strategy_simple**/
void	sort_three(t_stack **a, t_config *config);
void	sort_simple(t_stack **a, t_stack **b, int size, t_config *config);
/**ft_strategy_medium**/
void	sort_medium(t_stack **a, t_stack **b, int size, t_config *config);
/**ft_strategy_complex.c**/
void	sort_complex(t_stack **a, t_stack **b, int size, t_config *config);
/**parsing.c**/
int		parse_flags(int argc, char **argv, t_config *config);
int		check_dup(int *array, int size);
/**stack_init**/
void	init_stack(t_stack **stack_a, int *array, int *tmp, int size);
int		find_index(int val, int *sorted_array, int size);
void	sort_array(int *tmp, int size);
/**compute_disorder.c**/
double	compute_disorder(t_stack *stack, int size);
/**ft_dprintf.c**/
int		ft_dprintf(int fd, const char *format, ...);
/**handler**/
int		handle_c(va_list args, int fd);
int		handle_s(va_list args, int fd);
int		handle_p(va_list args, int fd);
int		handle_di(va_list args, int fd);
int		handle_u(va_list args, int fd);
int		handle_x(va_list args, int fd);
int		handle_big_x(va_list args, int fd);
/**base**/
int		ft_putnbr_base(unsigned long n, char *base, int fd);

#endif
