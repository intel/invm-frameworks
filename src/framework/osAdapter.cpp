/*
 * Copyright (c) 2015 2016, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Intel Corporation nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef __WINDOWS__
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <dlfcn.h>
#include <termios.h>
#include <unistd.h>
#endif

#include "osAdapter.h"
#include <iostream>
#include <string.h>

/*
 * Use OS specific libraries to find console size
 */
enum osResult getWindowSize(struct WindowSize &windowSize)
{
	enum osResult rc = OS_FAILED;
	memset(&windowSize, 0, sizeof (struct WindowSize));

#ifdef __WINDOWS__
	CONSOLE_SCREEN_BUFFER_INFO info;
	if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info) != 0)
	{
		windowSize.width = info.srWindow.Right - info.srWindow.Left;
		windowSize.height = info.srWindow.Bottom - info.srWindow.Top;
		rc = OS_SUCCESS;
	}
#else
	struct winsize consoleWidth;
	if (ioctl(0, TIOCGWINSZ , &consoleWidth) >= 0)
	{
		windowSize.width = consoleWidth.ws_col;
		windowSize.height = consoleWidth.ws_row;
		rc = OS_SUCCESS;
	}
#endif

return rc;
}


/*
 * add library extension
 */
char *dlib_suffix(char *buffer, size_t buffer_len)
{
#ifdef __WINDOWS__
	strncpy(buffer, ".dll", (buffer_len < 5 ? buffer_len : 5));
 #else
	strncpy(buffer, ".so", (buffer_len < 4 ? buffer_len : 4));
#endif
	return buffer;
}

/*
 * dynamically load a library
 */
void *dlib_load(const char *lib_path)
{
#ifdef __WINDOWS__
	return LoadLibraryExA(lib_path, 0, 0);
 #else
	return dlopen(lib_path, RTLD_LAZY);
#endif
	return NULL;
}

/*
 * Get the symbol name from a loaded library
 */
void *dlib_find_symbol(void *handle, const char *symbol)
{
#ifdef __WINDOWS__
	return (void*) GetProcAddress((HMODULE) handle, symbol);
#else
	return dlsym(handle, symbol);
#endif
}

/*
 * Turn screen echoing and string buffering off, read user's input, showing asterisks
 * instead of characters for security purposes
 */
#ifdef __WINDOWS__
void os_readUserHiddenString(std::string *p_strResponse)
{
	DWORD  mode, oldMode;
	HANDLE hConIn = GetStdHandle( STD_INPUT_HANDLE );
	GetConsoleMode( hConIn, &mode );
	oldMode = mode;
	mode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);
	SetConsoleMode( hConIn, mode );

	char buffer[1];
	DWORD read;
	ReadConsole(hConIn, buffer, sizeof(buffer), &read, NULL);
	while ((buffer[0] != '\n') && (buffer[0] != 13))
	{
		p_strResponse->push_back(buffer[0]);
		std::cout << '*';
		ReadConsole(hConIn, buffer, sizeof(buffer), &read, NULL);
	}

	mode = oldMode;
	std::cout << std::endl;
	SetConsoleMode( hConIn, mode );
}
#else
void os_readUserHiddenString(std::string *p_strResponse)
{
	struct termios settings, oldSettings;
	tcgetattr( STDIN_FILENO, &settings );
	oldSettings = settings;
	settings.c_lflag &= ~(ICANON | ECHO);
	settings.c_cc[VMIN] = 1;
	tcsetattr( STDIN_FILENO, TCSANOW, &settings );

	char ch = 0;
	while (((ch = std::cin.get()) != '\n') && (ch != 13))
	{
		p_strResponse->push_back(ch);
		std::cout << '*';
	}
	std::cout << std::endl;
	tcsetattr( STDIN_FILENO, TCSANOW, &oldSettings );
}
#endif
