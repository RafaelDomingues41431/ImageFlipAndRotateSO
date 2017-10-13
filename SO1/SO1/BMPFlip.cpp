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

typedef enum FLIP_enum_t {
	VER = 1,
	HOR = 0
};

void yyFlip(CHAR* fileIter) {

	DWORD pixelDataOffset = *(DWORD*)(fileIter + 10);
	WORD bitsPerPixel = *(WORD*)(fileIter + 28);
	DWORD widthOfBitmapInPixels = *(DWORD*)(fileIter + 18);
	DWORD heightOfBitmapInPixels = *(DWORD*)(fileIter + 22);

	CHAR* pixelDataBase = fileIter + pixelDataOffset;

	DWORD bytesPerPixel = (bitsPerPixel / 8);

	//offset from the start of the pixel data
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

		while (leftPixelIter < rightPixelIter) {

			DWORD leftAux = *(DWORD*)leftPixelIter & pixelReadMask;
			DWORD rightAux = *(DWORD*)rightPixelIter & pixelReadMask;

			*(DWORD*)leftPixelIter = (*(DWORD*)leftPixelIter & pixelWriteMask) | rightAux;
			*(DWORD*)rightPixelIter = (*(DWORD*)rightPixelIter & pixelWriteMask) | leftAux;

			leftPixelIter += bytesPerPixel;
			rightPixelIter -= bytesPerPixel;	
		}
		rowIter += bytesPerRow;
	}
	printf("Flip executed on yy axis! \n");
}

void xxFlip(CHAR* fileIter) {
	DWORD pixelDataOffset = *(DWORD*)(fileIter + 10);
	WORD bitsPerPixel = *(WORD*)(fileIter + 28);
	DWORD widthOfBitmapInPixels = *(DWORD*)(fileIter + 18);
	DWORD heightOfBitmapInPixels = *(DWORD*)(fileIter + 22);

	CHAR* pixelDataBase = fileIter + pixelDataOffset;

	DWORD bytesPerPixel = (bitsPerPixel / 8);

	DWORD lastPixelInRowOffset = bytesPerPixel * widthOfBitmapInPixels - bytesPerPixel;

	DWORD padding = 0;
	while ((lastPixelInRowOffset + bytesPerPixel + padding) % 4 != 0)
		++padding;

	DWORD bytesPerRow = (lastPixelInRowOffset + bytesPerPixel + padding);
	DWORD pixelReadMask = 0x00FFFFFF;
	DWORD pixelWriteMask = 0xFF000000;

	CHAR* baseRowIter;
	CHAR* topRowIter;

	for (int c = 0; c < widthOfBitmapInPixels; ++c) {
		baseRowIter = pixelDataBase + (c * bytesPerPixel);
		topRowIter = pixelDataBase + bytesPerRow*(heightOfBitmapInPixels - 1) + (c*bytesPerPixel);
		while (baseRowIter < topRowIter) {
			DWORD baseAux = *(DWORD*)baseRowIter & pixelReadMask;
			DWORD topAux = *(DWORD*)topRowIter & pixelReadMask;

			*(DWORD*)baseRowIter = (*(DWORD*)baseRowIter & pixelWriteMask) | topAux;
			*(DWORD*)topRowIter = (*(DWORD*)topRowIter & pixelWriteMask) | baseAux;

			baseRowIter += bytesPerRow;
			topRowIter -= bytesPerRow;
		}
	}
	printf("Flip executed on xx axis! \n");
}

void bmpRotate(LPCWSTR fileNameIn, LPCWSTR fileNameOut, FLIP_enum_t axis) {

	CopyFile(fileNameIn,fileNameOut,FALSE);
	HANDLE fileHandle = CreateFile(fileNameOut, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, NULL, NULL);
	HANDLE fileMappingHandle = CreateFileMapping(fileHandle, NULL, PAGE_READWRITE, 0, 0, fileNameIn);
	LPVOID fileView = MapViewOfFile(fileMappingHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	
	CHAR* fileIter = (CHAR*)fileView;
	CHAR* bmpIdentifier = (CHAR*)fileIter;
	WORD bmpBitsPerPixel = *(WORD*)(fileIter+28);

	if (*bmpIdentifier != 'B' && *(bmpIdentifier + 1)!= 'M') {
		DeleteFile(fileNameOut);
		printf("Not a .bmp image!");
		return;
	}

	if (bmpBitsPerPixel != 24) {
		DeleteFile(fileNameOut);
		printf("Pixel encoding is not 24bit!");
		return;
	}

	if (axis == HOR)
		xxFlip(fileIter);

	if (axis == VER)
		yyFlip(fileIter);

	UnmapViewOfFile(fileView);
	CloseHandle(fileMappingHandle);
	CloseHandle(fileHandle);
	system("pause");
}

int main() {
	bmpRotate(_T("Blade_Runner_Galileu.bmp"),_T("Blade_Runner_Galileu_Flipped.bmp"),VER);
	return 1;
}

