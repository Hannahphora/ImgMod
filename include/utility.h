#pragma once

#define ESC 27
#define ENTER 13
#define ARROW_UP 72
#define ARROW_DOWN 80

#define FN_DEF(func, wait) { func, #func, wait },

struct State {
	const char* inPath;
	const char* outPath;
	int w{}, h{}, comp{};
	uint8_t* img{};
	State(const char* i, const char* o) : inPath(i), outPath(o) {}
};

class Menu {
public:

	struct Opts {
		void (*fnptr)(State&);
		const char* name;
		const bool shouldWait;
	};

	Menu(Opts(&menuOpts)[], size_t menuLen, State& state);
	~Menu();

	void Run();

private:

	State& m_state;
	Opts(&m_arr)[];
	size_t m_len;

};

void CursorVis(bool);