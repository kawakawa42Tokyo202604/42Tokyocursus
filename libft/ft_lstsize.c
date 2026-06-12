#include "libft.h"

int	ft_lstsize(t_list *lst)
{
	int	len;

	if (!lst)
		return (0);
	len = 0;
	while (lst != NULL)
	{
		lst = lst->next;
		++len;
	}
	return (len);
}
