#include <windows.h>
#include <cstdlib>
#include <stdio.h>
#include <conio.h>
#include <cstring>
#include <iostream>
#include "../include/utility.h"

Menu::Menu(Opts(&menuOpts)[], size_t menuLen, State& state)
	: m_arr(menuOpts), m_len(menuLen), m_state(state) {}

Menu::~Menu() {
	delete[] m_state.img;
}

void Menu::Run() {

	CursorVis(false);
	bool running = true;
	int cursor = 0;

	while (running) {

		printf("|| ImgMod Menu\n\n");
		printf("Input path:  %s\nOutput path: %s\n\n", m_state.inPath, m_state.outPath);

		for (int i = 0; i < m_len; i++) {
			printf(" %c %2d) %s\n", (i == cursor ? '>' : ' '), i + 1, m_arr[i].name);
		}

		switch ((char)_getch()) {
		case 'w':
		case ARROW_UP:
			cursor = cursor == 0 ? m_len - 1 : cursor - 1;
			break;
		case 's':
		case ARROW_DOWN:
			cursor = cursor == m_len - 1 ? 0 : cursor + 1;
			break;
		case ENTER:
			system("cls");
			if (m_arr[cursor].name == "Exit") return;
			(*m_arr[cursor].fnptr)(m_state);
			if (m_arr[cursor].shouldWait) system("pause");
			break;
		case ESC:
			running = false;
			break;
		default:
			break;
		}
		system("cls");
	}
}

void CursorVis(bool vis) {
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO lpCursor;
	lpCursor.bVisible = vis;
	lpCursor.dwSize = 20;
	SetConsoleCursorInfo(console, &lpCursor);
}