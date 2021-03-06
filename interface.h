﻿#ifndef INTERFACE_H
#define INTERFACE_H

#include <windows.h>
#include <glut.h>

#include "hippocampus.h"
#include "environment.h"
#include "output.h"
#include "chart.h"
#include "interfaceClasses.h"
#include "synaps.h"

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
	double potentialPicture[NUMBEROFCELLSX][NUMBEROFCELLSY];
	double minimumPotential;
	double maximumPotential;
	double environmentField[NUMBEROFCELLSX][NUMBEROFCELLSY][NUMBEROFNEURONTYPES];
	int numberOfSynapses;
	Synaps *synapses;
	void getData();

	Hippocampus* hippocampus;
	Environment* environment;
	Configurator *configurator;
	Output *output;
	void printPicture();
	void drawNeuronPicture(FigureRectangle rectangle);
	void drawPotentialLineChart(FigureRectangle rectangle);
	void printConnections(FigureRectangle rectangle);
	
	void drawPixel(FigureRectangle rectangle, int x, int y, int type, double intensity = -1, int environmentType = -1);
	void drawText(const char *text, int length, float x, float y);
	void drawArgumentLabels(double minArgument, double maxArgument, int numberOfBins, FigureRectangle rectangle);
	void drawValueLabels(double minValue, double maxValue, int numberOfBins, FigureRectangle rectangle);
	void drawLineChart(LineChart &lineChart, FigureRectangle rectangle, int chartIdx = -1);
	/* End of picture creation */
public:
	OpenGLInterface();
	static OpenGLInterface* getOpenGLInterface();
	GLvoid KillGLWindow(GLvoid);

	void tick();
};

#endif INTERFACE_H