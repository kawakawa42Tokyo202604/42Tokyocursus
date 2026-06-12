#include "libft.h"

t_list	*ft_lstnew(void *content)
{
	t_list	*res;

	res = (t_list *)malloc(sizeof(t_list) * 1);
	if (!res)
		return (NULL);
	res->content = content;
	res->next = NULL;
	return (res);
}
