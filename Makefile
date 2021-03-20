all:
	gcc -o nsh.exe main.c
sanitize:
	cppcheck --enable=warning,style,performance,portability --inconclusive --std=posix --std=c99  *.c
	gcc -o nsh.exe main.c -fsanitize=address