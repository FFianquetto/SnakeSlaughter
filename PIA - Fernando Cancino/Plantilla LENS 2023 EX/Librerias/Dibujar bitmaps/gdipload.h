#include<Windows.h>
#include<GdiPlus.h>

using namespace Gdiplus;

struct miBitmap{

	int ancho;
	int alto;

	int *pixeles;

};

miBitmap gdipLoad(WCHAR nombre[])
{
	miBitmap mbmp;

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR  gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	Bitmap *bitmap=new Bitmap(nombre);
	BitmapData *bitmapData=new BitmapData;

	mbmp.ancho = bitmap->GetWidth();
	mbmp.alto = bitmap->GetHeight();

	Rect rect(0, 0, mbmp.ancho, mbmp.alto);

	bitmap->LockBits(&rect, ImageLockModeRead, PixelFormat32bppRGB, bitmapData);

	int* pixels = (int*)bitmapData->Scan0;

	mbmp.pixeles = new int[mbmp.alto*mbmp.ancho];

	for(int i=0, j=mbmp.alto*mbmp.ancho;i<j;i++)
	{
		mbmp.pixeles[i]=pixels[i];
	}
	bitmap->UnlockBits(bitmapData);

	delete bitmapData;
	delete bitmap;
	  
	GdiplusShutdown(gdiplusToken);

	return mbmp;
}