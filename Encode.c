#include <stdio.h>
#include <stdlib.h>

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

void Encode(unsigned char*, unsigned char*, unsigned long,unsigned long);
unsigned int GetBit(unsigned char*, unsigned long);

int main()
{
	FILE *fin1, *fin2, *fout;
	BITMAPFILEHEADER *fheader1;
	BITMAPINFOHEADER *iheader1;
	unsigned long filesize1, filesize2;
	unsigned char *buffer1, *buffer2;

	if((fin1 = fopen("Original.bmp", "rb")) == NULL) {
		printf("Error : Opening file...\n");
		return -1;
	}
	if((fin2 = fopen("One.txt", "rb")) == NULL) {
		printf("Error : Opening second file...\n");
		return -1;
	}
	if((fout = fopen("Steganography.bmp", "wb")) == NULL) {
		printf("Error : Opening output file...\n");
		return -1;
	}

	fseek(fin1, 0, SEEK_END);
	fseek(fin2, 0, SEEK_END);
	filesize1 = ftell(fin1);
	filesize2 = ftell(fin2);
	rewind(fin1);
	rewind(fin2);

	filesize1 = filesize1 - 54;

	buffer1 = (unsigned char*) malloc(filesize1);
	buffer2 = (unsigned char*) malloc(filesize2);

	if(filesize1 < (filesize2 * 8)) {
		printf("Error : Size of the image to be hided is greater than the input image / 8.\n");
		return -1;
	}

	/*	Allocate memory to the Pointers	*/
	fheader1 = (BITMAPFILEHEADER*) malloc(sizeof(BITMAPFILEHEADER));  
	iheader1 = (BITMAPINFOHEADER*) calloc(sizeof(BITMAPINFOHEADER), 1);

	fread(&fheader1->type, sizeof(unsigned short), 1, fin1);
	fread(&fheader1->size, sizeof(unsigned long), 1, fin1);
	fread(&fheader1->reserved1, sizeof(unsigned short), 1, fin1);
	fread(&fheader1->reserved2, sizeof(unsigned short), 1, fin1);
	fread(&fheader1->offsetbits, sizeof(unsigned long), 1, fin1);
	fread(iheader1, sizeof(BITMAPINFOHEADER), 1, fin1);

	fread(buffer1, sizeof(unsigned char), filesize1, fin1);
	fread(buffer2, sizeof(unsigned char), filesize2, fin2);

	Encode(buffer1, buffer2, filesize1, filesize2);

	fwrite(&fheader1->type, sizeof(unsigned short), 1, fout);
	fwrite(&fheader1->size, sizeof(unsigned long), 1, fout);
	fwrite(&fheader1->reserved1, sizeof(unsigned short), 1, fout);
	fwrite(&fheader1->reserved2, sizeof(unsigned short), 1, fout);
	fwrite(&fheader1->offsetbits, sizeof(unsigned long), 1, fout);
	fwrite(iheader1, sizeof(BITMAPINFOHEADER), 1, fout);
	fwrite(buffer1, sizeof(unsigned char), filesize1, fout);

	printf("Size of the hided text file = %d\n", filesize2);

	fclose(fin1);
	fclose(fin2);
	fclose(fout);
	return(0); 
}

void Encode(dest, src, fs1, fs2)
unsigned char *dest, *src;
unsigned long fs1, fs2;
{
	unsigned long index;
	unsigned long fromWhere;

	printf("Enter the starting location from where to encode : ");
	scanf("%d", &fromWhere);
	
	for(index = fromWhere; index < (fs2 * 8 + fromWhere); index++) {
		if(GetBit(src, index - fromWhere))
			dest[index] = dest[index] | 0x1;
		else
			dest[index] = dest[index] & 0xfe;
	}

	return;
}

unsigned int GetBit(unsigned char *array, unsigned long bitno)
{
	return (array[bitno / 8] & (1 << (7 - (bitno % 8)))) > 0 ? 1 : 0;
}
