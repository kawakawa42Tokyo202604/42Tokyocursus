*This project has been created as part of the 42 curriculum by \<takawaka\>.*

# get_next_line


## 概要
この課題では、ファイルディスクリプタから1行ずつ読み取る `get_next_line` を実装します。呼び出すたびに1行分の文字列を返し、次の呼び出しで使う残りのデータは `static` 変数に保持します。

### 読み込み方法
1回の `read` で読み込むサイズは、`BUFFER_SIZE` で指定します。

### 実装の流れ
`is_newline` を使って読み込んだデータに改行が含まれているかを確認します。改行が見つからない場合は、改行またはEOFに達するまで読み込みを続けます。1行分を返した後、残りのデータは `static` 変数に保存し、次の呼び出しで使います。

## 使用方法

読み取り対象のファイルディスクリプタを `get_next_line` に渡します。

### コンパイル
以下にMakefileの例と、ライブラリの作成に使うコマンドを示します。
```bash
make        # .aを生成
make clean  # .oを削除
make fclean # 生成ファイルをすべて削除
make re     # fclean の後に make
```
#### Makefileの例
```make
NAME    = get_next_line.a
CC      = cc
CFLAGS  = -Wall -Wextra -Werror

SRCS    = get_next_line.c get_next_line_utils.c
OBJS    = $(SRCS:.c=.o)

B_SRCS  = get_next_line_bonus.c get_next_line_utils_bonus.c
B_OBJS  = $(B_SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	ar rcs $(NAME) $(OBJS)

bonus: $(B_OBJS)
	ar rcs $(NAME) $(B_OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(B_OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re bonus
```
#### main
```c
#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>

int	main(int argc, char **argv)
{
	int		fd;
	char	*line;
	int		line_count;

	if (argc < 2)
	{
		printf("Usage: %s <file_path>\n", argv[1]);
		return (1);
	}

	// 1. ファイルの読み込みテスト
	fd = open(argv[1], O_RDONLY);
	if (fd == -1)
	{
		perror("Error opening file");
		return (1);
	}

	printf("--- Reading file: %s ---\n", argv[1]);
	line_count = 1;
	while ((line = get_next_line(fd)) != NULL)
	{
		printf("[%02d]: %s", line_count++, line);
		free(line);
	}
	close(fd);

	// 2. 標準入力のテスト (必要であればコメントアウトを外してください)
	/*
	printf("\n--- Reading from Standard Input (Ctrl+D to stop) ---\n");
	while ((line = get_next_line(0)) != NULL)
	{
		printf("STDIN: %s", line);
		free(line);
	}
	*/

	return (0);
}
```
#### main bonus
```c
#include "get_next_line_bonus.h"
#include <fcntl.h>
#include <stdio.h>


int	main(void)
{
	int		fd1;
	int		fd2;
	char	*line;
	int		i;

	fd1 = open("file1.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	fd2 = open("file2.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	write(fd1, "F1-Line 1\nF1-Line 2\nF1-Line 3\n", 30);
	write(fd2, "F2-Line A\nF2-Line B\nF2-Line C\n", 30);
	close(fd1);
	close(fd2);

	fd1 = open("file1.txt", O_RDONLY);
	fd2 = open("file2.txt", O_RDONLY);

	printf("--- Multiple FD Test Starting ---\n\n");

	i = 0;
	while (i < 3)
	{
		line = get_next_line(fd1);
		printf("FD %d: %s", fd1, line);
		free(line);

		line = get_next_line(fd2);
		printf("FD %d: %s", fd2, line);
		free(line);

		i++;
	}

	close(fd1);
	close(fd2);
	printf("\n--- Test Complete ---\n");
	return (0);
}
```
#### text1
```
Hello, 42!
This is a test for get_next_line.
The BUFFER_SIZE will determine how many bytes are read at once.
Final line without a newline... (maybe)
```
#### text2
```
[File 2] Line A: Hello
[File 2] Line B: This is the second file
[File 2] Line C: Bonus part test
[File 2] Line D: Goodbye!
```

## AIの利用
`static` 変数や `read`、`open` の使い方の理解、サンプルの `main` とREADMEの作成・見直しにAIを利用しました。
