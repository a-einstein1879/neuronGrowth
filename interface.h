﻿#ifndef INTERFACE_H
#define INTERFACE_H

#include <windows.h>
#include <glut.h>

#include "hippocampus.h"

class OpenGLInterface {
private:
	static OpenGLInterface *p_OpenGLInterface;

	HGLRC	hRC;
	HDC		hDC;
	HWND	hWnd;
	HINSTANCE hInstance;
	
	bool  keys[256];
	bool  active;
	bool  fullscreen;
	
	/* Window creation */
	int InitGL(GLvoid);
	BOOL CreateGLWindow(LPCWSTR title, int width, int height, int bits, bool fullscreenflag);
	GLvoid ReSizeGLScene(GLsizei width, GLsizei height);

	LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK InitialWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	/* End of window creation */

	/* Picture creation */
	int picture[NUMBEROFCELLSX][NUMBEROFCELLSY];

	Hippocampus* hippocampus;
	void printPicture();
	void drawPixel(int x, int y, int type);
	void getHippocampusCoordinates();

	double environmentField[NUMBEROFCELLSX][NUMBEROFCELLSY][NUMBEROFNEURONTYPES];
	void printEnvironment();
	/* End of picture creation */
public:
	OpenGLInterface();
	static OpenGLInterface* getOpenGLInterface();
	GLvoid KillGLWindow(GLvoid);

	void tick();
};

#endif INTERFACE_H