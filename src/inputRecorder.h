#pragma once

#include <list>
#include "main.h"

struct recorderFrame {
	unsigned int frame;

	recorderFrame(int frame): frame(frame) {}
	recorderFrame(std::istream &in) { in >> frame; }
	virtual void save(std::ostream &out) {}
	virtual void execute() {}
};

struct mouseMoveFrame: public recorderFrame {
	int x, y;

	mouseMoveFrame(unsigned int frame, int x, int y)
		: recorderFrame(frame), x(x), y(y) {}
	mouseMoveFrame(std::istream &in): recorderFrame(in) { in >> x >> y; }
	void save(std::ostream &out) { out << 'M' << ' ' << frame << ' ' << x << ' ' << y << ' '; }
	void execute() { mouseMove(x, y); }
};

struct mouseClickFrame: public recorderFrame {
	int button; bool up;
	mouseClickFrame(unsigned int frame, int button, bool up)
		: recorderFrame(frame), button(button), up(up) {}
	mouseClickFrame(std::istream &in): recorderFrame(in) { in >> button >> up; }
	void save(std::ostream &out) { out << 'C' << ' ' << frame << ' ' << button << ' ' << up << ' '; }
	void execute() {
		if(up) mouseClick(button, GLUT_UP, 0, 0);
		else mouseClick(button, GLUT_DOWN, 0, 0);
	}
};

struct keyboardFrame: public recorderFrame {
	unsigned char ch; bool up;
	keyboardFrame(unsigned int frame, unsigned char ch, bool up)
		: recorderFrame(frame), ch(ch), up(up) {}
	keyboardFrame(std::istream &in): recorderFrame(in) { int i; in >> i >> up; ch = (unsigned char)i; }
	void save(std::ostream &out) { out << 'K' << ' ' << frame << ' ' << (int)ch << ' ' << up << ' '; }
	void execute() {
		if(up) keyboardUp(ch, 0, 0);
		else keyboard(ch, 0, 0);
	}
};


class inputRecorder {
	std::list<recorderFrame*> frames;
	bool recording, running;
	std::list<recorderFrame*>::iterator iter;
public:
	inputRecorder() { recording = running = false; }

	~inputRecorder() {
		clear();
	}

	void clear() {
		for(std::list<recorderFrame*>::iterator i = frames.begin(); i != frames.end(); ++i)
			delete *i;
		frames.clear();
	}

	void start() { running = false; recording = true; clear(); }
	void stop() { recording = false; running = false; }
	void run() { if(frames.size() == 0) return; running = true; iter = frames.begin(); }
	bool isRecording() { return recording; }

	void mouseMove(unsigned int frame, int x, int y) {
		if(recording)
		frames.push_back(new mouseMoveFrame(frame, x, y));
	}

	void mouseClick(unsigned int frame, int button, bool up) {
		if(recording)
		frames.push_back(new mouseClickFrame(frame, button, up));
	}

	void keyboard(unsigned int frame, char ch, bool up) {
		if(recording)
		frames.push_back(new keyboardFrame(frame, ch, up));
	}

	void reset() { iter = frames.begin(); }

	void runFrame(unsigned int frame) {
		if(!running) return;
		while( iter != frames.end() && (*iter)->frame <= frame ) {
			(*iter)->execute(); ++iter;
		}
		if(iter == frames.end()) stop();
	}

	void save(std::ostream &out) {
		for(std::list<recorderFrame*>::iterator i = frames.begin(); i != frames.end(); ++i)
			(*i)->save(out);
	}

	void load(std::istream &in) {
		recorderFrame *f;
		char ch;
		while(!in.eof() && in.get(ch)) {		
			f = NULL;
			switch(ch) {
				case 'K': f = new keyboardFrame(in); break;
				case 'C': f = new mouseClickFrame(in); break;
				case 'M': f = new mouseMoveFrame(in); break;
			}
			if(f) frames.push_back(f);
		}
	}
};
