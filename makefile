all:
	g++ -Ofast -fomit-frame-pointer main.cpp -o monza -DUSE_SSE41 -msse4.1 -DUSE_SSSE3 -mssse3 -DUSE_SSE2 -msse2 -DUSE_SSE -msse
	# x86_64-w64-mingw32-gcc -Ofast -fomit-frame-pointer main.cpp -o monza.exe -DUSE_SSE41 -msse4.1 -DUSE_SSSE3 -mssse3 -DUSE_SSE2 -msse2 -DUSE_SSE -msse

debug:
	g++ -Ofast main.cpp -o monza
	# x86_64-w64-mingw32-gcc main.cpp -o monza.exe
