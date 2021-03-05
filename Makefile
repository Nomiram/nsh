all:
	gcc -o nsh.exe main.c
sanitize:
	gcc -o nsh.exe main.c -fsanitize=address
