#include "interface.h"

// Include tchar to define _T
#include <tchar.h>

OpenGLInterface* OpenGLInterface::p_OpenGLInterface = 0;

OpenGLInterface::OpenGLInterface() {
	hRC  = NULL;
	hDC  = NULL;
	hWnd = NULL;

	active = true;
	fullscreen = false;
	
	//CreateGLWindow(L"OpenGL window", 800, 600, 32, fullscreen);
	/* X scale is multiplied by 2 to draw the chart in the right side of the picture */
	CreateGLWindow(L"OpenGL window", NUMBEROFCELLSX * PICTURESCALEX, NUMBEROFCELLSY * PICTURESCALEY, 32, fullscreen);

	hippocampus = hippocampus->getHippocampus();
	environment = environment->getEnvironment();
	output = output->getOutput();
	for(int i = 0; i < NUMBEROFCELLSX; i++)
		for(int j = 0; j < NUMBEROFCELLSY; j++)
			picture[i][j] = NOTHING;
}

OpenGLInterface* OpenGLInterface::getOpenGLInterface() {
	if(!p_OpenGLInterface) {
		p_OpenGLInterface = new OpenGLInterface();
	}
	return p_OpenGLInterface;
}

int OpenGLInterface::InitGL(GLvoid) {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	return true;
}

BOOL OpenGLInterface::CreateGLWindow( LPCWSTR title, int width, int height, int bits, bool fullscreenflag ) {
	GLuint    PixelFormat;
	WNDCLASS  wc;
	DWORD    dwExStyle;
	DWORD    dwStyle;
	
	RECT WindowRect;                // Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;              // ���������� ����� ������������ � 0
	WindowRect.right=(long)width;              // ���������� ������ ������������ � Width
	WindowRect.top=(long)0;                // ���������� ������� ������������ � 0
	WindowRect.bottom=(long)height;              // ���������� ������ ������������ � Height

	fullscreen = fullscreenflag;              // ������������� �������� ���������� ���������� fullscreen
	hInstance    = GetModuleHandle(NULL);        // ������� ���������� ������ ����������
	wc.style    = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;      // ���������� ��� ����������� � ������ ������� DC
	wc.lpfnWndProc    = &OpenGLInterface::InitialWndProc;          // ��������� ��������� ���������
	wc.cbClsExtra    = 0;              // ��� �������������� ���������� ��� ����
	wc.cbWndExtra    = 0;              // ��� �������������� ���������� ��� ����
	wc.hInstance    = hInstance;            // ������������� ����������
	wc.hIcon    = LoadIcon(NULL, IDI_WINLOGO);        // ��������� ������ �� ���������
	wc.hCursor    = LoadCursor(NULL, IDC_ARROW);        // ��������� ��������� �����
	wc.hbrBackground  = NULL;              // ��� �� ��������� ��� GL
	wc.lpszMenuName    = NULL;              // ���� � ���� �� �����
	wc.lpszClassName  = L"OpenGL";            // ������������� ��� ������

	if( !RegisterClass( &wc ) ) {
    MessageBox( NULL, L"Failed To Register The Window Class.", L"ERROR", MB_OK | MB_ICONEXCLAMATION );
    return false;                // ����� � ����������� �������� �������� false
	}

	if( fullscreen ) {
		DEVMODE dmScreenSettings;            // ����� ����������
		memset( &dmScreenSettings, 0, sizeof( dmScreenSettings ) );    // ������� ��� �������� ���������
		dmScreenSettings.dmSize=sizeof( dmScreenSettings );      // ������ ��������� Devmode
		dmScreenSettings.dmPelsWidth  =   width;        // ������ ������
		dmScreenSettings.dmPelsHeight  =   height;        // ������ ������
		dmScreenSettings.dmBitsPerPel  =   bits;        // ������� �����
		dmScreenSettings.dmFields= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;// ����� �������
		// �������� ���������� ��������� ����� � �������� ���������.  ����������: CDS_FULLSCREEN ������� ������ ����������.
		if( ChangeDisplaySettings( &dmScreenSettings, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL ) {
			if( MessageBox( NULL, L"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?", L"NeHe GL", MB_YESNO | MB_ICONEXCLAMATION) == IDYES ) {
				fullscreen = false;
			} else {
				MessageBox( NULL, L"Program Will Now Close.", L"ERROR", MB_OK | MB_ICONSTOP );
				return false;
			}
		}
	}

	if(fullscreen) {
		dwExStyle  =   WS_EX_APPWINDOW;          // ����������� ����� ����
		dwStyle    =   WS_POPUP;            // ������� ����� ����
		ShowCursor( true );
	} else {
		dwExStyle  =   WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;      // ����������� ����� ����
		dwStyle    =   WS_OVERLAPPEDWINDOW;        // ������� ����� ����
	}
	AdjustWindowRectEx( &WindowRect, dwStyle, false, dwExStyle );

	if( !( hWnd = CreateWindowEx(  dwExStyle,          // ����������� ����� ��� ����
          _T("OpenGL"),          // ��� ������
          title,            // ��������� ����
          WS_CLIPSIBLINGS |        // ��������� ����� ��� ����
          WS_CLIPCHILDREN |        // ��������� ����� ��� ����
          dwStyle,          // ���������� ����� ��� ����
          0, 0,            // ������� ����
          WindowRect.right-WindowRect.left,    // ���������� ���������� ������
          WindowRect.bottom-WindowRect.top,    // ���������� ���������� ������
          NULL,            // ��� �������������
          NULL,            // ��� ����
          hInstance,          // ���������� ����������
          this ) ) ) {          // �� ������� ������ �� WM_CREATE (???)
			  KillGLWindow();                // ������������ �����
			  MessageBox( NULL, L"Window Creation Error.", L"ERROR", MB_OK | MB_ICONEXCLAMATION );
			  return false;                // ������� false
	}

	static  PIXELFORMATDESCRIPTOR pfd=            // pfd �������� Windows ����� ����� ����� �� ����� ������� �������
  {
    sizeof(PIXELFORMATDESCRIPTOR),            // ������ ����������� ������� ������� ��������
    1,                  // ����� ������
    PFD_DRAW_TO_WINDOW |              // ������ ��� ����
    PFD_SUPPORT_OPENGL |              // ������ ��� OpenGL
    PFD_DOUBLEBUFFER,              // ������ ��� �������� ������
    PFD_TYPE_RGBA,                // ��������� RGBA ������
    bits,                  // ���������� ��� ������� �����
    0, 0, 0, 0, 0, 0,              // ������������� �������� �����
    0,                  // ��� ������ ������������
    0,                  // ��������� ��� ������������
    0,                  // ��� ������ ����������
    0, 0, 0, 0,                // ���� ���������� ������������
    32,                  // 32 ������ Z-����� (����� �������)
    0,                  // ��� ������ ���������
    0,                  // ��� ��������������� �������
    PFD_MAIN_PLANE,                // ������� ���� ���������
    0,                  // ���������������
    0, 0, 0                  // ����� ���� ������������
  };

	if( !( hDC = GetDC( hWnd ) ) ) {             // ����� �� �� �������� �������� ����������?
		KillGLWindow();                // ������������ �����
		MessageBox( NULL, L"Can't Create A GL Device Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;                // ������� false
	}
	
	if( !( PixelFormat = ChoosePixelFormat( hDC, &pfd ) ) )        // ������ �� ���������� ������ �������?
  {
    KillGLWindow();                // ������������ �����
    MessageBox( NULL, L"Can't Find A Suitable PixelFormat.", L"ERROR", MB_OK | MB_ICONEXCLAMATION );
    return false;                // ������� false
  }
	
	if( !SetPixelFormat( hDC, PixelFormat, &pfd ) )          // �������� �� ���������� ������ �������?
  {
    KillGLWindow();                // ������������ �����
    MessageBox( NULL, L"Can't Set The PixelFormat.", L"ERROR", MB_OK | MB_ICONEXCLAMATION );
    return false;                // ������� false
  }
	
	if( !( hRC = wglCreateContext( hDC ) ) )          // �������� �� ���������� �������� ����������?
  {
    KillGLWindow();                // ������������ �����
    MessageBox( NULL, L"Can't Create A GL Rendering Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
    return false;                // ������� false
  }

	if( !wglMakeCurrent( hDC, hRC ) )            // ����������� ������������ �������� ����������
  {
    KillGLWindow();                // ������������ �����
    MessageBox( NULL, L"Can't Activate The GL Rendering Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION );
    return false;                // ������� false
  }

	ShowWindow( hWnd, SW_SHOW );              // �������� ����
	SetForegroundWindow( hWnd );              // ������ ������� ���������
	SetFocus( hWnd );                // ���������� ����� ���������� �� ���� ����
	ReSizeGLScene( width, height );              // �������� ����������� ��� ������ OpenGL ������.

	if( !InitGL() )                  // ������������� ������ ��� ���������� ����
  {
    KillGLWindow();                // ������������ �����
    MessageBox( NULL, _T("Initialization Failed."), _T("ERROR"), MB_OK | MB_ICONEXCLAMATION );
    return false;                // ������� false
  }

	return true;
}

GLvoid OpenGLInterface::KillGLWindow( GLvoid ) {
	if( fullscreen ) {
		ChangeDisplaySettings( NULL, 0 );
		ShowCursor( true );
	}
	if( hRC ) {
		if( !wglMakeCurrent( NULL, NULL ) ) {
			MessageBox( NULL, L"Release Of DC And RC Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION );
		}
		if( !wglDeleteContext( hRC ) ) {
			MessageBox( NULL, L"Release Rendering Context Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION );
		}
		hRC = NULL;
	}
	if( hDC && !ReleaseDC( hWnd, hDC ) ) {
		MessageBox( NULL, L"Release Device Context Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION );
		hDC = NULL;
	}
	if(hWnd && !DestroyWindow(hWnd)) {
		MessageBox( NULL, L"Could Not Release hWnd.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION );
		hWnd = NULL;
	}
	if( !UnregisterClass( L"OpenGL", hInstance ) ) {
		MessageBox( NULL, L"Could Not Unregister Class.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hInstance = NULL; 
	}
}

GLvoid OpenGLInterface::ReSizeGLScene(GLsizei width, GLsizei height) {
	if( height == 0 ) {
		height = 1;
	}
	glViewport(0, 0, width, height);
	/*glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective( 45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f );

    glMatrixMode( GL_MODELVIEW );*/
    glLoadIdentity();
}

LRESULT OpenGLInterface::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_ACTIVATE:
		{
			if( !HIWORD( wParam ) ){
				active = true;
			} else {
				active = false;
			}
			return 0;
		}
	case WM_SYSCOMMAND:
		{
			switch ( wParam ) {
			case SC_SCREENSAVE:
			case SC_MONITORPOWER:
			return 0;
			}
			break;
		}
	case WM_CLOSE:
		{
			PostQuitMessage( 0 );
			return 0;
		}
	case WM_KEYDOWN:
		{
			keys[wParam] = true;
			return 0;
		}
	case WM_KEYUP:
		{
			keys[wParam] = false;
			return 0;
		}
	case WM_SIZE:
		{
			ReSizeGLScene( LOWORD(lParam), HIWORD(lParam) );
			return 0;
		}
	}
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}


LRESULT CALLBACK OpenGLInterface::InitialWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
      if (Msg == WM_NCCREATE) {
        LPCREATESTRUCT create_struct = reinterpret_cast<LPCREATESTRUCT>(lParam);
        void *lpCreateParam = create_struct->lpCreateParams;
        OpenGLInterface *this_window = reinterpret_cast<OpenGLInterface *>(lpCreateParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this_window));
        SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&OpenGLInterface::StaticWndProc));
        return this_window->WndProc(hWnd, Msg, wParam, lParam);
      }
      return DefWindowProc(hWnd, Msg, wParam, lParam);
}

LRESULT CALLBACK OpenGLInterface::StaticWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
      LONG_PTR user_data = GetWindowLongPtr(hWnd, GWLP_USERDATA);
      OpenGLInterface *this_window = reinterpret_cast<OpenGLInterface *>(user_data);
      return this_window->WndProc(hWnd, Msg, wParam, lParam);
}

void OpenGLInterface::getCells() {
	for(int i = 0; i < NUMBEROFCELLSX; i++)
		for(int j = 0; j < NUMBEROFCELLSY; j++)
			picture[i][j] = hippocampus->getFieldType(i, j);

	for(int x = 0; x < NUMBEROFCELLSX; x++)
		for(int y = 0; y < NUMBEROFCELLSY; y++)
			for(int type = 0; type < NUMBEROFNEURONTYPES; type++)
				environmentField[x][y][type] = environment->getField(x, y, type);
}

void OpenGLInterface::printPicture() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);

	FigureRectangle rectangle(-1, -1, 0, 1);
	drawNeuronPicture(rectangle);

	rectangle.setFigure(0, -1, 1, 1);
	drawPotentialLineChart(rectangle);

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	SwapBuffers(hDC);

#ifdef STEPBYSTEP
	system("pause");
#endif
}

void OpenGLInterface::drawNeuronPicture(FigureRectangle rectangle) {
	getCells();
	for(int j = 0; j < NUMBEROFCELLSY; j++)
		for(int i = 0; i < NUMBEROFCELLSX; i++)
			drawPixel(rectangle, i, j, picture[i][j]);
	
	#ifdef DIFFUSIONVISIBLE
		for(int type = 0; type < NUMBEROFNEURONTYPES; type++)
			for(int j = 0; j < NUMBEROFCELLSY; j++)
				for(int i = 0; i < NUMBEROFCELLSX; i++)
					drawPixel(rectangle, i, j, ENVIRONMENT, type, environmentField[i][j][type]);
	#endif

	std::string environment;
	for(int type = 0; type < NUMBEROFNEURONTYPES; type++) {
		environment = "\n" + std::to_string(type);
		for(int y = 0; y < NUMBEROFCELLSY; y++) {
			for(int x = 0; x < NUMBEROFCELLSX; x++)
				environment += std::to_string(environmentField[x][y][type]) + " ";
			environment += "\n";
		}
		environment += "\n";
	}
	PRINTSTATISTICS(ENVIRONMENTSTATISTICSFILEID, environment);
}

void OpenGLInterface::drawPixel(FigureRectangle rectangle, int x, int y, int type, int environmentType, double intensity) {
    glLoadIdentity();
	switch(type) {
	case NOTHING:
		glColor3f(0, 0, 0);
		return;
		break;
	case NEURON:
		glColor3f(1, 1, 1);
		break;
	case AXON:
		glColor3f(0, 1, 0);
		break;
	case DENDRITE:
		glColor3f(1, 0, 0);
		break;
	case ENVIRONMENT:
		switch(environmentType) {
		case 0:
			glColor3f(0, 0, ENVIRONMENTINTENSITYMULTIPLIER * intensity);
			break;
		case 1:
			glColor3f(0, ENVIRONMENTINTENSITYMULTIPLIER * intensity, 0.6 * ENVIRONMENTINTENSITYMULTIPLIER * intensity);
			break;
		}
		break;
	default:
		glColor3f(0, 0, 0);
	}
	double startX = rectangle.getMiddleX() - rectangle.getSizeX() / 2;
	double startY = rectangle.getMiddleY() - rectangle.getSizeY() / 2;
	double scaleX = rectangle.getSizeX();
	double scaleY = rectangle.getSizeY();
	double X = double(x) / NUMBEROFCELLSX;
	double Y = double(y) / NUMBEROFCELLSY;
	glTranslatef(startX + X * scaleX, startY + Y * scaleY, 0.0);
	double halfSizeX = 1.0 / NUMBEROFCELLSX / 2 * scaleX;
	double halfSizeY = 1.0 / NUMBEROFCELLSY / 2 * scaleY;
	glBegin(GL_QUADS);
		glVertex2f(-halfSizeX, halfSizeY);
		glVertex2f(halfSizeX,  halfSizeY);
		glVertex2f(halfSizeX, -halfSizeY);
		glVertex2f(-halfSizeX,-halfSizeY);
	glEnd();
}

void OpenGLInterface::drawPotentialLineChart(FigureRectangle rectangle) {
	LineChart lineChart(MAXNUMBEROFNEURONS, 0, WORKTIME);
	hippocampus->feelPotentialsChart(lineChart);
	drawLineChart(lineChart, rectangle);
}

void OpenGLInterface::drawLineChart(LineChart &lineChart, FigureRectangle rectangle) {
	/* Put coursor back to (0, 0, 0) */
    glLoadIdentity();

	drawArgumentLabels(lineChart.getMinArgument(), lineChart.getMaxActiveArgument(), NUMBEROFXVALUELABELS, rectangle);
	drawValueLabels(lineChart.getMinValue(), lineChart.getMaxValue(), NUMBEROFYVALUELABELS, rectangle);
	rectangle.resize(0.9, 0.93);

	double startX = rectangle.getMiddleX() - rectangle.getSizeX() / 2;
	double startY = rectangle.getMiddleY() - rectangle.getSizeY() / 2;
	double scaleY = rectangle.getSizeY();
	double maxValue = lineChart.getMaxValue();
	double minValue = lineChart.getMinValue();

	for(int j = 0; j < lineChart.getNumberOfCharts(); j++) {
		int numberOfArguments = lineChart.getMaxActiveArgument(j);
		double widthStep = rectangle.getSizeX() / numberOfArguments;
		glLineWidth(1);
		Color color = lineChart.getColor(j);
		glColor3f(color.getRed(), color.getGreen(), color.getBlue());
		for(int i = 1; i < numberOfArguments; i++) {
			glBegin(GL_LINES);
				glVertex3f(startX + (i - 1) * widthStep,
				startY + ( lineChart.getValue(j, i - 1) - minValue ) / ( maxValue - minValue ) * scaleY, 0);
				glVertex3f(startX + i       * widthStep,
				startY + ( lineChart.getValue(j, i)     - minValue ) / ( maxValue - minValue ) * scaleY, 0);
			glEnd();
		}
	}
}

void OpenGLInterface::drawText(const char *text, int length, float x, float y) {
	glColor3f(1, 1, 1);
	glLoadIdentity();
	glRasterPos2f(x, y);
	for(int i = 0; i < length; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, (int)text[i]);
	}
}

void OpenGLInterface::drawArgumentLabels(double minArgument, double maxArgument, int numberOfBins, FigureRectangle rectangle) {
	double startX = rectangle.getMiddleX() - rectangle.getSizeX() / 2;
	double deltaX = rectangle.getSizeX() / double(numberOfBins);
	double y = rectangle.getMiddleY() - rectangle.getSizeY() / 2;
	double delta = (maxArgument - minArgument) / double(numberOfBins);
	for(int i = 0; i < numberOfBins; i++) {
		char bufer[10];
		_itoa_s(minArgument + delta * i, bufer, 10, 10);
		drawText(bufer, sizeof(bufer) / sizeof(char), startX + deltaX * i, y);
	}
}

void OpenGLInterface::drawValueLabels(double minValue, double maxValue, int numberOfBins, FigureRectangle rectangle) {
	double startY = rectangle.getMiddleY() - rectangle.getSizeY() / 2;
	double deltaY = rectangle.getSizeY() / double(numberOfBins);
	double x = rectangle.getMiddleX() - rectangle.getSizeX() / 2;
	double delta = (maxValue - minValue) / double(numberOfBins);
	for(int i = 0; i < numberOfBins; i++) {
		char bufer[10];
		int sign = minValue + delta * i;
		_itoa_s(sign, bufer, 10, 10);
		drawText(bufer, sizeof(bufer) / sizeof(char), x + 0.02, startY + deltaY * i);
	}
}

void OpenGLInterface::tick() {
	printPicture();
}