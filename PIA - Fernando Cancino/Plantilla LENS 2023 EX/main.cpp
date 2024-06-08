#include <windows.h>
#include <stdlib.h> 
#include <vector>
#include <iostream> //Libreria para entrada y salida de datos
#include <fstream> //Libreria para manejar archivos
#include <string> //Libreria para utilizar tipos de datos string
#include "Librerias/Dibujar bitmaps/gdipload.h"
#include "Librerias/Musica/libzplay.h"

#define Tick 100
#define Timer 100

using namespace libZPlay;//Se declara que se usara la libreria de libZPlay para reproductor de musica
using namespace std;

struct Musica
{
	string Dir;
	string Nombre;
};
Musica Cancion[3];

struct FrameArray {
	int x, y;
	int ancho, alto;
};//Similar a la estructura rect de winapi

struct Input
{
	//Genera una deteccion de todas las teclas del teclado
	enum Keys
	{
		Backspace = 0x08, Tab,
		Clear = 0x0C, Enter,
		Shift = 0x10, Control, Alt,
		Escape = 0x1B,
		Space = 0x20, PageUp, PageDown, End, Home, Left, Up, Right, Down,
		Zero = 0x30, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,
		A = 0x41, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		NumPad0 = 0x60, NumPad1, NumPad2, NumPad3, NumPad4, NumPad5, NumPad6, NumPad7, NumPad8, NumPad9,
		F1 = 0x70, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12
	};
}input;
bool KEYS[256];

enum Frames {
	Frame0,
	Frame1,
	Frame2,
	Frame3,
	Frame4,
	Frame5,
	Frame6,
	Frame7,
	Frame8,
	Frame9,
	Frame10,
	Frame11,
	Frame12,
	Frame13,
	Frame14,
	Frame15,
	Frame16,
	Frame17,
	Frame18,
	Frame19,
	Frame20
};

//Genera una deteccion de las animaciones
enum Animaciones
{
	Right,
	Left,
	Down,
	Up
};
enum Animaciones2
{
	Vertical,
	Horizontal,
	Cuerpo
};
enum Animaciones3
{
	Idle
};

//Aquí van la pantalla de inicio, el first stage y las pantallas de winner y looser
enum Stages
{
	Inicio,
	FirstStage,
	Winner,
	Looser
};

struct DatosPersonaje1
{
	//Ruta donde esta nuestra imagen del personaje
	WCHAR BmpW[MAX_PATH] = L"Recursos/snake_cabeza.bmp";
	miBitmap HojaSprite;

	//Posicion relativa de nuestro Sprite en la ventana
	int XCurrentCoordDraw;
	int YCurrentCoordDraw;

	//Dimensiones en pixeles que se requieren para dibujarlo en la ventana
	int HeightDraw = 20;
	int WeightDraw = 20;

	int Animaciones = 4; //3 Animaciones

	int FramesAnimacionLeft = 1;
	int FramesAnimacionRight = 1;
	int FramesAnimacionUp = 1;
	int FramesAnimacionDown = 1;

	FrameArray** FrameSpriteArray;
} miPersonaje1;

struct DatosPersonaje2
{
	//Ruta donde esta nuestra imagen del personaje
	WCHAR BmpW[MAX_PATH] = L"Recursos/snake_cuerpo.bmp";
	miBitmap HojaSprite;

	//Posicion relativa de nuestro Sprite en la ventana
	int XCurrentCoordDraw;
	int YCurrentCoordDraw;

	//Dimensiones en pixeles que se requieren para dibujarlo en la ventana
	int HeightDraw = 20;
	int WeightDraw = 20;

	int Animaciones = 3; //3 Animaciones
	int AnimacionActual;

	int FramesAnimacionVertical = 1;
	int FramesAnimacionHorizontal = 1;
	int FramesAnimacionCuerpo = 1;


	FrameArray** FrameSpriteArray;
};
vector<DatosPersonaje2> cuerpo = {};

struct DatosPersonaje3
{
	//Ruta donde esta nuestra imagen del personaje
	WCHAR BmpW[MAX_PATH] = L"Recursos/snake_cola.bmp";
	miBitmap HojaSprite;

	//Posicion relativa de nuestro Sprite en la ventana
	int XCurrentCoordDraw;
	int YCurrentCoordDraw;

	//Dimensiones en pixeles que se requieren para dibujarlo en la ventana
	int HeightDraw = 20;
	int WeightDraw = 20;

	int Animaciones = 4; //3 Animaciones

	int FramesAnimacionLeft = 1;
	int FramesAnimacionRight = 1;
	int FramesAnimacionUp = 1;
	int FramesAnimacionDown = 1;


	FrameArray** FrameSpriteArray;
}miPersonaje3;

struct DatosEnemigo
{
	//Ruta donde esta nuestra imagen del personaje
	WCHAR BmpW[MAX_PATH] = L"Recursos/alien.bmp";
	miBitmap HojaSprite;
	int tipo = -1; // 0 Alien | 1 Lava | 2 Obstaculo

	//Posicion relativa de nuestro Sprite en la ventana
	int XCurrentCoordDraw;
	int YCurrentCoordDraw;

	//Dimensiones en pixeles que se requieren para dibujarlo en la ventana
	int HeightDraw = 20;
	int WeightDraw = 20;

	int Animaciones = 1; //3 Animaciones

	int FramesAnimacionIdle = 1;

	FrameArray** FrameSpriteArray;
}miEnemigo1, miEnemigo2, miLava1, miLava2, miLava3, miObstaculo1, miObstaculo2, miObstaculo3;

//Doneee
struct DatosStage
{
	int Escenarios = 4;
	miBitmap ImagenEscenario1;
	WCHAR Bmp1[MAX_PATH] = L"Recursos/fondo.bmp";

	miBitmap ImagenEscenario2;
	WCHAR Bmp2[MAX_PATH] = L"Recursos/stage2.bmp";

	miBitmap ImagenEscenario3;
	WCHAR Bmp3[MAX_PATH] = L"Recursos/winner.bmp"; //Aquí irá winner

	miBitmap ImagenEscenario4;
	WCHAR Bmp4[MAX_PATH] = L"Recursos/looser.bmp"; //Aquí irá looser
}miStage;

//Variables Globales
const int ANCHO_VENTANA = 1000;
const int ALTO_VENTANA = 600;
const int BPP = 4;
const unsigned int TRANSPARENCY = 0xFF800020; //Color del Mapa
ZPlay* player = CreateZPlay(); //Generamos un objeto puntero para nuestro reproductor
TStreamStatus status;
bool pausa = false;
int* ptrBufferPixelsWindow;
int FrameActual = 0;
int DelayFrameAnimation = 0;

int aliensComidos = 0;
int alienAgrandar = 2;
int vidas = 3;

int AnimacionActual1;
int AnimacionActual3;

//Pantallas agregadas
bool pantallaInicial = true;
bool pantallaJuego = false;
bool pantallaWinner = false;
bool pantallaLooser = false;

const float sinlimite = 0;
const float fps1 = 1000 / 1;
const float fps24 = 1000 / 24;
const float fps30 = 1000 / 30;
const float fps45 = 1000 / 45;
const float fps60 = 1000 / 60;
const float fps120 = 1000 / 120;
float fps = fps30;
float timer = 0.0f;

//Definicion de funciones Wianpi
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void LimpiarFondo(int* ptrBuffer, unsigned int color, int area);
void TranScaleblt(int* punteroDestino, int* punteroOrigen, int inicioXDestino, int inicioYDestino, int inicioXOrigen, int inicioYOrigen, int ancho, int alto, int anchodefondo, int anchodeorigen, int escalaX, int escalaY);
void TranScaleblt
(int* punteroDestino, int* punteroOrigen, int inicioXDestino, int inicioYDestino, int inicioXOrigen, int inicioYOrigen, int ancho, int alto, int anchodefondo, int anchodeorigen, int escalaX, int escalaY, const unsigned int TRANSPARENCY, double limiteX);
void MainRender(HWND hWnd);
void Init();
void KeysEvents();
void ReproductorPausa();
void ReproductorReproduce();
void ReproductorInicializaYReproduce();
void ReproductorCambiarCancionYReproduce(int);
void CargaFramesSprite();
void ActualizarCuerpo();
void CambiarPosicion(DatosEnemigo&, bool = true);
void ColisionAlien(DatosEnemigo&);
bool ColisionObjeto(DatosEnemigo&);
void AgrandarCuerpo();
void MoverCuerpo();

void Escribir(HWND hwnd, HDC hdc);
void Escribir2(HWND hwnd, HDC hdc);
void Escribir3(HWND hwnd, HDC hdc);
void Escribir4(HWND hwnd, HDC hdc);
int randomInt(int min, int max) { return min + rand() % ((max + 1) - min); }


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;									// Windows Class Structure
	HWND hWnd;
	MSG msg;

	TCHAR szAppName[] = TEXT("MyWinAPIApp");
	TCHAR szAppTitle[] = TEXT("Plantilla Lenguaje Ensamblador (cambiar nombre aqui)");

	hInstance = GetModuleHandle(NULL);				// Grab An Instance For Our Window

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc = (WNDPROC)WndProc;					// WndProc Handles Messages
	wc.cbClsExtra = 0;									// No Extra Window Data
	wc.cbWndExtra = 0;									// No Extra Window Data
	wc.hInstance = hInstance;							// Set The Instance	
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);			// No Background Required For GL
	wc.lpszMenuName = NULL;									// We Don't Want A Menu	
	wc.lpszClassName = szAppName;							// Set The Class Name
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,
			L"Fallo al registrar clase (Failed To Register The Window Class).",
			L"ERROR",
			MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	hWnd = CreateWindowEx(
		WS_EX_CLIENTEDGE | WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,	// Extended Style For The Window
		szAppName,							// Class Name
		szAppTitle,							// Window Title
		WS_OVERLAPPEDWINDOW |				// Defined Window Style
		WS_CLIPSIBLINGS |					// Required Window Style
		WS_CLIPCHILDREN,					// Required Window Style
		0, 0,								// Window Position
		ANCHO_VENTANA,						// Calculate Window Width
		ALTO_VENTANA,						// Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		hInstance,							// Instance
		NULL);								// Pass this class To WM_CREATE								

	if (hWnd == NULL) {
		MessageBox(NULL,
			L"Error al crear ventana (Window Creation Error).",
			L"ERROR",
			MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	Init();
	ShowWindow(hWnd, nCmdShow);
	SetFocus(hWnd);

	SetTimer(hWnd, Timer, Tick, NULL);
	ZeroMemory(&msg, sizeof(MSG));

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return(int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)									// Check For Windows Messages
	{
	case WM_CREATE:
		break;
	case WM_KEYDOWN:
	{
		KEYS[wParam] = true;
	}
	break;

	//Aquí se moldea que hacen las teclas del juego
	case WM_KEYUP:
	{
		KEYS[wParam] = false;
		switch (wParam) {
		case VK_F1://F1
		{
			ReproductorPausa();
		}
		return 0;
		case VK_F2://F2
		{
			ReproductorCambiarCancionYReproduce(0);
		}
		return 0;
		case VK_F3://F3
		{
			ReproductorCambiarCancionYReproduce(1);
		}
		return 0;
		case VK_F4://F4
		{
			ReproductorCambiarCancionYReproduce(2);
		}
		return 0;
		}
	}
	break;
	case WM_TIMER:
		if (wParam == Tick)
		{
			MainRender(hWnd);
			player->GetStatus(&status);
			if (!pausa && status.fPlay == 0) {
				ReproductorReproduce();
			}
		}
		break;
	case WM_PAINT:
	{
		HDC hdc;
		PAINTSTRUCT ps;
		hdc = BeginPaint(hWnd, &ps);
		HBITMAP h_CMC = CreateBitmap(ANCHO_VENTANA, ALTO_VENTANA, 1, 32, ptrBufferPixelsWindow);
		HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, h_CMC);//Creacion de marco para proyecto

		BitBlt(hdc, 0, 0, ANCHO_VENTANA, ALTO_VENTANA, hdcMem, 0, 0, SRCCOPY);

		if (pantallaJuego)
		{
			//aqui debes poner aliens comidos, pero crear una variable diferente para que no se traslape
			Escribir(hWnd, hdc);
		}

		if (pantallaInicial)
		{
			/*Escribir2(hWnd, hdc);*/
		}
		if (pantallaWinner) {
			Escribir3(hWnd, hdc);

			/*Escribir3(hWnd, hdc);*/
		}
		if (pantallaLooser) {
			Escribir4(hWnd, hdc);
			/*Escribir4(hWnd, hdc);*/
		}

		DeleteObject(h_CMC);
		//SelectObject(hdcMem, hbmOld);
		DeleteDC(hdcMem);
		//DeleteObject(hbmOld);
	}
	break;
	/*case WM_KEYDOWN:
		{
			KEYS[ wParam ] = true;
		}
		break;*/
	case WM_CLOSE:
	{

		aliensComidos = 0;
		DestroyWindow(hWnd);
	}
	break;
	case WM_DESTROY: //Send A Quit Message
	{
		KillTimer(hWnd, Tick);
		PostQuitMessage(0);
	}
	break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

/* Inicializacion de variables y reserva de memoria.
	*/
void Init()
{
	ReproductorInicializaYReproduce();

	ReproductorCambiarCancionYReproduce(0);

	CargaFramesSprite();

	//Cargamos imagen bitmap de nuestro escenario
	miStage.ImagenEscenario1 = gdipLoad(miStage.Bmp1);

	if (pantallaJuego)
	{
		miStage.ImagenEscenario2 = gdipLoad(miStage.Bmp2);
		ReproductorCambiarCancionYReproduce(0);
	}

	else if (pantallaWinner)
	{
		miStage.ImagenEscenario3 = gdipLoad(miStage.Bmp3);
		ReproductorCambiarCancionYReproduce(1);
	}

	else if (pantallaLooser)
	{
		miStage.ImagenEscenario4 = gdipLoad(miStage.Bmp4);
		ReproductorCambiarCancionYReproduce(2);
	}

	//Definimos un puntero del total de pixeles que tiene nuestra ventana
	ptrBufferPixelsWindow = new int[ANCHO_VENTANA * ALTO_VENTANA];

	// Definimos la animacion inicial
	AnimacionActual1 = Right;
	AnimacionActual3 = Right;
}

void TranScaleblt(int* punteroDestino, int* punteroOrigen, int inicioXDestino, int inicioYDestino, int inicioXOrigen, int inicioYOrigen, int ancho, int alto, int anchodefondo, int anchodeorigen, int escalaX, int escalaY, const unsigned int TRANSPARENCY, double limiteX) {
	//blt = block transfer, transferencia de bloque de imagen
	int bytesporlineafondo = anchodefondo * 4;
	int bytesporlineaorigen = anchodeorigen * 4;
	int bytesporlineaimagen = ancho * 4;
	if ((inicioXDestino + (ancho * 2) > 0 && inicioYDestino + alto > 0) && (inicioYDestino < 600 && inicioXDestino < 800)) { // Verificamos si el sprite esta dentro de los limites del buffer

		if (inicioXDestino < 0) { // verificamos si la esquina superior izquierda esta fueda del buffer, de ser asi, hacemos mas peque;o el sprite
			ancho += inicioXDestino;
			bytesporlineaimagen = ancho * 4;
			inicioXDestino = 0;
		}

		if ((inicioXDestino + ancho + limiteX) > 800) { // verificamos si la esquina inferior derecha esta fueda del buffer, de ser asi, hacemos mas peque;o el sprite
			ancho = (800 - (inicioXDestino + (limiteX * 2)));
			bytesporlineaimagen = ancho * 4;
			inicioXDestino = 800 - ancho;
		}

		if (inicioYDestino < 0) { // verificamos si la esquina superior izquierda esta fueda del buffer, de ser asi, hacemos mas peque;o el sprite
			alto += inicioYDestino;
			inicioYDestino = 0;
		}
		if ((inicioYDestino + alto) > 600) { // verificamos si la esquina inferior izquierda esta fueda del buffer, de ser asi, hacemos mas peque;o el sprite
			alto = (600 - inicioYDestino);
			inicioYDestino = 600 - alto;
		}
		if (ancho < 1) ancho = 1;
	}

	__asm {

		mov edi, punteroDestino //movemos la direccion del bitmap a edi, para poder escribir en el
		//Conseguimos el pixel inicial donde empezaremos a dibujar
		mov eax, inicioYDestino
		mul bytesporlineafondo //inicioY * bytesporlineafondo, asi iniciamos en la linea donde queremos 
		mov ebx, eax //ebx contendra el resultado anterior
		mov eax, 4
		mul inicioXDestino //inicioX*4, para asi encontrar la columnda donde queremos empezar a dibujar
		add eax, ebx //posicion de columna + posicion de linea
		add edi, eax //Sumamos el desplazamiento anterior al inicio de nuestra imagen para empezar a trabajar en la posicion deseada
		mov esi, punteroOrigen //movemos la direccion de la imagen a dibujar a esi, para poder escribir de ella
		//Conseguimos el pixel inicial DEL CUAL empezaremos a dibujar
		mov eax, inicioYOrigen
		mul bytesporlineaorigen //inicioY * bytesporlineaorigen, asi iniciamos en la linea donde queremos 
		mov ebx, eax //ebx contendra el resultado anterior
		mov eax, 4
		mul inicioXOrigen //inicioX*4, para asi encontrar la columnda de donde queremos empezar a leer
		add eax, ebx //posicion de columna + posicion de linea
		add esi, eax //Sumamos el desplazamiento anterior al inicio de nuestra imagen para empezar a trabajar en la posicion deseada
		mov eax, TRANSPARENCY

		mov ecx, alto //movemos a ecx la cantidad de lineas que dibujaremos

		lazollenarY :
		push ecx
			mov ecx, escalaY
			escaladoY :
		push ecx //guardamos el valor anterior de ecx, porque lo reemplazaremos en un nuevo ciclo
			mov ecx, ancho //la cantidad de columnas que dibujaremos
			lazollenarX :
		//movsd //mueve un pixel de la direccion apuntada por esi a la apuntada por edi, e incrementa esi y edi por 4
		push ecx
			mov ecx, escalaX
			escaladoX :
		cmp eax, [esi]
			je nodibujar
			mov edx, [esi]
			mov[edi], edx
			nodibujar :
		add edi, 4
			loop escaladoX
			add esi, 4
			pop ecx
			loop lazollenarX
			add edi, bytesporlineafondo //le sumamos la cantidad de bytes de la linea del fondo para pasar a la siguiente linea
			push eax
			mov eax, bytesporlineaimagen
			mul escalaX
			sub edi, eax //y retrocedemos una cantidad igual al a su ancho para dibujar desde la posicion X inicial y que no quede escalonado
			pop eax
			sub esi, bytesporlineaimagen
			pop ecx
			loop escaladoY
			//Lo mismo para esi
			add esi, bytesporlineaorigen
			pop ecx //recuperamos el valor del contador del ciclo exterior
			loop lazollenarY
	}

}

void mirrorxblt(int* punteroDestino,//Buffer donde comenzara a pasar los datos
	int* punteroOrigen, // Buffer que copiara los datos del bmp
	int inicioXDestino,//ventana
	int inicioYDestino,
	int inicioXOrigen,//bmp
	int inicioYOrigen,
	int ancho,//ancho del sprite
	int alto,//alto del sprite
	int anchodefondo,//dimensiones
	int anchodeorigen,
	int escalaX,//tamaño de escala X bmp
	int escalaY,//tamaño de escala Y bmp
	const unsigned int TRANSPARENCY) {//Color Transparencia
	//blt = block transfer, transferencia de bloque de imagen
	int bytesporlineafondo = anchodefondo * 4;//fondo-destino total
	int bytesporlineaorigen = anchodeorigen * 4;//bmp total
	int bytesporlineaimagen = ancho * 4;

	__asm {

		mov edi, punteroDestino //movemos la direccion del bitmap a edi, para poder escribir en el
		//Conseguimos el pixel inicial donde empezaremos a dibujar
		mov eax, inicioYDestino
		mul bytesporlineafondo //inicioY * bytesporlineafondo, asi iniciamos en la linea donde queremos 
		mov ebx, eax //ebx contendra el resultado anterior
		mov eax, 4
		mul inicioXDestino //inicioX*4, para asi encontrar la columnda donde queremos empezar a dibujar
		add eax, ebx //posicion de columna + posicion de linea
		add edi, eax //Sumamos el desplazamiento anterior al inicio de nuestra imagen para empezar a trabajar en la posicion deseada

		mov esi, punteroOrigen //movemos la direccion de la imagen a dibujar a esi, para poder escribir de ella
		//Conseguimos el pixel inicial DEL CUAL empezaremos a dibujar
		mov eax, inicioYOrigen
		mul bytesporlineaorigen //inicioY * bytesporlineaorigen, asi iniciamos en la linea donde queremos 
		mov ebx, eax //ebx contendra el resultado anterior
		mov eax, 4
		mul inicioXOrigen //inicioX*4, para asi encontrar la columnda de donde queremos empezar a leer
		add eax, ebx //posicion de columna + posicion de linea
		add esi, eax //Sumamos el desplazamiento anterior al inicio de nuestra imagen para empezar a trabajar en la posicion deseada


		mov eax, TRANSPARENCY
		mov ecx, alto //movemos a ecx la cantidad de lineas que dibujaremos
		lazollenarY :
		push ecx
			mov ecx, escalaY//es como el ancho
			sub esi, 4
			escaladoY :
			push ecx //guardamos el valor anterior de ecx, porque lo reemplazaremos en un nuevo ciclo
			mov ecx, ancho //la cantidad de columnas que dibujaremos
			add esi, bytesporlineaimagen

			lazollenarX :
		//movsd //mueve un pixel de la direccion apuntada por esi a la apuntada por edi, e incrementa esi y edi por 4
		push ecx
			mov ecx, escalaX
			escaladoX :
		cmp eax, [esi]
			je nodibujar
			mov edx, [esi]
			mov[edi], edx
			nodibujar :
		add edi, 4
			loop escaladoX
			sub esi, 4
			pop ecx
			loop lazollenarX
			add edi, bytesporlineafondo //le sumamos la cantidad de bytes de la linea del fondo para pasar a la siguiente linea
			push eax
			mov eax, escalaX
			mul bytesporlineaimagen
			mov ebx, eax
			pop eax
			sub edi, ebx //y retrocedemos una cantidad igual al a su ancho para dibujar desde la posicion X inicial y que no quede escalonado
			//sub edi, bytesporlineaimagen //y retrocedemos una cantidad igual al a su ancho para dibujar desde la posicion X inicial y que no quede escalonado

			pop ecx
			loop escaladoY
			//Lo mismo para esi
			add esi, bytesporlineaorigen
			add esi, 4
			//sub esi, bytesporlineaimagen
			pop ecx //recuperamos el valor del contador del ciclo exterior
			loop lazollenarY
	}

}

void DibujaHitbox(int* ptrBuffer,
	unsigned int color, int anchoWnd, int altoWnd, int HitboxX, int HitboxY, int HitboxAncho, int HitboxAlto, int escalaX, int escalaY)
{
	HitboxAncho = HitboxAncho * escalaX;
	HitboxAlto = HitboxAlto * escalaY;
	__asm {
		mov edi, ptrBuffer

		mov eax, HitboxY
		mul BPP
		mul anchoWnd
		add edi, eax

		mov eax, HitboxX
		mul BPP
		add edi, eax

		mov eax, color

		mov ecx, HitboxAlto
		lazo_alto :
		push ecx
			mov ecx, HitboxAncho
			lazo_ancho :
		mov[edi], eax
			add edi, BPP
			//stosd
			loop lazo_ancho
			push eax
			mov eax, anchoWnd
			sub eax, HitboxAncho
			mul BPP
			add edi, eax
			pop eax
			pop ecx
			loop lazo_alto
	}
}


//Se encarga de la animación del movimiento
void CargaFramesSprite()
{
	miEnemigo1.tipo = 0;
	miEnemigo2.tipo = 0;

	miLava1.tipo = 1;
	miLava2.tipo = 1;
	miLava3.tipo = 1;

	miObstaculo1.tipo = 2;
	miObstaculo2.tipo = 2;
	miObstaculo3.tipo = 2;

	//Cargamos primero las hojas de sprite
	miPersonaje1.HojaSprite = gdipLoad(miPersonaje1.BmpW);
	miPersonaje3.HojaSprite = gdipLoad(miPersonaje3.BmpW);

	miEnemigo1.HojaSprite = gdipLoad(miEnemigo1.BmpW);
	miEnemigo2.HojaSprite = gdipLoad(miEnemigo2.BmpW);

	miLava1.HojaSprite = gdipLoad(miLava1.BmpW);
	miLava2.HojaSprite = gdipLoad(miLava2.BmpW);
	miLava3.HojaSprite = gdipLoad(miLava2.BmpW);

	miObstaculo1.HojaSprite = gdipLoad(miObstaculo1.BmpW);
	miObstaculo2.HojaSprite = gdipLoad(miObstaculo2.BmpW);
	miObstaculo3.HojaSprite = gdipLoad(miObstaculo2.BmpW);
	//Definiendo las coordenadas iniciales en pantalla donde iniciaremos
	miPersonaje1.XCurrentCoordDraw = 120;
	miPersonaje1.YCurrentCoordDraw = 60;
	miPersonaje3.XCurrentCoordDraw = 60;
	miPersonaje3.YCurrentCoordDraw = 60;

	CambiarPosicion(miEnemigo1);
	CambiarPosicion(miEnemigo2);

	CambiarPosicion(miLava1);
	CambiarPosicion(miLava2);
	CambiarPosicion(miLava3);

	CambiarPosicion(miObstaculo1);
	CambiarPosicion(miObstaculo2);
	CambiarPosicion(miObstaculo3);
	//Definiendo los tamaños de nuestro sprite para renderizarlo en la ventana
	miPersonaje1.WeightDraw = 20;
	miPersonaje1.HeightDraw = 20;
	miPersonaje3.WeightDraw = 20;
	miPersonaje3.HeightDraw = 20;

	miEnemigo1.WeightDraw = 20;
	miEnemigo1.HeightDraw = 20;
	miEnemigo2.WeightDraw = 20;
	miEnemigo2.HeightDraw = 20;

	miLava1.WeightDraw = 20;
	miLava1.HeightDraw = 20;
	miLava2.WeightDraw = 20;
	miLava2.HeightDraw = 20;
	miLava3.WeightDraw = 20;
	miLava3.HeightDraw = 20;

	miObstaculo1.WeightDraw = 20;
	miObstaculo1.HeightDraw = 20;
	miObstaculo2.WeightDraw = 20;
	miObstaculo2.HeightDraw = 20;
	miObstaculo3.WeightDraw = 20;
	miObstaculo3.HeightDraw = 20;

	//Definiendo las dimenciones en base al # de Animaciones
	miPersonaje1.FrameSpriteArray = new FrameArray * [miPersonaje1.Animaciones]; // Cabeza
	miPersonaje3.FrameSpriteArray = new FrameArray * [miPersonaje3.Animaciones]; // Cola
	//Definiendo la cantidad de Frames en base a cada Animacion
	miPersonaje1.FrameSpriteArray[Right] = new FrameArray[miPersonaje1.FramesAnimacionRight];
	miPersonaje1.FrameSpriteArray[Left] = new FrameArray[miPersonaje1.FramesAnimacionLeft];
	miPersonaje1.FrameSpriteArray[Up] = new FrameArray[miPersonaje1.FramesAnimacionUp];
	miPersonaje1.FrameSpriteArray[Down] = new FrameArray[miPersonaje1.FramesAnimacionDown];

	miPersonaje3.FrameSpriteArray[Right] = new FrameArray[miPersonaje3.FramesAnimacionRight];
	miPersonaje3.FrameSpriteArray[Left] = new FrameArray[miPersonaje3.FramesAnimacionLeft];
	miPersonaje3.FrameSpriteArray[Up] = new FrameArray[miPersonaje3.FramesAnimacionUp];
	miPersonaje3.FrameSpriteArray[Down] = new FrameArray[miPersonaje3.FramesAnimacionDown];

	//Cargando Frames a nuestro arreglo del sprite
	// ------ - Animacion 1 Idle -------- // //Modifican la textura del personaje
	miPersonaje1.FrameSpriteArray[Right][Frame0].x = 0; miPersonaje1.FrameSpriteArray[Right][Frame0].y = 20;
	miPersonaje1.FrameSpriteArray[Right][Frame0].ancho = 20; miPersonaje1.FrameSpriteArray[Right][Frame0].alto = 20;

	miPersonaje3.FrameSpriteArray[Right][Frame0].x = 0; miPersonaje3.FrameSpriteArray[Right][Frame0].y = 20;
	miPersonaje3.FrameSpriteArray[Right][Frame0].ancho = 20; miPersonaje3.FrameSpriteArray[Right][Frame0].alto = 20;
	//

	miPersonaje1.FrameSpriteArray[Left][Frame0].x = 20; miPersonaje1.FrameSpriteArray[Left][Frame0].y = 0;
	miPersonaje1.FrameSpriteArray[Left][Frame0].ancho = 20; miPersonaje1.FrameSpriteArray[Left][Frame0].alto = 20;

	miPersonaje3.FrameSpriteArray[Left][Frame0].x = 20; miPersonaje3.FrameSpriteArray[Left][Frame0].y = 0;
	miPersonaje3.FrameSpriteArray[Left][Frame0].ancho = 20; miPersonaje3.FrameSpriteArray[Left][Frame0].alto = 20;

	//
	miPersonaje1.FrameSpriteArray[Up][Frame0].x = 20; miPersonaje1.FrameSpriteArray[Up][Frame0].y = 20;
	miPersonaje1.FrameSpriteArray[Up][Frame0].ancho = 20; miPersonaje1.FrameSpriteArray[Up][Frame0].alto = 20;

	miPersonaje3.FrameSpriteArray[Up][Frame0].x = 20; miPersonaje3.FrameSpriteArray[Up][Frame0].y = 20;
	miPersonaje3.FrameSpriteArray[Up][Frame0].ancho = 20; miPersonaje3.FrameSpriteArray[Up][Frame0].alto = 20;

	//
	miPersonaje1.FrameSpriteArray[Down][Frame0].x = 0; miPersonaje1.FrameSpriteArray[Down][Frame0].y = 0;
	miPersonaje1.FrameSpriteArray[Down][Frame0].ancho = 20; miPersonaje1.FrameSpriteArray[Down][Frame0].alto = 20;

	miPersonaje3.FrameSpriteArray[Down][Frame0].x = 0; miPersonaje3.FrameSpriteArray[Down][Frame0].y = 0;
	miPersonaje3.FrameSpriteArray[Down][Frame0].ancho = 20; miPersonaje3.FrameSpriteArray[Down][Frame0].alto = 20;

	//
	miEnemigo1.FrameSpriteArray = new FrameArray * [miEnemigo1.Animaciones];
	miEnemigo1.FrameSpriteArray[Idle] = new FrameArray[miEnemigo1.FramesAnimacionIdle];
	miEnemigo1.FrameSpriteArray[Idle][Frame0].x = 0; miEnemigo1.FrameSpriteArray[Idle][Frame0].y = 0;
	miEnemigo1.FrameSpriteArray[Idle][Frame0].ancho = 20; miEnemigo1.FrameSpriteArray[Idle][Frame0].alto = 20;

	miEnemigo2.FrameSpriteArray = new FrameArray * [miEnemigo2.Animaciones];
	miEnemigo2.FrameSpriteArray[Idle] = new FrameArray[miEnemigo2.FramesAnimacionIdle];
	miEnemigo2.FrameSpriteArray[Idle][Frame0].x = 0; miEnemigo2.FrameSpriteArray[Idle][Frame0].y = 0;
	miEnemigo2.FrameSpriteArray[Idle][Frame0].ancho = 20; miEnemigo2.FrameSpriteArray[Idle][Frame0].alto = 20;

	miLava1.FrameSpriteArray = new FrameArray * [miLava1.Animaciones];
	miLava1.FrameSpriteArray[Idle] = new FrameArray[miLava1.FramesAnimacionIdle];
	miLava1.FrameSpriteArray[Idle][Frame0].x = 0; miLava1.FrameSpriteArray[Idle][Frame0].y = 20;
	miLava1.FrameSpriteArray[Idle][Frame0].ancho = 20; miLava1.FrameSpriteArray[Idle][Frame0].alto = 20;

	miLava2.FrameSpriteArray = new FrameArray * [miLava2.Animaciones];
	miLava2.FrameSpriteArray[Idle] = new FrameArray[miLava2.FramesAnimacionIdle];
	miLava2.FrameSpriteArray[Idle][Frame0].x = 0; miLava2.FrameSpriteArray[Idle][Frame0].y = 20;
	miLava2.FrameSpriteArray[Idle][Frame0].ancho = 20; miLava2.FrameSpriteArray[Idle][Frame0].alto = 20;

	miLava3.FrameSpriteArray = new FrameArray * [miLava3.Animaciones];
	miLava3.FrameSpriteArray[Idle] = new FrameArray[miLava3.FramesAnimacionIdle];
	miLava3.FrameSpriteArray[Idle][Frame0].x = 0; miLava3.FrameSpriteArray[Idle][Frame0].y = 20;
	miLava3.FrameSpriteArray[Idle][Frame0].ancho = 20; miLava3.FrameSpriteArray[Idle][Frame0].alto = 20;

	miObstaculo1.FrameSpriteArray = new FrameArray * [miObstaculo1.Animaciones];
	miObstaculo1.FrameSpriteArray[Idle] = new FrameArray[miObstaculo1.FramesAnimacionIdle];
	miObstaculo1.FrameSpriteArray[Idle][Frame0].x = 20; miObstaculo1.FrameSpriteArray[Idle][Frame0].y = 0;
	miObstaculo1.FrameSpriteArray[Idle][Frame0].ancho = 20; miObstaculo1.FrameSpriteArray[Idle][Frame0].alto = 20;

	miObstaculo2.FrameSpriteArray = new FrameArray * [miObstaculo2.Animaciones];
	miObstaculo2.FrameSpriteArray[Idle] = new FrameArray[miObstaculo2.FramesAnimacionIdle];
	miObstaculo2.FrameSpriteArray[Idle][Frame0].x = 20; miObstaculo2.FrameSpriteArray[Idle][Frame0].y = 0;
	miObstaculo2.FrameSpriteArray[Idle][Frame0].ancho = 20; miObstaculo2.FrameSpriteArray[Idle][Frame0].alto = 20;

	miObstaculo3.FrameSpriteArray = new FrameArray * [miObstaculo3.Animaciones];
	miObstaculo3.FrameSpriteArray[Idle] = new FrameArray[miObstaculo3.FramesAnimacionIdle];
	miObstaculo3.FrameSpriteArray[Idle][Frame0].x = 20; miObstaculo3.FrameSpriteArray[Idle][Frame0].y = 0;
	miObstaculo3.FrameSpriteArray[Idle][Frame0].ancho = 20; miObstaculo3.FrameSpriteArray[Idle][Frame0].alto = 20;

	cuerpo.clear();
	AgrandarCuerpo();
}

/* Funcion principal. Encargada de hacer el redibujado en pantalla cada intervalo (o "Tick") del timer que se haya creado.
	@param hWnd. Manejador de la ventana.
	*/
void DibujaPixeles() //Medio Listo
{
	if (pantallaInicial)
	{
		//Dibujamos el fondo
		TranScaleblt(ptrBufferPixelsWindow, (miStage.ImagenEscenario1.pixeles),
			0, 0,//Iniciamos a dibujar en la ventana en 0,0
			0, 0,//Indicamos cuales son las coordenadas para dibujar desde nuestra imagen; iniciamos en 0,0 desde nuestro escenario
			ANCHO_VENTANA, ALTO_VENTANA,//Definimos cuantos pixeles dibujaremos de nuestra imagen a la pantalla
			1000, miStage.ImagenEscenario1.ancho,
			1, 1);//Si ponemos un numero mayor a 1 estaremos repitiendo 2 veces la linea de pixeles en X o en Y

	}

	else if (pantallaJuego)
	{
		//Dibujamos el fondo
		TranScaleblt(ptrBufferPixelsWindow, (miStage.ImagenEscenario2.pixeles),
			0, 0,//Iniciamos a dibujar en la ventana en 0,0
			0, 0,//Indicamos cuales son las coordenadas para dibujar desde nuestra imagen; iniciamos en 0,0 desde nuestro escenario
			ANCHO_VENTANA, ALTO_VENTANA,//Definimos cuantos pixeles dibujaremos de nuestra imagen a la pantalla
			1000, miStage.ImagenEscenario2.ancho,
			1, 1);//Si ponemos un numero mayor a 1 estaremos repitiendo 2 veces la linea de pixeles en X o en Y

		//Dibujamos a nuestro personaje
		TranScaleblt(ptrBufferPixelsWindow, (miPersonaje3.HojaSprite.pixeles),
			miPersonaje3.XCurrentCoordDraw, miPersonaje3.YCurrentCoordDraw,
			miPersonaje3.FrameSpriteArray[AnimacionActual3][FrameActual].x, miPersonaje3.FrameSpriteArray[AnimacionActual3][FrameActual].y,
			miPersonaje3.FrameSpriteArray[AnimacionActual3][FrameActual].ancho, miPersonaje3.FrameSpriteArray[AnimacionActual3][FrameActual].alto,
			1000, miPersonaje3.HojaSprite.ancho,
			3, 3, 0xFF880015, 1);  //Hacer que abarque la seleccion de toda la pantalla

		for (int i = cuerpo.size() - 1; i >= 0; i--) {
			TranScaleblt(ptrBufferPixelsWindow, (cuerpo.at(i).HojaSprite.pixeles),
				cuerpo.at(i).XCurrentCoordDraw, cuerpo.at(i).YCurrentCoordDraw,
				cuerpo.at(i).FrameSpriteArray[cuerpo.at(i).AnimacionActual][FrameActual].x, cuerpo.at(i).FrameSpriteArray[cuerpo.at(i).AnimacionActual][FrameActual].y,
				cuerpo.at(i).FrameSpriteArray[cuerpo.at(i).AnimacionActual][FrameActual].ancho, cuerpo.at(i).FrameSpriteArray[cuerpo.at(i).AnimacionActual][FrameActual].alto,
				1000, cuerpo.at(i).HojaSprite.ancho,
				3, 3, 0xFF880015, 1);  //Hacer que abarque la seleccion de toda la pantalla
		}

		TranScaleblt(ptrBufferPixelsWindow, (miPersonaje1.HojaSprite.pixeles),
			miPersonaje1.XCurrentCoordDraw, miPersonaje1.YCurrentCoordDraw,
			miPersonaje1.FrameSpriteArray[AnimacionActual1][FrameActual].x, miPersonaje1.FrameSpriteArray[AnimacionActual1][FrameActual].y,
			miPersonaje1.FrameSpriteArray[AnimacionActual1][FrameActual].ancho, miPersonaje1.FrameSpriteArray[AnimacionActual1][FrameActual].alto,
			1000, miPersonaje1.HojaSprite.ancho,
			3, 3, 0xFF880015, 1);  //Hacer que abarque la seleccion de toda la pantalla

		// Enemigos
		TranScaleblt(ptrBufferPixelsWindow, (miEnemigo1.HojaSprite.pixeles),
			miEnemigo1.XCurrentCoordDraw, miEnemigo1.YCurrentCoordDraw,
			miEnemigo1.FrameSpriteArray[Idle][Frame0].x, miEnemigo1.FrameSpriteArray[Idle][Frame0].y,
			miEnemigo1.FrameSpriteArray[Idle][Frame0].ancho, miEnemigo1.FrameSpriteArray[Idle][Frame0].alto,
			1000, miEnemigo1.HojaSprite.ancho,
			3, 3, 0xFF880015, 1);  //Hacer que abarque la seleccion de toda la pantalla

		TranScaleblt(ptrBufferPixelsWindow, (miEnemigo2.HojaSprite.pixeles),
			miEnemigo2.XCurrentCoordDraw, miEnemigo2.YCurrentCoordDraw,
			miEnemigo2.FrameSpriteArray[Idle][Frame0].x, miEnemigo2.FrameSpriteArray[Idle][Frame0].y,
			miEnemigo2.FrameSpriteArray[Idle][Frame0].ancho, miEnemigo2.FrameSpriteArray[Idle][Frame0].alto,
			1000, miEnemigo2.HojaSprite.ancho,
			3, 3, 0xFF880015, 1);  //Hacer que abarque la seleccion de toda la pantalla

		//Obstaculos
		TranScaleblt(ptrBufferPixelsWindow, (miLava1.HojaSprite.pixeles),
			miLava1.XCurrentCoordDraw, miLava1.YCurrentCoordDraw,
			miLava1.FrameSpriteArray[Idle][Frame0].x, miLava1.FrameSpriteArray[Idle][Frame0].y,
			miLava1.FrameSpriteArray[Idle][Frame0].ancho, miLava1.FrameSpriteArray[Idle][Frame0].alto,
			1000, miLava1.HojaSprite.ancho,
			3, 3, 0xFF880015, 1);  //Hacer que abarque la seleccion de toda la pantalla

		TranScaleblt(ptrBufferPixelsWindow, (miLava2.HojaSprite.pixeles),
			miLava2.XCurrentCoordDraw, miLava2.YCurrentCoordDraw,
			miLava2.FrameSpriteArray[Idle][Frame0].x, miLava2.FrameSpriteArray[Idle][Frame0].y,
			miLava2.FrameSpriteArray[Idle][Frame0].ancho, miLava2.FrameSpriteArray[Idle][Frame0].alto,
			1000, miLava2.HojaSprite.ancho,
			3, 3, 0xFF880015, 1);  //Hacer que abarque la seleccion de toda la pantalla

		TranScaleblt(ptrBufferPixelsWindow, (miLava3.HojaSprite.pixeles),
			miLava3.XCurrentCoordDraw, miLava3.YCurrentCoordDraw,
			miLava3.FrameSpriteArray[Idle][Frame0].x, miLava3.FrameSpriteArray[Idle][Frame0].y,
			miLava3.FrameSpriteArray[Idle][Frame0].ancho, miLava3.FrameSpriteArray[Idle][Frame0].alto,
			1000, miLava3.HojaSprite.ancho,
			3, 3, 0xFF880015, 1);  //Hacer que abarque la seleccion de toda la pantalla

		TranScaleblt(ptrBufferPixelsWindow, (miObstaculo1.HojaSprite.pixeles),
			miObstaculo1.XCurrentCoordDraw, miObstaculo1.YCurrentCoordDraw,
			miObstaculo1.FrameSpriteArray[Idle][Frame0].x, miObstaculo1.FrameSpriteArray[Idle][Frame0].y,
			miObstaculo1.FrameSpriteArray[Idle][Frame0].ancho, miObstaculo1.FrameSpriteArray[Idle][Frame0].alto,
			1000, miObstaculo1.HojaSprite.ancho,
			3, 3, 0xFF880015, 1);  //Hacer que abarque la seleccion de toda la pantalla

		TranScaleblt(ptrBufferPixelsWindow, (miObstaculo2.HojaSprite.pixeles),
			miObstaculo2.XCurrentCoordDraw, miObstaculo2.YCurrentCoordDraw,
			miObstaculo2.FrameSpriteArray[Idle][Frame0].x, miObstaculo2.FrameSpriteArray[Idle][Frame0].y,
			miObstaculo2.FrameSpriteArray[Idle][Frame0].ancho, miObstaculo2.FrameSpriteArray[Idle][Frame0].alto,
			1000, miObstaculo2.HojaSprite.ancho,
			3, 3, 0xFF880015, 1);  //Hacer que abarque la seleccion de toda la pantalla

		TranScaleblt(ptrBufferPixelsWindow, (miObstaculo3.HojaSprite.pixeles),
			miObstaculo3.XCurrentCoordDraw, miObstaculo3.YCurrentCoordDraw,
			miObstaculo3.FrameSpriteArray[Idle][Frame0].x, miObstaculo3.FrameSpriteArray[Idle][Frame0].y,
			miObstaculo3.FrameSpriteArray[Idle][Frame0].ancho, miObstaculo3.FrameSpriteArray[Idle][Frame0].alto,
			1000, miObstaculo3.HojaSprite.ancho,
			3, 3, 0xFF880015, 1);  //Hacer que abarque la seleccion de toda la pantalla
	}

	else if (pantallaWinner)
	{
		//Dibujamos el fondo Winner
		TranScaleblt(ptrBufferPixelsWindow, (miStage.ImagenEscenario3.pixeles),
			0, 0,//Iniciamos a dibujar en la ventana en 0,0
			0, 0,//Indicamos cuales son las coordenadas para dibujar desde nuestra imagen; iniciamos en 0,0 desde nuestro escenario
			ANCHO_VENTANA, ALTO_VENTANA,//Definimos cuantos pixeles dibujaremos de nuestra imagen a la pantalla
			1000, miStage.ImagenEscenario3.ancho,
			1, 1);//Si ponemos un numero mayor a 1 estaremos repitiendo 2 veces la linea de pixeles en X o en Y

	}

	else if (pantallaLooser)
	{
		//Dibujamos el fondo Looser
		TranScaleblt(ptrBufferPixelsWindow, (miStage.ImagenEscenario4.pixeles),
			0, 0,//Iniciamos a dibujar en la ventana en 0,0
			0, 0,//Indicamos cuales son las coordenadas para dibujar desde nuestra imagen; iniciamos en 0,0 desde nuestro escenario
			ANCHO_VENTANA, ALTO_VENTANA,//Definimos cuantos pixeles dibujaremos de nuestra imagen a la pantalla
			1000, miStage.ImagenEscenario4.ancho,
			1, 1);//Si ponemos un numero mayor a 1 estaremos repitiendo 2 veces la linea de pixeles en X o en Y

	}
	else
	{
	}
}

void ActualizaAnimacion(HWND hWnd) {
	switch (AnimacionActual1) {

	case Right:
		//if (Tick % Tick == 0 && FrameActual == 0)//125
		//{
		//	DelayFrameAnimation++;
		//}
		//else if (Tick % Tick == 0 && FrameActual == 2)
		//{
		//	DelayFrameAnimation += 2;
		//}
		//if (DelayFrameAnimation % 18 == 0)
		//{
		//	FrameActual++;
		//	if (FrameActual > 5) FrameActual = 0;
		//}

		break;

	}

	InvalidateRect(hWnd, NULL, FALSE);
	UpdateWindow(hWnd);
}

void MainRender(HWND hWnd)
{
	LimpiarFondo(ptrBufferPixelsWindow, 0xFF800020, (ANCHO_VENTANA * ALTO_VENTANA));
	KeysEvents();
	DibujaPixeles();
	ActualizaAnimacion(hWnd);
}

void Frame(float deltatime)
{

}

//Input de las teclas para movernos
void KeysEvents()
{
	if (pantallaInicial) {
		if (KEYS[input.Enter])
		{
			pantallaInicial = false;
			pantallaJuego = true;
			Init();
		}

		if (KEYS[input.Escape])
		{
			pantallaInicial = false;
			pantallaJuego = true;
			exit(-1);
		}
	}
	else if (pantallaJuego)
	{
		if (KEYS[input.Backspace])
		{
			pantallaInicial = true;
			pantallaJuego = false;
			ReproductorPausa();
			Init();
			return;
		}

		if ((KEYS[input.D] || KEYS[input.Right]) && AnimacionActual1 != Left && miPersonaje1.XCurrentCoordDraw < 900)
		{
			MoverCuerpo();

			miPersonaje1.XCurrentCoordDraw += 60;

			AnimacionActual1 = Right;
			ActualizarCuerpo();
		}
		else if ((KEYS[input.A] || KEYS[input.Left]) && AnimacionActual1 != Right && miPersonaje1.XCurrentCoordDraw > 60)
		{
			MoverCuerpo();

			miPersonaje1.XCurrentCoordDraw -= 60;

			AnimacionActual1 = Left;
			ActualizarCuerpo();
		}

		else if ((KEYS[input.W] || KEYS[input.Up]) && AnimacionActual1 != Down && miPersonaje1.YCurrentCoordDraw > 60)
		{
			MoverCuerpo();

			miPersonaje1.YCurrentCoordDraw -= 60;

			AnimacionActual1 = Up;
			ActualizarCuerpo();
		}
		else if ((KEYS[input.S] || KEYS[input.Down]) && AnimacionActual1 != Up && miPersonaje1.YCurrentCoordDraw < 400)
		{
			MoverCuerpo();

			miPersonaje1.YCurrentCoordDraw += 60;

			AnimacionActual1 = Down;
			ActualizarCuerpo();
		}

		//else
		//{
		//	AnimacionActual = Idle;
		//	FrameActual = 0;
		//}
	}
	else if (pantallaWinner) {
		if (KEYS[input.Enter])
		{
			pantallaWinner = false;
			pantallaInicial = true;
			Init();
		}
	}
	else if (pantallaLooser) {
		if (KEYS[input.Enter])
		{
			pantallaLooser = false;
			pantallaInicial = true;
			Init();
		}
	}

}
#pragma region LENS_CODE
/* Pinta el fondo de la ventana de acuerdo al color especificado.
	@param *ptrBuffer.	Puntero al area de memoria reservada para el proceso de dibujado.
	@param color.		Color expresado en formato hexadecimal.
	@param area.		Area de la ventana.
	*/
void LimpiarFondo(int* ptrBuffer, unsigned int colorFondo, int area)
{
	__asm 
	{
		mov edi, ptrBuffer
		mov ecx, area
		mov eax, colorFondo

		draw :
		mov[edi], eax
			add edi, BPP
			loop draw

			rep stosd
	}
}

/* Funcion que pinta una figura rectangular en pantalla.
	@param *ptrBuffer.	Puntero al area de memoria reservada para el dibujado.
	@param color.		Color de la figura expresado en formato hexadecimal.
	@param anchoWnd.	Ancho total de la ventana.
	@param altoWnd.		Alto total de la ventana.
	@param dmnFigura.	Especifica las dimensiones de la figura en relacion con la ventana.
	@param posFigura.	Posiciona la figura en la ventana.
	*/

void TranScaleblt(int* punteroDestino, int* punteroOrigen, int inicioXDestino, int inicioYDestino, int inicioXOrigen, int inicioYOrigen, int ancho, int alto, int anchodefondo, int anchodeorigen, int escalaX, int escalaY) {
	//blt = block transfer, transferencia de bloque de imagen
	int bytesporlineafondo = anchodefondo * 4;
	int bytesporlineaorigen = anchodeorigen * 4;
	int bytesporlineaimagen = ancho * 4;

	__asm
	{
		mov edi, punteroDestino //movemos la direccion del bitmap a edi, para poder escribir en el
		//Conseguimos el pixel inicial donde empezaremos a dibujar
		mov eax, inicioYDestino
		mul bytesporlineafondo //inicioY * bytesporlineafondo, asi iniciamos en la linea donde queremos 
		mov ebx, eax //ebx contendra el resultado anterior
		mov eax, 4
		mul inicioXDestino //inicioX*4, para asi encontrar la columna donde queremos empezar a dibujar
		add eax, ebx //posicion de columna + posicion de linea
		add edi, eax //Sumamos el desplazamiento anterior al inicio de nuestra imagen para empezar a trabajar en la posicion deseada
		mov esi, punteroOrigen //movemos la direccion de la imagen a dibujar a esi, para poder escribir de ella

		//Conseguimos el pixel inicial DEL CUAL empezaremos a dibujar
		mov eax, inicioYOrigen
		mul bytesporlineaorigen //inicioY * bytesporlineaorigen, asi iniciamos en la linea donde queremos 
		mov ebx, eax //ebx contendra el resultado anterior
		mov eax, 4
		mul inicioXOrigen //inicioX*4, para asi encontrar la columnda de donde queremos empezar a leer
		add eax, ebx //posicion de columna + posicion de linea
		add esi, eax //Sumamos el desplazamiento anterior al inicio de nuestra imagen para empezar a trabajar en la posicion deseada		
		mov eax, [esi]
		mov ecx, alto //movemos a ecx la cantidad de lineas que dibujaremos
		lazollenarY :
		push ecx
			mov ecx, escalaY
			escaladoY :
		push ecx //guardamos el valor anterior de ecx, porque lo reemplazaremos en un nuevo ciclo
			mov ecx, ancho //la cantidad de columnas que dibujaremos
			lazollenarX :
		//mueve un pixel de la direccion apuntada por esi a la apuntada por edi, e incrementa esi y edi por 4
		push ecx
			mov ecx, escalaX
			escaladoX :
		cmp eax, [esi]
			je nodibujar
			mov edx, [esi]
			mov[edi], edx
			nodibujar :
		add edi, 4
			loop escaladoX
			add esi, 4
			pop ecx
			loop lazollenarX
			add edi, bytesporlineafondo //le sumamos la cantidad de bytes de la linea del fondo para pasar a la siguiente linea
			push eax
			mov eax, bytesporlineaimagen
			mul escalaX
			sub edi, eax //y retrocedemos una cantidad igual al a su ancho para dibujar desde la posicion X inicial y que no quede escalonado
			pop eax
			sub esi, bytesporlineaimagen
			pop ecx
			loop escaladoY
			//Lo mismo para esi
			add esi, bytesporlineaorigen
			pop ecx //recuperamos el valor del contador del ciclo exterior
			loop lazollenarY
	}
}
#pragma endregion

void ReproductorInicializaYReproduce() {
	Cancion[0].Nombre = "Inicio";
	Cancion[0].Dir = "Recursos/PorterPiano.mp3";
	Cancion[1].Nombre = "Victoria";
	Cancion[1].Dir = "Recursos/Funky.mp3";
	Cancion[2].Nombre = "Derrota";
	Cancion[2].Dir = "Recursos/Faro.mp3";
	ifstream inputFile(Cancion[0].Dir.c_str());

	if (!inputFile.good())
		printf("No file found");
	else
		player->OpenFile(Cancion[0].Dir.c_str(), sfAutodetect);
	player->SetMasterVolume(25, 25);// Sonido tipo estereo Left and Right - Volumen de 0 - 100
	player->Play();
}

void ReproductorCambiarCancionYReproduce(int NumeroCancionAeproducir) {
	player->Stop();
	ifstream inputFile(Cancion[NumeroCancionAeproducir].Dir.c_str());

	if (!inputFile.good())
		printf("No file found");
	else
		player->OpenFile(Cancion[NumeroCancionAeproducir].Dir.c_str(), sfAutodetect);
	player->Play();
}

void ReproductorPausa() {
	player->Pause();
	pausa = true;
}

void ReproductorReproduce() {
	if (pausa)
		player->Resume();
	else
		player->Play();
	pausa = false;
}


//Las medidas del sprite ya estan definidas, sin embargo, sera corregida su animación en la tercer entrega
void CambiarPosicion(DatosEnemigo& _enemigo, bool force)
{
	_enemigo.XCurrentCoordDraw = randomInt(2, 12) * 60;
	_enemigo.YCurrentCoordDraw = randomInt(2, 6) * 60;

	if (_enemigo.tipo == 0) {
		if (_enemigo.XCurrentCoordDraw == miLava1.XCurrentCoordDraw && _enemigo.YCurrentCoordDraw == miLava1.YCurrentCoordDraw) { CambiarPosicion(_enemigo); return; }
		if (_enemigo.XCurrentCoordDraw == miLava2.XCurrentCoordDraw && _enemigo.YCurrentCoordDraw == miLava2.YCurrentCoordDraw) { CambiarPosicion(_enemigo); return; }
		if (_enemigo.XCurrentCoordDraw == miLava3.XCurrentCoordDraw && _enemigo.YCurrentCoordDraw == miLava3.YCurrentCoordDraw) { CambiarPosicion(_enemigo); return; }
		if (_enemigo.XCurrentCoordDraw == miObstaculo1.XCurrentCoordDraw && _enemigo.YCurrentCoordDraw == miObstaculo1.YCurrentCoordDraw) { CambiarPosicion(_enemigo); return; }
		if (_enemigo.XCurrentCoordDraw == miObstaculo2.XCurrentCoordDraw && _enemigo.YCurrentCoordDraw == miObstaculo2.YCurrentCoordDraw) { CambiarPosicion(_enemigo); return; }
		if (_enemigo.XCurrentCoordDraw == miObstaculo3.XCurrentCoordDraw && _enemigo.YCurrentCoordDraw == miObstaculo3.YCurrentCoordDraw) { CambiarPosicion(_enemigo); return; }
	}
	else if (_enemigo.tipo == 1 || _enemigo.tipo == 2) {
		if (_enemigo.XCurrentCoordDraw == miEnemigo1.XCurrentCoordDraw && _enemigo.YCurrentCoordDraw == miEnemigo1.YCurrentCoordDraw) { CambiarPosicion(_enemigo); return; }
		if (_enemigo.XCurrentCoordDraw == miEnemigo2.XCurrentCoordDraw && _enemigo.YCurrentCoordDraw == miEnemigo2.YCurrentCoordDraw) { CambiarPosicion(_enemigo); return; }
	}

	if (_enemigo.XCurrentCoordDraw == miPersonaje1.XCurrentCoordDraw && _enemigo.YCurrentCoordDraw == miPersonaje1.YCurrentCoordDraw) { CambiarPosicion(_enemigo); return; }
	for (int i = 0; i < cuerpo.size(); i++) {
		if (_enemigo.XCurrentCoordDraw == cuerpo.at(i).XCurrentCoordDraw && _enemigo.YCurrentCoordDraw == cuerpo.at(i).YCurrentCoordDraw) { CambiarPosicion(_enemigo); return; }
	}
	if (_enemigo.XCurrentCoordDraw == miPersonaje3.XCurrentCoordDraw && _enemigo.YCurrentCoordDraw == miPersonaje3.YCurrentCoordDraw) { CambiarPosicion(_enemigo); return; }

	if (!force) { return; }

	/*CambiarPosicion(miLava1, false);
	if (randomInt(0, 1)) { CambiarPosicion(miObstaculo1, false); }
	else if (randomInt(0, 1)) { CambiarPosicion(miLava2, false); }
	else if (randomInt(0, 1)) { CambiarPosicion(miObstaculo2, false); }
	else if (randomInt(0, 1)) { CambiarPosicion(miLava3, false); }
	else if (randomInt(0, 1)) { CambiarPosicion(miObstaculo3, false); }  */
}

void ColisionAlien(DatosEnemigo& _enemigo)
{
	if (miPersonaje1.XCurrentCoordDraw != _enemigo.XCurrentCoordDraw) { return; }

	if (miPersonaje1.YCurrentCoordDraw != _enemigo.YCurrentCoordDraw) { return; }

	CambiarPosicion(_enemigo);

	alienAgrandar--;
	aliensComidos++;

	if (alienAgrandar <= 0) {
		AgrandarCuerpo();
		alienAgrandar = 2;

		CambiarPosicion(miLava1, false);
		CambiarPosicion(miObstaculo1, false);
		CambiarPosicion(miLava2, false);
		CambiarPosicion(miObstaculo2, false);
		CambiarPosicion(miLava3, false);
		CambiarPosicion(miObstaculo3, false);
	}

	if (aliensComidos >= 20)
	{
		pantallaInicial = false;
		pantallaJuego = false;
		pantallaWinner = true;
		vidas = 3;
		Init();
	}
}

bool ColisionObjeto(DatosEnemigo& _enemigo)
{
	if (miPersonaje1.XCurrentCoordDraw != _enemigo.XCurrentCoordDraw) { return false; }
	if (miPersonaje1.YCurrentCoordDraw != _enemigo.YCurrentCoordDraw) { return false; }
	return true;
}


void ActualizarCuerpo() {
	for (int i = 0; i < cuerpo.size(); i++)
	{
		if (miPersonaje1.XCurrentCoordDraw != cuerpo.at(i).XCurrentCoordDraw) { continue; }
		if (miPersonaje1.YCurrentCoordDraw != cuerpo.at(i).YCurrentCoordDraw) { continue; }

		pantallaInicial = false;
		pantallaJuego = false;
		pantallaLooser = true;
		vidas = 3;
		Init();

		return;
	}

	if (miPersonaje3.XCurrentCoordDraw < cuerpo.at(0).XCurrentCoordDraw) {
		AnimacionActual3 = Right;
	}
	else if (miPersonaje3.XCurrentCoordDraw > cuerpo.at(0).XCurrentCoordDraw) {
		AnimacionActual3 = Left;
	}
	else if (miPersonaje3.YCurrentCoordDraw < cuerpo.at(0).YCurrentCoordDraw) {
		AnimacionActual3 = Down;
	}
	else if (miPersonaje3.YCurrentCoordDraw > cuerpo.at(0).YCurrentCoordDraw) {
		AnimacionActual3 = Up;
	}

	for (int i = 0; i < cuerpo.size(); i++) {
		if (i == 0 && cuerpo.size() <= 1) {
			if (miPersonaje1.XCurrentCoordDraw == miPersonaje3.XCurrentCoordDraw) {
				cuerpo.at(i).AnimacionActual = Vertical;
			}
			else if (miPersonaje1.YCurrentCoordDraw == miPersonaje3.YCurrentCoordDraw) {
				cuerpo.at(i).AnimacionActual = Horizontal;
			}
			else {
				cuerpo.at(i).AnimacionActual = Cuerpo;
			}
		}
		else if (i == 0 && cuerpo.size() > 1) {
			if (miPersonaje3.XCurrentCoordDraw == cuerpo.at(i + 1).XCurrentCoordDraw) {
				cuerpo.at(i).AnimacionActual = Vertical;
			}
			else if (miPersonaje3.YCurrentCoordDraw == cuerpo.at(i + 1).YCurrentCoordDraw) {
				cuerpo.at(i).AnimacionActual = Horizontal;
			}
			else {
				cuerpo.at(i).AnimacionActual = Cuerpo;
			}
		}
		else if (i == cuerpo.size() - 1) {
			if (miPersonaje1.XCurrentCoordDraw == cuerpo.at(i - 1).XCurrentCoordDraw) {
				cuerpo.at(i).AnimacionActual = Vertical;
			}
			else if (miPersonaje1.YCurrentCoordDraw == cuerpo.at(i - 1).YCurrentCoordDraw) {
				cuerpo.at(i).AnimacionActual = Horizontal;
			}
			else {
				cuerpo.at(i).AnimacionActual = Cuerpo;
			}
		}
		else {
			if (cuerpo.at(i - 1).XCurrentCoordDraw == cuerpo.at(i + 1).XCurrentCoordDraw) {
				cuerpo.at(i).AnimacionActual = Vertical;
			}
			else if (cuerpo.at(i - 1).YCurrentCoordDraw == cuerpo.at(i + 1).YCurrentCoordDraw) {
				cuerpo.at(i).AnimacionActual = Horizontal;
			}
			else {
				cuerpo.at(i).AnimacionActual = Cuerpo;
			}
		}
	}

	ColisionAlien(miEnemigo1);
	ColisionAlien(miEnemigo2);

	if (ColisionObjeto(miLava1) || ColisionObjeto(miLava2) || ColisionObjeto(miLava3)) {
		pantallaInicial = false;
		pantallaJuego = false;
		pantallaLooser = true;

		vidas = 3;
		Init();
	}

	else if (ColisionObjeto(miObstaculo1) || ColisionObjeto(miObstaculo2) || ColisionObjeto(miObstaculo3)) {
		if (vidas > 1)
		{
			vidas--;
			CambiarPosicion(miObstaculo1, false);
			CambiarPosicion(miObstaculo2, false);
			CambiarPosicion(miObstaculo3, false);
		}
		else {
			pantallaInicial = false;
			pantallaJuego = false;
			pantallaLooser = true;
			vidas = 3;
			Init();
		}
	}
}

void MoverCuerpo()
{
	miPersonaje3.XCurrentCoordDraw = cuerpo.at(0).XCurrentCoordDraw;
	miPersonaje3.YCurrentCoordDraw = cuerpo.at(0).YCurrentCoordDraw;

	for (int i = 0; i < cuerpo.size(); i++) {
		if (i == cuerpo.size() - 1) {
			cuerpo.at(i).XCurrentCoordDraw = miPersonaje1.XCurrentCoordDraw;
			cuerpo.at(i).YCurrentCoordDraw = miPersonaje1.YCurrentCoordDraw;
		}
		else {
			cuerpo.at(i).XCurrentCoordDraw = cuerpo.at(i + 1).XCurrentCoordDraw;
			cuerpo.at(i).YCurrentCoordDraw = cuerpo.at(i + 1).YCurrentCoordDraw;
		}
	}
}
void AgrandarCuerpo()
{
	DatosPersonaje2 _nuevoCuerpo;
	_nuevoCuerpo.HojaSprite = gdipLoad(_nuevoCuerpo.BmpW);
	_nuevoCuerpo.XCurrentCoordDraw = miPersonaje1.XCurrentCoordDraw;
	_nuevoCuerpo.YCurrentCoordDraw = miPersonaje1.YCurrentCoordDraw;
	_nuevoCuerpo.WeightDraw = 20;
	_nuevoCuerpo.HeightDraw = 20;

	_nuevoCuerpo.FrameSpriteArray = new FrameArray * [_nuevoCuerpo.Animaciones]; // Cuerpo 

	_nuevoCuerpo.FrameSpriteArray[Vertical] = new FrameArray[_nuevoCuerpo.FramesAnimacionVertical];
	_nuevoCuerpo.FrameSpriteArray[Horizontal] = new FrameArray[_nuevoCuerpo.FramesAnimacionHorizontal];
	_nuevoCuerpo.FrameSpriteArray[Cuerpo] = new FrameArray[_nuevoCuerpo.FramesAnimacionCuerpo];

	_nuevoCuerpo.FrameSpriteArray[Vertical][Frame0].x = 0; _nuevoCuerpo.FrameSpriteArray[Vertical][Frame0].y = 0;
	_nuevoCuerpo.FrameSpriteArray[Vertical][Frame0].ancho = 20; _nuevoCuerpo.FrameSpriteArray[Vertical][Frame0].alto = 20;

	_nuevoCuerpo.FrameSpriteArray[Horizontal][Frame0].x = 20; _nuevoCuerpo.FrameSpriteArray[Horizontal][Frame0].y = 0;
	_nuevoCuerpo.FrameSpriteArray[Horizontal][Frame0].ancho = 20; _nuevoCuerpo.FrameSpriteArray[Horizontal][Frame0].alto = 20;

	_nuevoCuerpo.FrameSpriteArray[Cuerpo][Frame0].x = 0; _nuevoCuerpo.FrameSpriteArray[Cuerpo][Frame0].y = 20;
	_nuevoCuerpo.FrameSpriteArray[Cuerpo][Frame0].ancho = 20; _nuevoCuerpo.FrameSpriteArray[Cuerpo][Frame0].alto = 20;

	_nuevoCuerpo.AnimacionActual = AnimacionActual1;

	cuerpo.push_back(_nuevoCuerpo);

	switch (AnimacionActual1) {
	case Left: {
		miPersonaje1.XCurrentCoordDraw -= 60;

		break;
	}
	case Up: {
		miPersonaje1.YCurrentCoordDraw -= 60;

		break;
	}
	case Down: {
		miPersonaje1.YCurrentCoordDraw += 60;

		break;
	}
	case Right: {
		miPersonaje1.XCurrentCoordDraw += 60;

		break;
	}
	}
	ActualizarCuerpo();
}

void Escribir(HWND hwnd, HDC hdc)
{
	char buffer[32] = { 0 };
	sprintf_s(buffer, "Aliens: %d/20 | Vidas: %d", aliensComidos, vidas);

	RECT rc;
	GetClientRect(hwnd, &rc);
	DrawTextA(hdc, (LPSTR)buffer, -1, &rc, DT_SINGLELINE);
	ReleaseDC(hwnd, hdc);
}

void Escribir2(HWND hwnd, HDC hdc)
{
	//La clave es poner aliensComidos2 
	char buffer[32] = { 0 };
	sprintf_s(buffer, "Puntuación Anterior: %d/20", aliensComidos);

	RECT rc;
	GetClientRect(hwnd, &rc);
	DrawTextA(hdc, (LPSTR)buffer, -1, &rc, DT_SINGLELINE);
	ReleaseDC(hwnd, hdc);
}

void Escribir3(HWND hwnd, HDC hdc)
{
	char buffer[32] = { 0 };
	sprintf_s(buffer, "Puntuación Obtenida: %d/20", aliensComidos);

	RECT rc;
	GetClientRect(hwnd, &rc);
	DrawTextA(hdc, (LPSTR)buffer, -1, &rc, DT_SINGLELINE);
	ReleaseDC(hwnd, hdc);
}

void Escribir4(HWND hwnd, HDC hdc)
{
	char buffer[32] = { 0 };
	sprintf_s(buffer, "Puntuación Obtenida: %d/20", aliensComidos);

	RECT rc;
	GetClientRect(hwnd, &rc);
	DrawTextA(hdc, (LPSTR)buffer, -1, &rc, DT_SINGLELINE);
	ReleaseDC(hwnd, hdc);
}

