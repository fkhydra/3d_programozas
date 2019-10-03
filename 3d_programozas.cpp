#include <math.h>
#include <stdio.h>
#include <windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#pragma comment(lib, "d2d1")
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1000

struct VEKTOR {
	float x;
	float y;
	float z;
};
VEKTOR vLight;
D2D1_TRIANGLE triangle;

#define MAX_OBJ_NUM 20000000
int viewpoint = -1100;
float rot_degree_x;
float rot_degree_y;
float rot_degree_z;
float rot_degree_x2 = 0;
float rot_degree_y2 = 90.0f;
float rot_degree_z2 = 0;
float Math_PI = 3.14159265358979323846;
float raw_verticesX[MAX_OBJ_NUM], raw_verticesY[MAX_OBJ_NUM], raw_verticesZ[MAX_OBJ_NUM];
float rotated_verticesX[MAX_OBJ_NUM], rotated_verticesY[MAX_OBJ_NUM], rotated_verticesZ[MAX_OBJ_NUM];
int raw_vertices_length;

int zorder_length;
int zorder_index[MAX_OBJ_NUM];
float zorder_distance[MAX_OBJ_NUM];

void shell_sorting(void);
void init_3D(void);
void render_scene(void);
void rotation(int maxitemcount, float* rawarrayX, float* rawarrayY, float* rawarrayZ, float* rotarrayX, float* rotarrayY, float* rotarrayZ, float degree_cosx, float degree_sinx, float degree_cosy, float degree_siny, float degree_cosz, float degree_sinz);
void D2D_drawing(int maxitemcount, float* rotarrayX, float* rotarrayY, float* rotarrayZ);
void zoom_in(int maxitemcount, float* rawarrayX, float* rawarrayY, float* rawarrayZ);
void zoom_out(int maxitemcount, float* rawarrayX, float* rawarrayY, float* rawarrayZ);
//************************************

//***********STANDARD WIN32API************
ID2D1Factory* pD2DFactory = NULL;
ID2D1HwndRenderTarget* pRT = NULL;
#define HIBA_00 TEXT("Error:Program initialisation process.")
HINSTANCE hInstGlob;
int SajatiCmdShow;
HWND Form1; //Ablak kezeloje
LRESULT CALLBACK WndProc0(HWND, UINT, WPARAM, LPARAM);
//******************************************************

//********************************
//OBJ formatum kezelesehez
//********************************
float tomb_vertices[MAX_OBJ_NUM][3];
int tomb_faces[MAX_OBJ_NUM][5];
int tomb_vertices_length = 0, tomb_faces_length = 0;
int getelementcount(unsigned char csv_content[]);
void getelement(unsigned char csv_content[], unsigned int data_index, unsigned char csv_content2[]);
void obj_loader(void);

//*********************************
//Foprogram belepesi pont
//*********************************
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("StdWinClassName");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass0;
	SajatiCmdShow = iCmdShow;
	hInstGlob = hInstance;

	//*********************************
	//Ablak elokeszitese
	//*********************************
	wndclass0.style = CS_HREDRAW | CS_VREDRAW;
	wndclass0.lpfnWndProc = WndProc0;
	wndclass0.cbClsExtra = 0;
	wndclass0.cbWndExtra = 0;
	wndclass0.hInstance = hInstance;
	wndclass0.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass0.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass0.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wndclass0.lpszMenuName = NULL;
	wndclass0.lpszClassName = TEXT("WIN0");

	//*********************************
	//Ablak regisztrralasa
	//*********************************
	if (!RegisterClass(&wndclass0))
	{
		MessageBox(NULL, HIBA_00, TEXT("Program Start"), MB_ICONERROR);
		return 0;
	}

	//*********************************
	//Ablak letrehozasaCreating the window
	//*********************************
	Form1 = CreateWindow(TEXT("WIN0"),
		TEXT("CUDA - DIRECT2D"),
		(WS_OVERLAPPED | WS_SYSMENU | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX),
		0,
		0,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	//*********************************
	//Ablak megjelenitese
	//*********************************
	ShowWindow(Form1, SajatiCmdShow);
	UpdateWindow(Form1);

	//*********************************
	//Ablak mukodtetese
	//*********************************
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

//*********************************
//Uzenetkezeles
//*********************************
LRESULT CALLBACK WndProc0(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	unsigned int xPos, yPos, fwButtons;

	switch (message)
	{
		//*********************************
		//Ablak letrehozasa
		//*********************************
	case WM_CREATE:
		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);
		pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(
				hwnd, D2D1::SizeU(SCREEN_WIDTH, SCREEN_HEIGHT)),
			&pRT);
		init_3D();
		obj_loader();
		if ((joyGetNumDevs()) > 0) joySetCapture(hwnd, JOYSTICKID1, NULL, FALSE);
		return 0;
		//*********************************
		//Villodzas ellen
		//*********************************
	case WM_ERASEBKGND:
		return (LRESULT)1;
	case MM_JOY1MOVE:
		fwButtons = wParam;
		xPos = LOWORD(lParam);
		yPos = HIWORD(lParam);
		if (xPos == 65535) {
			rot_degree_y2 += 5.0; render_scene();
		}
		else if (xPos == 0) {
			rot_degree_y2 -= 5.0; render_scene();
		}
		if (yPos == 65535) {
			rot_degree_x2 += 5.0; render_scene();
		}
		else if (yPos == 0) {
			rot_degree_x2 -= 5.0; render_scene();
		}
		if (fwButtons == 128) {
			rot_degree_z2 += 5.0; render_scene();
		}
		else if (fwButtons == 64) {
			rot_degree_z2 -= 5.0; render_scene();
		}
		if (rot_degree_y2 > 360) {
			rot_degree_y2 = 0; render_scene();
		}
		else if (rot_degree_y2 < 0) {
			rot_degree_y2 = 358; render_scene();
		}
		if (rot_degree_x2 > 359) {
			rot_degree_x2 = 0; render_scene();
		}
		else if (rot_degree_x2 < 0) {
			rot_degree_x2 = 358; render_scene();
		}
		if (rot_degree_z2 > 359) {
			rot_degree_z2 = 0; render_scene();
		}
		else if (rot_degree_z2 < 0) {
			rot_degree_z2 = 358; render_scene();
		}

		if (fwButtons == 2)
		{
			zoom_in(raw_vertices_length, raw_verticesX, raw_verticesY, raw_verticesZ);
			render_scene();
		}
		else if (fwButtons == 4)
		{
			zoom_out(raw_vertices_length, raw_verticesX, raw_verticesY, raw_verticesZ);
			render_scene();
		}
		else if (fwButtons == 1)
		{
			viewpoint += 100;
			render_scene();
		}
		else if (fwButtons == 8)
		{
			viewpoint -= 100;
			render_scene();
		}
		break;
		//*********************************
		//Ablak ujrarajzolasa
		//*********************************
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		render_scene();
		return 0;
		//*********************************
		//Ablak bezarasa
		//*********************************
	case WM_CLOSE:
		pRT->Release();
		pD2DFactory->Release();
		DestroyWindow(hwnd);
		return 0;
		//*********************************
		//Ablak megsemmisitese
		//*********************************
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

//********************************
//PEGAZUS 3D - INIT
//********************************
void init_3D(void)
{
	rot_degree_x = 0 * Math_PI / 180; rot_degree_x2 = 0;
	rot_degree_y = 0 * Math_PI / 180; rot_degree_y2 = 0;
	rot_degree_z = 0 * Math_PI / 180; rot_degree_z2 = 0;
	vLight.x = -0.5; vLight.y = 1.0; vLight.z = -0.9;
	raw_vertices_length = 0;
}

//********************************
//OBJ formatum kezelese
//********************************
int getelementcount(unsigned char csv_content[])
{
	int s1, s2;
	for (s1 = s2 = 0; s1 < strlen((const char*)csv_content); ++s1)
	{
		if (csv_content[s1] == 10) break;
		else if (csv_content[s1] == 32) ++s2;
	}
	return s2;
}

void getelement(unsigned char csv_content[], unsigned int data_index, unsigned char csv_content2[])
{
	int s1, s2, s3, s4 = 0;
	for (s1 = 0, s2 = 0; s1 < strlen((const char*)csv_content); ++s1)
	{
		if (csv_content[s1] == 32)
		{
			++s2;
			if (s2 == data_index)
			{
				for (s3 = s1 + 1; s3 < strlen((const char*)csv_content); ++s3)
				{
					if (csv_content[s3] == 32 || csv_content[s3] == 10)
					{
						csv_content2[s4] = 0;
						return;
					}
					else csv_content2[s4++] = csv_content[s3];
				}
			}
		}
	}
}

void obj_loader(void)
{
	FILE* objfile;
	int i, j;
	float data1, data2, data3;
	unsigned char row1[1024], row2[1024];
	int data_count, max_row_length = 250;
	char tempstr[200];

	objfile = fopen("yamaha.obj", "rt");
	if (objfile == NULL) return;

	tomb_vertices_length = tomb_vertices_length = 0;

	while (!feof(objfile))
	{
		fgets((char*)row1, max_row_length, objfile);

		if (row1[0] == 118 && row1[1] == 32) //*** 'v '
		{
			getelement(row1, 1, row2); data1 = atof((const char*)row2);
			getelement(row1, 2, row2); data2 = atof((const char*)row2);
			getelement(row1, 3, row2); data3 = atof((const char*)row2);
			tomb_vertices[tomb_vertices_length][0] = data1 * 4;
			tomb_vertices[tomb_vertices_length][1] = data2 * 4;
			tomb_vertices[tomb_vertices_length++][2] = data3 * 4;
		}
		else if (row1[0] == 102 && row1[1] == 32) //*** 'f '
		{
			data_count = getelementcount(row1);

			tomb_faces[tomb_faces_length][0] = data_count;
			for (i = 1; i < data_count + 1; ++i)
			{
				getelement(row1, i, row2);
				data1 = atof((const char*)row2);
				tomb_faces[tomb_faces_length][i] = data1 - 1;
			}
			++tomb_faces_length;
		}
	}
	fclose(objfile);
	int  base_index;
	for (i = 0; i < tomb_faces_length; ++i)
	{
		base_index = tomb_faces[i][1];
		if (tomb_faces[i][0] == 3)
		{
			raw_verticesX[raw_vertices_length] = tomb_vertices[tomb_faces[i][1]][0];
			raw_verticesY[raw_vertices_length] = tomb_vertices[tomb_faces[i][1]][1];
			raw_verticesZ[raw_vertices_length++] = tomb_vertices[tomb_faces[i][1]][2];

			raw_verticesX[raw_vertices_length] = tomb_vertices[tomb_faces[i][2]][0];
			raw_verticesY[raw_vertices_length] = tomb_vertices[tomb_faces[i][2]][1];
			raw_verticesZ[raw_vertices_length++] = tomb_vertices[tomb_faces[i][2]][2];

			raw_verticesX[raw_vertices_length] = tomb_vertices[tomb_faces[i][3]][0];
			raw_verticesY[raw_vertices_length] = tomb_vertices[tomb_faces[i][3]][1];
			raw_verticesZ[raw_vertices_length++] = tomb_vertices[tomb_faces[i][3]][2];
		}
		else if (tomb_faces[i][0] == 4)
		{
			raw_verticesX[raw_vertices_length] = tomb_vertices[tomb_faces[i][1]][0];
			raw_verticesY[raw_vertices_length] = tomb_vertices[tomb_faces[i][1]][1];
			raw_verticesZ[raw_vertices_length++] = tomb_vertices[tomb_faces[i][1]][2];

			raw_verticesX[raw_vertices_length] = tomb_vertices[tomb_faces[i][2]][0];
			raw_verticesY[raw_vertices_length] = tomb_vertices[tomb_faces[i][2]][1];
			raw_verticesZ[raw_vertices_length++] = tomb_vertices[tomb_faces[i][2]][2];

			raw_verticesX[raw_vertices_length] = tomb_vertices[tomb_faces[i][3]][0];
			raw_verticesY[raw_vertices_length] = tomb_vertices[tomb_faces[i][3]][1];
			raw_verticesZ[raw_vertices_length++] = tomb_vertices[tomb_faces[i][3]][2];

			raw_verticesX[raw_vertices_length] = tomb_vertices[tomb_faces[i][1]][0];
			raw_verticesY[raw_vertices_length] = tomb_vertices[tomb_faces[i][1]][1];
			raw_verticesZ[raw_vertices_length++] = tomb_vertices[tomb_faces[i][1]][2];

			raw_verticesX[raw_vertices_length] = tomb_vertices[tomb_faces[i][3]][0];
			raw_verticesY[raw_vertices_length] = tomb_vertices[tomb_faces[i][3]][1];
			raw_verticesZ[raw_vertices_length++] = tomb_vertices[tomb_faces[i][3]][2];

			raw_verticesX[raw_vertices_length] = tomb_vertices[tomb_faces[i][4]][0];
			raw_verticesY[raw_vertices_length] = tomb_vertices[tomb_faces[i][4]][1];
			raw_verticesZ[raw_vertices_length++] = tomb_vertices[tomb_faces[i][4]][2];
		}
	}
}

void render_scene(void)
{
	char tempstr[255], tempstr2[255];
	float fps_stat;
	ULONGLONG starttime, endtime;

	strcpy(tempstr2, "Vertices: ");
	_itoa(raw_vertices_length, tempstr, 10);
	strcat(tempstr2, tempstr);
	strcat(tempstr2, " Triangles: ");
	_itoa(raw_vertices_length/3, tempstr, 10);
	strcat(tempstr2, tempstr);
	strcat(tempstr2, " Z ordered: ");
	starttime = GetTickCount64();

	rot_degree_x = rot_degree_x2 * Math_PI / 180;
	rot_degree_y = rot_degree_y2 * Math_PI / 180;
	rot_degree_z = rot_degree_z2 * Math_PI / 180;
	float degree_sinx = sin(rot_degree_x);
	float degree_cosx = cos(rot_degree_x);
	float degree_siny = sin(rot_degree_y);
	float degree_cosy = cos(rot_degree_y);
	float degree_sinz = sin(rot_degree_z);
	float degree_cosz = cos(rot_degree_z);

	rotation(raw_vertices_length, raw_verticesX, raw_verticesY, raw_verticesZ, rotated_verticesX, rotated_verticesY, rotated_verticesZ, degree_cosx, degree_sinx, degree_cosy, degree_siny, degree_cosz, degree_sinz);
	shell_sorting();
	D2D_drawing(raw_vertices_length, rotated_verticesX, rotated_verticesY, rotated_verticesZ);

	endtime = GetTickCount64();
	if ((endtime - starttime) == 0) ++endtime;
	fps_stat = 1000 / (endtime - starttime); strcat(tempstr2, " FPS: "); _itoa(fps_stat, tempstr, 10); strcat(tempstr2, tempstr);
	strcat(tempstr2, ", X: "); _itoa(rot_degree_x2, tempstr, 10); strcat(tempstr2, tempstr);
	strcat(tempstr2, ", Y: "); _itoa(rot_degree_y2, tempstr, 10); strcat(tempstr2, tempstr);
	strcat(tempstr2, ", Z: "); _itoa(rot_degree_z2, tempstr, 10); strcat(tempstr2, tempstr);
	SetWindowTextA(Form1, tempstr2);
}

void rotation(int maxitemcount, float* rawarrayX, float* rawarrayY, float* rawarrayZ, float* rotarrayX, float* rotarrayY, float* rotarrayZ, float degree_cosx, float degree_sinx, float degree_cosy, float degree_siny, float degree_cosz, float degree_sinz)
{
	int i;
	float t0;

	//rotaion
	for (i = 0; i < maxitemcount; ++i)
	{
		rotarrayY[i] = (rawarrayY[i] * degree_cosx) - (rawarrayZ[i] * degree_sinx);
		rotarrayZ[i] = rawarrayY[i] * degree_sinx + rawarrayZ[i] * degree_cosx;

		rotarrayX[i] = rawarrayX[i] * degree_cosy + rotarrayZ[i] * degree_siny;
		rotarrayZ[i] = -rawarrayX[i] * degree_siny + rotarrayZ[i] * degree_cosy;// +

		t0 = rotarrayX[i];
		//Nemi finomhangolas OBJ modellek eseten: "+ (SCREEN_WIDTH / 4)" and "+ (SCREEN_HEIGHT / 4)"
		rotarrayX[i] = t0 * degree_cosz - rotarrayY[i] * degree_sinz + (SCREEN_WIDTH / 4);
		rotarrayY[i] = t0 * degree_sinz + rotarrayY[i] * degree_cosz + (SCREEN_HEIGHT / 4);
	}

	//persspektiva projekcio
	int s1;
	float sx = SCREEN_WIDTH / 2;
	float sultra = SCREEN_HEIGHT / 2, sultra2 = SCREEN_HEIGHT / 3;
	int x_minusz_edge = 0, y_minusz_edge = 0, x_max_edge = SCREEN_WIDTH - 1, y_max_edge = SCREEN_HEIGHT - 1;
	float distance;

	zorder_length = 0;

	for (i = 0; i < maxitemcount; i+=3)
	{
		distance = 999999;

		for (s1 = i; s1 < i + 3; ++s1)
		{
			if (rotarrayZ[s1] < distance) distance = rotarrayZ[s1];
			if (distance < viewpoint) { rotarrayZ[s1] = -9999999; continue; }
			sultra = viewpoint / (viewpoint - rotarrayZ[s1]);
			rotarrayX[s1] = rotarrayX[s1] * sultra + 400;
			rotarrayY[s1] = (rotarrayY[s1] * sultra) + sultra2;
			if (rotarrayX[s1] < x_minusz_edge || rotarrayX[s1] > x_max_edge) { rotarrayZ[s1] = -9999999; continue; }
			if (rotarrayY[s1] < y_minusz_edge || rotarrayY[s1] > y_max_edge) { rotarrayZ[s1] = -9999999; continue; }
		}

		zorder_index[zorder_length] = i;
		zorder_distance[zorder_length++] = distance;
	}
}

void D2D_drawing(int maxitemcount, float* rotarrayX, float* rotarrayY, float* rotarrayZ)
{
	int i, px, py, drawcolor;
	VEKTOR Vector1, Vector2, vNormal, vNormalized;//for visibility check
	float Light_intensity, Vector_length;

	pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	pRT->BeginDraw();
	pRT->Clear(D2D1::ColorF(D2D1::ColorF::White));
	for (i = zorder_length; i >= 0 ; --i)
	{
		if ((rotarrayZ[zorder_index[i]] < -9000000) || (rotarrayZ[zorder_index[i] + 1] < -9000000) || (rotarrayZ[zorder_index[i] + 2] < -9000000)) continue;

		// lathatosagi vizsgalat
		Vector1.x = rotarrayX[zorder_index[i] + 1] - rotarrayX[zorder_index[i]];
		Vector1.y = rotarrayY[zorder_index[i] + 1] - rotarrayY[zorder_index[i]];
		Vector1.z = rotarrayZ[zorder_index[i] + 1] - rotarrayZ[zorder_index[i]];
		Vector2.x = rotarrayX[zorder_index[i] + 2] - rotarrayX[zorder_index[i]];
		Vector2.y = rotarrayY[zorder_index[i] + 2] - rotarrayY[zorder_index[i]];
		Vector2.z = rotarrayZ[zorder_index[i] + 2] - rotarrayZ[zorder_index[i]];

		vNormal.x = ((Vector1.y * Vector2.z) - (Vector1.z * Vector2.y));
		vNormal.y = ((Vector1.z * Vector2.x) - (Vector1.x * Vector2.z));
		vNormal.z = ((Vector1.x * Vector2.y) - (Vector1.y * Vector2.x));
		if (vNormal.z > 0) continue;
		//*/

		Vector_length = sqrtf((vNormal.x * vNormal.x) + (vNormal.y * vNormal.y) + (vNormal.z * vNormal.z));
		vNormalized.x = vNormal.x / Vector_length;
		vNormalized.y = vNormal.y / Vector_length;
		vNormalized.z = vNormal.z / Vector_length;
		Light_intensity = ((vNormalized.x * vLight.x) + (vNormalized.y * vLight.y) + (vNormalized.z * vLight.z));
		if (Light_intensity > 1) Light_intensity = 1;
		else if (Light_intensity < 0) Light_intensity = 0;

		//drawcolor = RGB(180 * ((float)i / (float)maxitemcount * 100), 180 * ((float)i / (float)maxitemcount * 100), 180 * ((float)i / (float)maxitemcount * 100));
		drawcolor = RGB(205* Light_intensity,205* Light_intensity,205* Light_intensity);
		triangle.point1 = D2D1::Point2F(rotarrayX[zorder_index[i]], rotarrayY[zorder_index[i]]);
		triangle.point2 = D2D1::Point2F(rotarrayX[zorder_index[i] +1], rotarrayY[zorder_index[i] +1]);
		triangle.point3 = D2D1::Point2F(rotarrayX[zorder_index[i] +2], rotarrayY[zorder_index[i] +2]);
		ID2D1SolidColorBrush* ecset;
		pRT->CreateSolidColorBrush(D2D1::ColorF(drawcolor, 1.0f),&ecset);
				
		//kitoltott haromszogek
		ID2D1Mesh* pMesh = NULL;
		ID2D1TessellationSink* tessSink = NULL;
		pRT->CreateMesh(&pMesh);
		pMesh->Open(&tessSink);
		tessSink->AddTriangles(&triangle, 1);
		tessSink->Close();
		pRT->FillMesh(pMesh, ecset);
		tessSink->Release();
		pMesh->Release();//*/

		//csak drotvazmodell
		/*pRT->DrawLine(
			D2D1::Point2F(rotarrayX[zorder_index[i]], rotarrayY[zorder_index[i]]),
			D2D1::Point2F(rotarrayX[zorder_index[i] + 1], rotarrayY[zorder_index[i] + 1]),
			ecset,
			1.0f);
		pRT->DrawLine(
			D2D1::Point2F(rotarrayX[zorder_index[i]+2], rotarrayY[zorder_index[i]+2]),
			D2D1::Point2F(rotarrayX[zorder_index[i] + 1], rotarrayY[zorder_index[i] + 1]),
			ecset,
			1.0f);
		pRT->DrawLine(
			D2D1::Point2F(rotarrayX[zorder_index[i]], rotarrayY[zorder_index[i]]),
			D2D1::Point2F(rotarrayX[zorder_index[i] + 2], rotarrayY[zorder_index[i] + 2]),
			ecset,
			1.0f);//*/
	}
	pRT->EndDraw();
}

void zoom_in(int maxitemcount, float* rawarrayX, float* rawarrayY, float* rawarrayZ)
{
	int i;
	for (i = 0; i < maxitemcount; ++i)
	{
		rawarrayX[i] *= 1.2;
		rawarrayY[i] *= 1.2;
		rawarrayZ[i] *= 1.2;
	}
}

void zoom_out(int maxitemcount, float* rawarrayX, float* rawarrayY, float* rawarrayZ)
{
	int i;
	for (i = 0; i < maxitemcount; ++i)
	{
		rawarrayX[i] /= 1.2;
		rawarrayY[i] /= 1.2;
		rawarrayZ[i] /= 1.2;
	}
}

void shell_sorting(void)
{
	int i,j,k;
	int swap0;
	float swap2;

	for (k = zorder_length / 2; k > 0; k = k / 2)
		for (i = k; i < zorder_length; ++i)
			for (j = i - k; (j >= 0) && (zorder_distance[j] > zorder_distance[j + k]); j = j - k)
			{
				swap0 = zorder_index[j];
				swap2 = zorder_distance[j];
				zorder_index[j] = zorder_index[j + k];
				zorder_distance[j] = zorder_distance[j + k];
				zorder_index[j + k] = swap0;
				zorder_distance[j + k] = swap2;
			}
}