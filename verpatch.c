/*
  gcc -O2 -overpatch -static-libgcc verpatch.c
  strip verpatch.exe
*/

#include <windows.h>
#include <stdio.h>

int main(int argc, char **argv){
	int i=1,r=0;
	if(argc<2){printf("verpatch [programs...]\n");return -1;}
	for(;i<argc;i++){
		HANDLE hfile = CreateFile(argv[i],GENERIC_READ|GENERIC_WRITE,0,0,OPEN_EXISTING,0,NULL);
		printf("patching %s: ",argv[i]);
		if(hfile!=INVALID_HANDLE_VALUE){
			HANDLE hmapping = CreateFileMapping(hfile,0,PAGE_READWRITE,0,0,0);
			if(hmapping){
				BYTE *file_base = (BYTE *)MapViewOfFileEx(hmapping,FILE_MAP_ALL_ACCESS,0,0,0,0);
				if(file_base){
					IMAGE_OPTIONAL_HEADER *ioh = (IMAGE_OPTIONAL_HEADER *)(file_base + ((IMAGE_DOS_HEADER *)file_base)->e_lfanew + 4 + sizeof(IMAGE_FILE_HEADER));
					ioh->MajorOperatingSystemVersion = 4;
					ioh->MinorOperatingSystemVersion = 0;
					ioh->MajorSubsystemVersion = 4;
					ioh->MinorSubsystemVersion = 0;
					UnmapViewOfFile(file_base);
					puts("OK");
				}else
					puts("Failed [Cannot get mapfile address]"),r++;
				CloseHandle(hmapping);
			}else
				puts("Failed [Cannot map file]"),r++;
			CloseHandle(hfile);
		}else
			puts("Failed [Cannot open file]"),r++;
	}
	return r;
}