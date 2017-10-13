#include "stdafx.h"
#include "windows.h"

typedef struct bmpImportantInfo{
	DWORD fileLength;
	DWORD pixelDataOffset;
	DWORD widthOfBitmapInPixels;
	DWORD heightOfBitmapInPixels;
	WORD bitsPerPixel;
	DWORD sizeOfPixelData;
};

void bmpRotate(LPCWSTR fileNameIn, LPCWSTR fileNameOut, LPCWSTR rotation) {
	HANDLE fileHandle = CreateFile(fileNameIn, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, NULL, NULL);
	HANDLE fileMappingHandle = CreateFileMapping(fileHandle, NULL, PAGE_READWRITE, 0, 0, fileNameIn);
	LPVOID fileView = MapViewOfFile(fileMappingHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	
	CHAR* fileIter = (CHAR*)fileView;
	CHAR* bmpIdentifier = (CHAR*)fileIter;
	DWORD fileLength = *(DWORD*)(fileIter + 2);
	DWORD pixelDataOffset = *(DWORD*)(fileIter + 10);
	DWORD widthOfBitmapInPixels = *(DWORD*)(fileIter + 18);
	DWORD heightOfBitmapInPixels = *(DWORD*)(fileIter + 22);
	WORD bitsPerPixel = *(WORD*)(fileIter + 28);
	DWORD compressionMode = *(DWORD*)(fileIter + 30);

	//start of pixel data
	CHAR* pixelDataBase = fileIter + pixelDataOffset;

	//number of bytes used in representing a pixel
	DWORD bytesPerPixel = (bitsPerPixel / 8);

	//offset from the first pixel
	DWORD lastPixelInRowOffset = bytesPerPixel * widthOfBitmapInPixels - bytesPerPixel;

	DWORD padding = 0;
	while ((lastPixelInRowOffset + bytesPerPixel + padding) % 4 != 0)
		++padding;

	DWORD bytesPerRow = (lastPixelInRowOffset + bytesPerPixel + padding);
	DWORD pixelReadMask = 0x00FFFFFF;
	DWORD pixelWriteMask = 0xFF000000;

	CHAR* rowIter = pixelDataBase;
	
	for (int l = 0; l < heightOfBitmapInPixels; ++l) {
		CHAR* leftPixelIter = rowIter;
		CHAR* rightPixelIter = rowIter + lastPixelInRowOffset;

		while(leftPixelIter < rightPixelIter) {

			DWORD leftAux = *(DWORD*)leftPixelIter & pixelReadMask;
			DWORD rightAux = *(DWORD*)rightPixelIter & pixelReadMask;

			*(DWORD*)leftPixelIter = (*(DWORD*)leftPixelIter & pixelWriteMask) | rightAux;
			*(DWORD*)rightPixelIter = (*(DWORD*)rightPixelIter & pixelWriteMask) | leftAux;

			leftPixelIter += bytesPerPixel;
			rightPixelIter -= bytesPerPixel;

		}

		rowIter += bytesPerRow;
	}

	system("pause");
}

int main() {
	bmpRotate(_T("Blade_Runner_Galileu.bmp"),_T("Blade_Runner_Galileu_flipped.bmp"),_T("vertical"));
	return 1;
}

