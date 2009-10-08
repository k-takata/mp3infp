#include "stdafx.h"

#include <windows.h>
#include <stdio.h>
#include <winioctl.h>						 // Windows NT IOCTL codes
//#define Not_VxD
//#include <vwin32.h>							// Windows 95 IOCTL codes

/*
	The <vwin32.h> header file is part of the Windows 95 DDK.
	If you don't have the Windows 95 DDK, then use the definitions
	below.
*/ 

#ifndef VWIN32_DIOC_DOS_IOCTL
#define VWIN32_DIOC_DOS_IOCTL 1

typedef struct _DIOC_REGISTERS {
	DWORD reg_EBX;
	DWORD reg_EDX;
	DWORD reg_ECX;
	DWORD reg_EAX;
	DWORD reg_EDI;
	DWORD reg_ESI;
	DWORD reg_Flags;
}DIOC_REGISTERS, *PDIOC_REGISTERS;

#endif

// Intel x86 processor status fla
#define CARRY_FLAG  0x1


#pragma pack(1)
typedef struct _DOSDPB {
	BYTE	 specialFunc;	 // 
	BYTE	 devType;		  // 
	WORD	 devAttr;		  // 
	WORD	 cCyl;			  // number of cylinders
	BYTE	 mediaType;		// 
	WORD	 cbSec;			 // Bytes per sector
	BYTE	 secPerClus;	  // Sectors per cluster
	WORD	 cSecRes;		  // Reserved sectors
	BYTE	 cFAT;			  // FATs
	WORD	 cDir;			  // Root Directory Entries
	WORD	 cSec;			  // Total number of sectors in image
	BYTE	 bMedia;			// Media descriptor
	WORD	 secPerFAT;		// Sectors per FAT
	WORD	 secPerTrack;	 // Sectors per track
	WORD	 cHead;			 // Heads
	DWORD	cSecHidden;	  // Hidden sectors
	DWORD	cTotalSectors;  // Total sectors, if cbSec is zero
	BYTE	 reserved[6];	 // 
} DOSDPB, *PDOSDPB;
#pragma pack()


/*
	GetDriveFormFactor returns the drive form factor.

	It returns 350 if the drive is a 3.5" floppy drive.
	It returns 525 if the drive is a 5.25" floppy drive.
	It returns 800 if the drive is a 8" floppy drive.
	It returns	0 on error.
	It returns	1 if the drive is a hard drive.
	It returns	2 if the drive is something else.

	iDrive is 1 for drive A:, 2 for drive B:, etc.

	Note that iDrive is not range-checked or otherwise validated.
	It is the responsibility of the caller only to pass values for
	iDrive between 1 and 26 (inclusive).

	Although the function, as written, returns only the drive form
	factor, it can easily be extended to return other types of
	drive information.
*/ 

DWORD GetDriveFormFactor(int iDrive)
{
	 HANDLE h;
	 TCHAR tsz[8];
	 DWORD dwRc;

	 if ((int)GetVersion() < 0)
	 {	// Windows 95

		 /*
			 On Windows 95, use the technique described in
			 the Knowledge Base article Q125712 and in MSDN under
			 "Windows 95 Guide to Programming", "Using Windows 95
			 features", "Using VWIN32 to Carry Out MS-DOS Functions".
		 */ 
		 h = CreateFileA("\\\\.\\VWIN32", 0, 0, 0, 0,
								FILE_FLAG_DELETE_ON_CLOSE, 0);

		 if (h != INVALID_HANDLE_VALUE)
		 {
			 DWORD			 cb;
			 DIOC_REGISTERS reg;
			 DOSDPB			dpb;

			 dpb.specialFunc = 0;  // return default type; do not hit disk

			 reg.reg_EBX	= iDrive;		 // BL = drive number (1-based)
			 reg.reg_EDX	= (DWORD)&dpb;  // DS:EDX -> DPB
			 reg.reg_ECX	= 0x0860;		 // CX = Get DPB
			 reg.reg_EAX	= 0x440D;		 // AX = Ioctl
			 reg.reg_Flags = CARRY_FLAG;	// assume failure

			 // Make sure both DeviceIoControl and Int 21h succeeded.
			 if (DeviceIoControl (h, VWIN32_DIOC_DOS_IOCTL, &reg,
										 sizeof(reg), &reg, sizeof(reg),
										 &cb, 0)
				  && !(reg.reg_Flags & CARRY_FLAG))
			 {
				 switch (dpb.devType)
				 {
				 case 0: // 5.25 360K floppy
				 case 1: // 5.25 1.2MB floppy
					 dwRc = 525; break;

				 case 2: // 3.5  720K floppy
				 case 7: // 3.5  1.44MB floppy
				 case 9: // 3.5  2.88MB floppy
					 dwRc = 350; break;

				 case 3: // 8" low-density floppy
				 case 4: // 8" high-density floppy
					 dwRc = 800; break;

				 case 5: // hard drive
					 dwRc = 1; break;

				 case 6: // tape drive
				 case 8: // optical disk
					 dwRc = 2; break;

				 default: // unknown
					 dwRc = 0; break;
				 }
			 }
			 else
				 dwRc = 0;


			 CloseHandle(h);
		 }
		 else
			 dwRc = 0;

	}
	else
	{  // Windows NT

		/*
			On Windows NT, use the technique described in the Knowledge
			Base article Q115828 and in the "FLOPPY" SDK sample.
		*/ 
		wsprintf(tsz, TEXT("\\\\.\\%c:"), TEXT('@') + iDrive);
		h = CreateFile(tsz, 0, FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
		if (h != INVALID_HANDLE_VALUE)
		{
			DISK_GEOMETRY Geom[20];
			DWORD cb;

			if (DeviceIoControl (h, IOCTL_DISK_GET_MEDIA_TYPES, 0, 0,
										Geom, sizeof(Geom), &cb, 0)
				 && cb > 0)
			{
				switch (Geom[0].MediaType)
				{
				case F5_1Pt2_512: // 5.25 1.2MB floppy
				case F5_360_512:  // 5.25 360K  floppy
				case F5_320_512:  // 5.25 320K  floppy
				case F5_320_1024: // 5.25 320K  floppy
				case F5_180_512:  // 5.25 180K  floppy
				case F5_160_512:  // 5.25 160K  floppy
					dwRc = 525;
					break;

				case F3_1Pt44_512: // 3.5 1.44MB floppy
				case F3_2Pt88_512: // 3.5 2.88MB floppy
				case F3_20Pt8_512: // 3.5 20.8MB floppy
				case F3_720_512:	// 3.5 720K	floppy
					dwRc = 350;
					break;

				case RemovableMedia:
					dwRc = 2;
					break;

				case FixedMedia:
					dwRc = 1;
					break;

				default:
					dwRc = 0;
					break;
				}
			}
			else
				dwRc = 0;

			CloseHandle(h);
		}
		else
			dwRc = 0;
	}

	/*
		If you are unable to determine the drive type via ioctls,
		then it means one of the following:

		1.  It is hard drive and we do not have administrator privilege
		2.  It is a network drive
		3.  The drive letter is invalid

		GetDriveType can distinguish these three cases.
	*/ 
	if (dwRc == 0)
	{
		wsprintf(tsz, TEXT("%c:\\"), TEXT('@') + iDrive);

		switch (GetDriveType(tsz))
		{
			case DRIVE_FIXED:
				dwRc = 1;
				break;
			case DRIVE_REMOVABLE:
			case DRIVE_REMOTE:
			case DRIVE_CDROM:
			case DRIVE_RAMDISK:
				dwRc = 2;
				break;
			default:
				dwRc = 0;
		}
	}
	return dwRc;
}


/*
	Sample program:  Count from A to Z and display the drive
	form factor for each drive that exists.
*/ 
/*
int main(int argc, char **argv)
{
	int iDrive;

	for (iDrive = 1; iDrive <= 26; ++iDrive)
	{
		DWORD dwFF = GetDriveFormFactor(iDrive);

		if (dwFF)
			printf("%c: = %d\n", TEXT('@') + iDrive, dwFF);
	}
	return 0;
} 


*/