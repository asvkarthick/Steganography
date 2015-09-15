#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

typedef struct {
	unsigned short type;
	unsigned long size;
	unsigned short reserved1;
	unsigned short reserved2;
	unsigned long offsetbits;
} BITMAPFILEHEADER;

typedef struct {
	unsigned long size;
	unsigned long width;
	unsigned long height;
	unsigned short planes;
	unsigned short bitcount;
	unsigned long compression;
	unsigned long sizeimage;
	long xpelspermeter;
	long ypelspermeter;
	unsigned long colorsused;
	unsigned long colorsimportant;
} BITMAPINFOHEADER;

void Decode(unsigned char*, unsigned char*, unsigned long,unsigned long);
unsigned int GETBIT(unsigned char*, unsigned long);

int main()
{
	FILE *fin, *fout, *ftxt;
	BITMAPFILEHEADER *fheader1;
	BITMAPINFOHEADER *iheader1;
	unsigned long filesize, size, index;
	unsigned char *buffer, *text;

	if((fin = fopen("Steganography.bmp", "rb")) == NULL) {
		printf("Error : Opening input file...\n");
		return -1;
	}
	if((fout = fopen("Decoded.txt", "wb")) == NULL) {
		printf("Error : Opening output file...\n");
		return -1;
	}
	if((ftxt = fopen("Output.txt", "w")) == NULL)
	{
		printf("Error : Opening output file.\n");
		return -1;
	}

	fseek(fin, 0, SEEK_END);
	filesize = ftell(fin);
	rewind(fin);

	filesize = filesize - 54;
	buffer = (unsigned char*) malloc(filesize);

	printf("Enter the hidden text size : ");
	scanf("%d", &size);

	if(filesize < (size * 8)) {
		printf("The size entered is wrong...\n");
		return -1;
	}

	text = (unsigned char*) malloc(size + 1);

	for(index = 0; index < size; index++)
		text[index] = 0;

	fheader1 = (BITMAPFILEHEADER*) malloc(sizeof(BITMAPFILEHEADER));
	iheader1 = (BITMAPINFOHEADER*) malloc(sizeof(BITMAPINFOHEADER));

	fread(&fheader1->type, sizeof(unsigned short), 1, fin);
	fread(&fheader1->size, sizeof(unsigned long), 1, fin);
	fread(&fheader1->reserved1, sizeof(unsigned short), 1, fin);
	fread(&fheader1->reserved2, sizeof(unsigned short), 1, fin);
	fread(&fheader1->offsetbits, sizeof(unsigned long), 1, fin);
	fread(iheader1, sizeof(BITMAPINFOHEADER), 1, fin);

	fread(buffer, sizeof(unsigned char), filesize, fin);

	Decode(text, buffer, filesize, size);

	text[size] = '\0';

	fwrite(text, sizeof(unsigned char), size, fout);
	printf("Hidden text = \n%s\n", text);
	fprintf(ftxt, "%s", text);
	
	fclose(fin);
	fclose(fout);
	fclose(ftxt);
	return 0;
}

void Decode(unsigned char *dest, unsigned char *src, unsigned long fs1, unsigned long fs2)
{
	unsigned long index;
	unsigned long startLocation;
	printf("Enter the starting location : ");
	scanf("%d", &startLocation);
	for(index = 0; index < (fs2 * 8); index++) {
		dest[index / 8] = dest[index / 8] | (GETBIT(src, index + startLocation) << (7 - (index % 8)));
	}
	
	return;
}

unsigned int GETBIT(unsigned char *array, unsigned long bitno)
{
	return array[bitno] & 0x1;
}
