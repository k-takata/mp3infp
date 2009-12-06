#include "stdafx.h"

#include "wa_ipc.h"
#include "ipc_pe.h"
#include "winampcmd.h"
#include "resource.h"		// メイン シンボル
#include "GlobalCommand.h"

#include <Mmsystem.h>
#include <mmreg.h>
#include <Msacm.h>
#include <Vfw.h>
#define sizeof_array(a) (sizeof(a)/sizeof(a[0]))

#define INITGUID
#include <Ks.h>
#include <KsMedia.h>

#pragma comment(lib,"msacm32.lib")
#pragma comment(lib,"vfw32.lib")
#pragma comment(lib,"winmm.lib")

typedef struct _AUDIO_CODEC_TYPE{
	DWORD dwType;
	LPCTSTR format;
}AUDIO_CODEC_TYPE;

typedef struct _VIDEO_CODEC_TYPE{
	char *type;
	LPCTSTR format;
}VIDEO_CODEC_TYPE;

/*
	コーデック情報は"真空波動研"(http://www.kurohane.net/)の黒羽さん提供していただきました。2002-09-16
*/

AUDIO_CODEC_TYPE audio_codecs[] =
	{
		{0x0000,_T("Unknown")},
		{0x0001,_T("Microsoft PCM")},
		{0x0002,_T("Microsoft ADPCM")},
		{0x0003,_T("IEEE Float")},
		{0x0004,_T("Compaq Computer's VSELP")},
		{0x0005,_T("IBM CVSD")},
		{0x0006,_T("CCITT A-Law")},
		{0x0007,_T("CCITT u-Law")},
		{0x0008,_T("Microsoft DTS")},
		{0x0009,_T("Microsoft DRM")},
		{0x000A,_T("Windows Media Audio 9 Voice")},
		{0x0010,_T("OKI ADPCM")},
		{0x0011,_T("IMA ADPCM")},
		{0x0012,_T("MediaSpace ADPCM")},
		{0x0013,_T("Sierra ADPCM")},
		{0x0014,_T("CCITT G.723 ADPCM")},
		{0x0015,_T("DSP Group DigiSTD")},
		{0x0016,_T("DSP Group DigiFIX")},
		{0x0017,_T("Dialogic OKI ADPCM")},
		{0x0018,_T("MediaVision ADPCM")},
		{0x0019,_T("HP CU")},
		{0x0020,_T("YAMAHA ADPCM")},
		{0x0021,_T("Sonarc(TM) Compression")},
		{0x0022,_T("DSP Group TrueSpeech(TM)")},
		{0x0023,_T("Echo Speech")},
		{0x0024,_T("AUDIOFILE AF36")},
		{0x0025,_T("Audio Processing Technology")},
		{0x0026,_T("AUDIOFILE AF10")},
		{0x0027,_T("Prosody 1612")},
		{0x0028,_T("LRC")},
		{0x0030,_T("Dolby AC-2")},
		{0x0031,_T("GSM 6.10")},
		{0x0032,_T("MSNAudio")},
		{0x0033,_T("ADPCME")},
		{0x0034,_T("Control Resources Limited VQLPC")},
		{0x0035,_T("DSP Group REAL")},
		{0x0036,_T("DSP Group ADPCM")},
		{0x0037,_T("Control Resources Limited CR10")},
		{0x0038,_T("NMS VBXADPCM")},
		{0x0039,_T("Roland RDAC")},
		{0x003a,_T("EchoSC3")},
		{0x003b,_T("Rockwell ADPCM")},
		{0x003c,_T("Rockwell Digit LK")},
		{0x003d,_T("Xebec")},
		{0x0040,_T("G.721 ADPCM")},
		{0x0041,_T("G.728 CELP")},
		{0x0042,_T("MS G.723")},
		{0x0043,_T("IBM AVC ADPCM")},
		{0x0045,_T("SHARP G.726")},
		{0x004f,_T("Ogg Vorbis(mode1)")},
		{0x0050,_T("MPEG-1 layer 1, 2")},
	//選択	{0x0050,_T("Ogg Vorbis(mode2)")},
		{0x0051,_T("Ogg Vorbis(mode3)")},
		{0x0052,_T("RT24")},
		{0x0053,_T("PAC")},
		{0x0055,_T("MPEG1-Layer3")},
		{0x0059,_T("Lucent G.723")},
		{0x0060,_T("Cirrus")},
		{0x0061,_T("ESPCM")},
		{0x0062,_T("Voxware")},
		{0x0063,_T("Canopus Atrac")},
		{0x0064,_T("G.726 ADPCM")},
		{0x0065,_T("G.722 ADPCM")},
		{0x0066,_T("DSAT")},
		{0x0067,_T("DSAT Display")},
		{0x0069,_T("Voxware Byte Aligned")},
		{0x006f,_T("Ogg Vorbis(mode1+)")},
		{0x0070,_T("Ogg Vorbis(mode2+)")},
	//選択		{0x0070,_T("Voxware AC8")},
		{0x0071,_T("Ogg Vorbis(mode3+)")},
	//選択		{0x0071,_T("Voxware AC10")},
		{0x0072,_T("Voxware AC16")},
		{0x0073,_T("Voxware AC20")},
		{0x0074,_T("Voxware MetaVoice")},
		{0x0075,_T("Voxware MetaSound")},
		{0x0076,_T("Voxware RT29HW")},
		{0x0077,_T("Voxware VR12")},
		{0x0078,_T("Voxware VR18")},
		{0x0079,_T("Voxware TQ40")},
		{0x0080,_T("Softsound")},
		{0x0081,_T("Voxware TQ60")},
		{0x0082,_T("MSRT24")},
		{0x0083,_T("G.729A")},
		{0x0084,_T("MVI MV12")},
		{0x0085,_T("DF G.726")},
		{0x0086,_T("DF GSM610")},
		{0x0088,_T("ISIAudio")},
		{0x0089,_T("Onlive")},
		{0x0091,_T("Siemens SBC24")},
		{0x0092,_T("Dolby AC3 SPDIF")},
		{0x0097,_T("ZyXEL ADPCM")},
		{0x0098,_T("Philips LPCBB")},
		{0x0099,_T("Packed")},
		{0x00A0,_T("Malden Electronics PHONYTALK")},
		{0x00e1,_T("Microsoft ADPCM")},
		{0x00FF,_T("Advanced Audio Coding")},
		{0x0100,_T("Rhetorex ADPCM")},
		{0x0101,_T("BeCubed Software's IRAT")},
		{0x0111,_T("Vivo G.723")},
		{0x0112,_T("Vivo Siren")},
		{0x0123,_T("Digital G.723")},
		{0x0125,_T("Sanyo ADPCM")},
		{0x0130,_T("ACELP.net Sipro Lab Audio")},
		{0x0131,_T("Sipro Lab Telecom ACELP.4800")},
		{0x0132,_T("Sipro Lab Telecom ACELP.8V3")},
		{0x0133,_T("Sipro Lab Telecom ACELP.G.729")},
		{0x0134,_T("Sipro Lab Telecom ACELP.G.729A")},
		{0x0135,_T("Sipro Lab Telecom ACELP.KELVIN")},
		{0x0140,_T("Dictaphone G.726 ADPCM")},
		{0x0150,_T("Qualcomm PUREVOICE")},
		{0x0151,_T("Qualcomm HALFRATE")},
		{0x0155,_T("Ring Zero Systems TUBGSM")},
		{0x0160,_T("Windows Media Audio1")},
		{0x0161,_T("DivX Audio(WMA)")},
		{0x0162,_T("Windows Media Audio9 Professional")},
		{0x0163,_T("Windows Media Audio9 Lossless")},
		{0x0170,_T("UNISYS NAP ADPCM")},
		{0x0171,_T("UNISYS NAP ULAW")},
		{0x0172,_T("UNISYS NAP ALAW")},
		{0x0173,_T("UNISYS NAP 16K")},
		{0x0200,_T("Creative ADPCM")},
		{0x0202,_T("Creative FastSpeech8")},
		{0x0203,_T("Creative FastSpeech10")},
		{0x0220,_T("Quarterdeck")},
		{0x0210,_T("UHER informatic GmbH ADPCM")},
		{0x0215,_T("Ulead DV ACM")},
		{0x0216,_T("Ulead DV ACM")},
		{0x0220,_T("Quarterdeck")},
		{0x0300,_T("Fujitsu FM-TOWNS SND")},
		{0x0230,_T("I-link Worldwide ILINK VC")},
		{0x0240,_T("Aureal Semiconductor RAW SPORT")},
		{0x0241,_T("ESST AC3")},
		{0x0250,_T("Interactive Products HSX")},
		{0x0251,_T("Interactive Products RPELP")},
		{0x0260,_T("Consistent Software CS2")},
		{0x0270,_T("Sony ATRAC3 (SCX, same as MiniDisk LP2)")},
		{0x0400,_T("BTV Digital")},
		{0x0401,_T("Intel Music Coder")},
		{0x0402,_T("Ligos Indeo Audio")},
		{0x0450,_T("QDesign Music")},
		{0x0680,_T("AT&T Labs VME VMPCM")},
		{0x0681,_T("AT&T Labs TPC")},
		{0x1000,_T("Olivetti GSM")},
		{0x1001,_T("Olivetti ADPCM")},
		{0x1002,_T("Olivetti CELP")},
		{0x1003,_T("Olivetti SBC")},
		{0x1004,_T("Olivetti OPR")},
		{0x1100,_T("LH Codec")},
		{0x1101,_T("Lernout & Hauspie CELP codec")},
		{0x1102,_T("Lernout & Hauspie SBC codec")},
		{0x1103,_T("Lernout & Hauspie SBC codec")},
		{0x1104,_T("Lernout & Hauspie SBC codec")},
		{0x1400,_T("Norris")},
		{0x1401,_T("ISIAudio")},
		{0x1500,_T("Soundspace Music Compression")},
		{0x2000,_T("FAST Multimedia AG DVM (Dolby AC3)")},
		{0x181C,_T("VoxWare RT24 speech codec")},
		{0x181E,_T("Lucent elemedia AX24000P Music codec")},
		{0x1C07,_T("Lucent SX8300P speech codec")},
		{0x1C0C,_T("Lucent SX5363S G.723 compliant codec")},
		{0x1F03,_T("CUseeMe DigiTalk (ex-Rocwell)")},
		{0x1FC4,_T("NTC ALF2CD ACM")},
		{0x2001,_T("DTS")},
		{0x2002,_T("RealAudio?(14_4)")},
		{0x2003,_T("RealAudio?(28_8)")},
		{0x2004,_T("RealAudio?(COOK)")},
		{0x2005,_T("RealAudio?(DNET)")},
		{0x674f,_T("Ogg Vorbis (mode 1)")},
		{0x6750,_T("Ogg Vorbis (mode 2)")},
		{0x6751,_T("Ogg Vorbis (mode 3)")},
		{0x676f,_T("Ogg Vorbis (mode 1+)")},
		{0x6770,_T("Ogg Vorbis (mode 2+)")},
		{0x6771,_T("Ogg Vorbis (mode 3+)")},
		{0x7A21,_T("GSM-AMR (CBR no SID)")},
		{0x7A22,_T("GSM-AMR (VBR including SID)")},
		{0x77A1,_T("The True Audio")},
		{0xC0CC,_T("GigaLink Audio Codec")},
		{0xDFAC,_T("DebugMode SonicFoundry Vegas FrameServer ACM Codec")},
		{0xF1AC,_T("CoreFLAC ACM")},
		{0xFFFE,_T("Extensible wave format")},
		{0xFFFF,_T("In Development / Unregistered")}
	};

VIDEO_CODEC_TYPE divx_vers[] =
{
		{"DivX999b000",_T("XviD MPEG4")},
		{"DivX500Build413",_T("DivX 5.0.0")},
		{"DivX501b413",_T("DivX 5.0.0")}, //Unconfirmed
		{"DivX501b450",_T("DivX 5.0.1")},
		{"DivX501b481",_T("DivX 5.0.2")},
		{"DivX501b484",_T("DivX 5.0.2")},
		{"DivX501b487",_T("DivX 5.0.2")},
		{"DivX503b688",_T("DivX 5.0.3 Beta")},
		{"DivX503b696",_T("DivX 5.0.3 Beta")},	// 2004-07-21 (自信なし)
		{"DivX503b740",_T("DivX 5.0.3")},
		{"DivX503b795",_T("DivX 5.0.4 Beta1-2")},
		{"DivX503b804",_T("DivX 5.0.4 Beta3")},
		{"DivX503b814",_T("DivX 5.0.4 Beta4")},
		{"DivX503b822",_T("DivX 5.0.4")},
		{"DivX503b830",_T("DivX 5.0.5.830")},
		{"DivX503b894",_T("DivX 5.0.5 Kauehi")},
		{"DivX503b922",_T("DivX 5.1.0 Beta1")},
		{"DivX503b936",_T("DivX 5.1.0 Beta2")},
		{"DivX503b959",_T("DivX 5.1.0.959")},// 2003-09-05
		{"DivX503b0985",_T("DivX 5.1(DivX HD)")},
		{"DivX503b1009",_T("DivX 5.1.1 Beta1")},
		{"DivX503b1025",_T("DivX 5.1.1 Beta2")},
		{"DivX503b1031",_T("DivX 5.1.1.1031")},// 2003-11-24
		{"DivX503b1263",_T("DivX 5.2.0")},// 2004-07-19
		{"DivX503b1272",_T("DivX 5.2.0(Dr.DivX 1.0.5)")},
		{"DivX503b1307",_T("DivX 5.2.1 Alpha(Dr.DivX)")},
		{"DivX503b1314",_T("DivX Pro 5.2.1 Codec (X-Transcoder)")},	// 2006-11-13
		{"DivX503b1328",_T("DivX 5.2.1")},	// 2004-09-24
		{"DivX503b1338",_T("DivX 5.2.1(Dr.DivX 1.0.6)")},	// 2004-11-07
		{"DivX503b1394",_T("DivX Pro Plasma Codec Ver.5.3.0 Build 1394")},
		{"DivX503b1408",_T("DivX Pro Plasma Codec Ver.5.3.0 Build 1408")},
		{"DivX503b1453",_T("DivX Pro Fusion Beta Build 1453 (DivX Ver.5.9)")},
		{"DivX503b1461",_T("DivX Pro Fusion Beta Build 1461 (DivX Ver.5.9)")},
		{"DivX503b1528",_T("DivX 5.9 Fusion")},
		{"DivX503b1571",_T("DivX 6.0.0")},	// 2005-07-08 BBS7592
		{"DivX503b1594",_T("DivX 6.0.0 (DivX Converter 1.0)")},
		{"DivX503b1599",_T("DivX 6 Helium")},
		{"DivX503b1612",_T("DivX 6 Helium")},
		{"DivX503b1670",_T("Dr.DivX 1.07")},
		{"DivX503b1697",_T("DivX 6.0.3")},	// 2005-12-04 BBS8167
		{"DivX503b1737",_T("DivX He-3")},
		{"DivX503b1786",_T("DivX 6.1.0")},		// 2006-03-12 BBS8433
		{"DivX503b1807",_T("DivX 6.1.0 Patch 1 Beta")},
		{"DivX503b1814",_T("DivX 6.1.0 Patch 2 Beta")},
		{"DivX503b1828",_T("DivX 6.1.1")},	// 2006-03-12 BBS8433
		{"DivX503b1838",_T("DivX 6 (TMPGEnc)")},	// 2007-07-17 BBS8655
		{"DivX503b1893",_T("DivX 6.2.0 Beta 1")},	// 2006-04-01 BBS8463
		{"DivX503b1910",_T("DivX 6.2.0")},	// 2006-04-12
		{"DivX503b1913",_T("DivX 6.2.1")},	// 2006-04-24 BBS8489
		{"DivX503b1915",_T("DivX 6.2.1 Patch 1 Beta")},
		{"DivX503b1920",_T("DivX 6.2.2")},	// 2006-04-24
		{"DivX503b1974",_T("DivX 6.2.5")},	// 2007-07-26 BBS8688 
		{"DivX503b1977",_T("DivX 6.2.5")},	// 2007-07-17 BBS8670
		{"DivX503b1988",_T("DivX 6.2.5 (DivX Converter 6.2.1)")},	// 2007-08-27 BBS8715
		{"DivX503b2081",_T("DivX 6.4 Beta 1")},	// 2007-10-03 BBS8757
		{"DivX503b2086",_T("DivX 6.4")},		// 2007-10-03 BBS8781
		{"DivX503b2151",_T("DivX 6.4 (DivX Author)")},
		{"DivX503b2201",_T("DivX 6.5.0")},		// 2007-08-26 BBS9011
		{"DivX503b2207",_T("DivX6.5.1")},	// 2007-08-26 BBS9019
		{"DivX503b2292",_T("DivX6.6.0")},	// 2007-08-26 BBS9114
		{"DivX503b2306",_T("DivX6.6.1")},	// 2007-08-26 BBS9125
		{"DivX503b2309",_T("DivX 6.6.1")},
		{"DivX503b2318",_T("DivX 6.6.1.4")},
		{"DivX503b2376",_T("DivX 6 (Stage6)")},
		{"DivX503b2396",_T("DivX 6.7 Beta")},
		{"DivX503b2432",_T("DivX 6.7.0.28")},
		{"DivX503b2510",_T("DivX 6.8.0.14")},
		{"DivX503b2521",_T("DivX 6.8.0 (DivX Converter 6.6)")},
		{"DivX503b2559",_T("DivX 6.8.2.6")},
		{"DivX503b2676",_T("DivX 6.8.3.13")},
		{"DivX503b2816",_T("DivX 6.8.5.5")},	//090626 BBS10017
};

VIDEO_CODEC_TYPE xvid_vers[] =
{
		{"XviD0009",_T("XviD 0.9 BitStream 9")},
		{"XviD0011",_T("XviD 0.9 Patch-66")},
		{"XviD0012",_T("XviD 0.9 BitStream 12")},
		{"XviD0016",_T("XviD 1.0 Patch-13")},
		{"XviD0019",_T("XviD 1.0 Patch-47")},
		{"XviD0020",_T("XviD 1.0 Patch-61")},
		{"XviD0021",_T("XviD 1.0.0 Beta1")},
		{"XviD0022",_T("XviD 1.0.0 Beta1.5")},
		{"XviD0023",_T("XviD 1.0.0 Beta2")},
		{"XviD0024",_T("XviD 1.0.0 Beta2.5")},
		{"XviD0025",_T("XviD 1.0.0 Beta3")},
		{"XviD0026",_T("XviD 1.0.0 RC1")},
		{"XviD0027",_T("XviD 1.0.0 RC1b")},
		{"XviD0028",_T("XviD 1.0.0 RC2")},
		{"XviD0029",_T("XviD 1.0.0 RC3")},
		{"XviD0030",_T("XviD 1.0.0 RC4")},
		{"XviD0031",_T("XviD 1.0.0 RC4b")},
		{"XviD0032",_T("XviD 1.0.0 RC4c")},
		{"XviD0033",_T("XviD 1.0.0 RC4d")},
		{"XviD0034",_T("XviD 1.0.0")},
		{"XviD0035",_T("XviD 1.0.1")},
		{"XviD0036",_T("XviD 1.0.2")},
		{"XviD0037",_T("XviD 1.0.3")},
		{"XviD0038",_T("XviD 1.1.0 Beta1")},
		{"XviD0039",_T("XviD 1.1.0 Beta2")},
		{"XviD0040",_T("XviD 1.1.0")},
		{"XviD0041",_T("XviD 1.1.0 Final")},
		{"XviD0042",_T("XviD 1.2.-127")},
		{"XviD0043",_T("XviD 1.2 SMP")},
		{"XviD0044",_T("XviD 1.1 1")},
		{"XviD0045",_T("XviD 1.2.-127")},
		{"XviD0046",_T("XviD 1.1.2 Final")},
		{"XviD0047",_T("XviD 1.1.-127")},
		{"XviD0048",_T("XviD 1.2.-127")},
		{"XviD0049",_T("XviD 1.2.0")},
		{"XviD0050",_T("XviD 1.2.1")},
		{"XviD0055",_T("XviD 1.3.-127")},
};

VIDEO_CODEC_TYPE video_codecs[] =
	{
		//非圧縮
		{"DIB ",_T("NoCompress(DIB)")},
		{"RGB ",_T("NoCompress(RGB)")},
		{"BGR ",_T("NoCompress(RGB32)")},
		{"RAW ",_T("NoCompress(RAW)")},
		//圧縮
		{"3IVX",_T("3ivx Delta x.x")},
		{"3IVD",_T("DivX3/MS MPEG4-V3(3IVD)")},
		{"3IV0",_T("3ivx Delta 1.0-3.5(3IV0)")},
		{"3IV1",_T("3ivx Delta 1.0-3.5(3IV1)")},
		{"3IV2",_T("3ivx Delta 4.0")},
		{"AAS4",_T("Autodesk Animator (AAS4)")},
		{"AASC",_T("Autodesk Animator (AASC)")},
		{"ABYR",_T("Kensington?")},
		{"ADV1",_T("Loronix WaveCodec")},
		{"ADVJ",_T("Avid M-JPEG(ADVJ)")},
		{"AEIK",_T("Intel Indeo 3.2")},
		{"AEMI",_T("Array VideoONE MPEG1-I Capture")},
		{"AFLI",_T("Autodesk Animator(FLI)")},
		{"AFLC",_T("Autodesk Animator(FLC)")},
		{"AJPG",_T("Animated JPEG")}, //Unconfirmed
		{"AMM2",_T("AMV2 MT Codec")},
		{"AMPG",_T("Array VideoONE MPEG")},
		{"AMV3",_T("AMV3 Codec")},
		{"ANIM",_T("Intel RDX(ANIM)")},
		{"AP41",_T("AngelPotion MPEG4-V3 Hacked")},
		{"ASLC",_T("Alparysoft Lossless Video Codec")},
		{"ASV1",_T("Asus Video")},
		{"ASV2",_T("Asus Video(2)")},
		{"ASVX",_T("Asus Video 2.0")},
		{"AUR2",_T("Aura 2 YUV 422")},
		{"AURA",_T("Aura 1 YUV 411")},
		{"AVC1",_T("H.264(AVC1)")},
		{"AVRN",_T("Avid M-JPEG(AVRN)")},
		{"BINK",_T("Bink Video")},
		{"BLZ0",_T("DivX 5.x(BLZ0)")},
		{"BT20",_T("Conexant Prosumer Video")},
		{"BTCV",_T("Conexant Composite Video")},
		{"BW10",_T("Broadway MPEG Capture/Compression")},
		{"CC12",_T("Intel YUV12")},
		{"CDVC",_T("Canopus DV")},
		{"CFCC",_T("DPS Perception")},
		{"CFHD",_T("Adobe Premiere HDV")},
		{"CGDI",_T("Microsoft Camcorder Video")},
		{"CHAM",_T("Winnov Caviara Champagne")},
		{"CJPG",_T("Creative WebCam JPEG")},
		{"CLJR",_T("Cirrus Logic YUV")},
		{"CMYK",_T("Colorgraph 32Bit CMYK")},
		{"COL0",_T("DivX3/MS MPEG4-V3(COL0)")},
		{"COL1",_T("DivX3/MS MPEG4-V3(COL1)")},
		{"CPLA",_T("Weitek YUV 4:2:0")},
		{"CRAM",_T("Microsoft Video 1(CRAM)")},
		{"CVID",_T("Radius Cinepak")},
		{"CWLT",_T("Microsoft Color WLT DIB")},
		{"CYUV",_T("Creative YUV")},
		{"CYUY",_T("ATI YUV")},
		{"D261",_T("DEC H.261")},
		{"D263",_T("DEC H.263")},
		{"DAVC",_T("H.264(DAVC)")},
		{"DCL1",_T("Data Connection Conferencing")},
		{"DIVA",_T("DivA MPEG-4")},

		{"DIVX",_T("DivX4")},	// biCommpress を見てDivx5 or Divx4を確定
		{"DIV2",_T("Microsoft MPEG4-V1/V2")},
		{"DIV3",_T("DivX ;-) MPEG-4 Video Codec (low motion)")},
		{"DIV4",_T("DivX ;-) MPEG-4 Video Codec (fast motion)")},
		{"DIV5",_T("DivX5 (DIV5)")},
		{"DIV6",_T("DivX (DIV6)")},
		
		{"DVIS",_T("DV Codec \"Iris\"")},	// 2004-03-13
		{"DVSD",_T("DV Codec \"Iris\" [vdsd]")},	// 2004-03-13
		{"DVRS",_T("DV Codec \"Iris\" Reference AVI")},	// 2004-03-13

		{"DM4V",_T("Dicas MPEGable MPEG-4")},
		{"DMB1",_T("Matrox Rainbow Runner MJPEG")},
		{"DMB2",_T("Paradigm MJPG?")},
		{"DMK2",_T("ViewSonic V36")},
		{"DOHC",_T("DummyCodec")},
		{"DP02",_T("DynaPel MPEG-4")},
		{"DPS0",_T("DPS Reality")},
		{"DPSC",_T("DPS PAR")},
		{"DSVD",_T("DV Codec(DSVD)")},
		{"DV25",_T("Matrox DVCPRO")},
		{"DV50",_T("Matrox DVCPRO50")},
		{"DVAN",_T("?(DVAN)")},
		{"DVC ",_T("DV Codec(DVC )")},
		{"DVCP",_T("DV Codec(DVCP)")},
		{"DVCS",_T("DV Codec(DVCS)")},
		{"DVE2",_T("Insoft DVE-2 Videoconferencing")},
		{"DVH1",_T("Panasonic SMPTE 370M")},
		{"DVHD",_T("DV Codec(DVHD)")},
		{"DVMA",_T("Darim Vision DVMPEG")},
		{"DVSL",_T("DV Codec(DVSL)")},
		{"DUCK",_T("Duck TrueMotion 1.0")},
		{"DVX1",_T("Lucent DVX1000SP")},
		{"DVX2",_T("Lucent DVX2000S")},
		{"DVX3",_T("Lucent DVX3000S")},
		{"DXGM",_T("EA/CinemaWare Game Movie")},
		{"DXT1",_T("DirectX Compressed Texture(DXT1)")},
		{"DXT2",_T("DirectX Compressed Texture(DXT2)")},
		{"DXT3",_T("DirectX Compressed Texture(DXT3)")},
		{"DXT4",_T("DirectX Compressed Texture(DXT4)")},
		{"DXT5",_T("DirectX Compressed Texture(DXT5)")},
		
		{"DX50",_T("DivX5")},	//
		
		{"DXTN",_T("DirectX Compressed Texture")},
		{"DXTC",_T("DirectX Texture Compression")},
		{"ELK0",_T("Elsa YUV(ELK0)")},
		{"EKQ0",_T("Elsa YUV(EKQ0)")},
		{"EM2V",_T("Etymonix MPEG-2 I-frame")},
		{"ESCP",_T("Eidos Escape")},
		{"ETV1",_T("eTreppid Video(ETV1)")},
		{"ETV2",_T("eTreppid Video(ETV2)")},
		{"ETVC",_T("eTreppid Video(ETVC)")},
		{"FFV1",_T("FFMPEG")},
		{"FLJP",_T("D-Vision Field Encoded Motion JPEG")},
		{"FMP4",_T("FFmpeg MPEG4")},
		{"FPS1",_T("Fraps Movie Capture")},
		{"FRWA",_T("SoftLab-Nsk Forward Motion JPEG with alpha channel")},
		{"FRWD",_T("SoftLab-Nsk Forward Motion JPEG(FRWD)")},
		{"FRWT",_T("SoftLab-Nsk Forward Motion JPEG(FRWT)")},
		{"FRWU",_T("Darim Vision Forward")},
		{"FVF1",_T("Fractal Video Frame")},
		{"FVFW",_T("FFVFW")},
		{"GLCC",_T("GigaLink Video Codec")},
		{"GLZW",_T("Motion LZW")},
		{"GPEG",_T("Motion JPEG(GPEG)")},
		{"GWLT",_T("Microsoft Greyscale WLT DIB")},
		{"H260",_T("Intel ITU H.260")},
		{"H261",_T("Intel ITU H.261")},
		{"H262",_T("Intel ITU H.262")},
		{"H263",_T("Intel ITU H.263")},
		{"H264",_T("Intel ITU H.264")},
		{"H265",_T("Intel ITU H.265")},
		{"H266",_T("Intel ITU H.266")},
		{"H267",_T("Intel ITU H.267")},
		{"H268",_T("Intel ITU H.268")},
		{"H269",_T("Intel ITU H.269")},
		{"HFYU",_T("Ben Rudiak-Gould Huffyuv")},
		{"HMCR",_T("Rendition Motion Compensation Format")},
		{"HMRR",_T("Rendition Motion Compensation Format")},
		{"I263",_T("Intel ITU H.263")},
		{"I420",_T("Intel Indeo 4")},
		{"IAN ",_T("Intel RDX(IAN)")},
		{"ICLB",_T("InSoft CellB Videoconferencing")},
		{"IF09",_T("Intel Intermediate YUV9")},
		{"IGOR",_T("Power DVD")},
		{"IJPG",_T("Intergraph JPEG")},
		{"ILVC",_T("Intel Layered Video")},
		{"ILVR",_T("ITU H.263+")},
		{"IPDV",_T("I/O DATA Giga AVI DV")},
		{"IPJ2",_T("Image Power JPEG2000")},
		{"IR21",_T("Intel Indeo 2.1")},
		{"IRAW",_T("Intel Uncompressed UYUV")},
		{"IR45",_T("?(IR45)")},
		{"IV30",_T("Intel Indeo 3")},
		{"IV31",_T("Intel Indeo 3.1")},
		{"IV32",_T("Intel Indeo 3.2")},
		{"IV33",_T("Intel Indeo 3.3")},
		{"IV34",_T("Intel Indeo 3.4")},
		{"IV35",_T("Intel Indeo 3.5")},
		{"IV36",_T("Intel Indeo 3.6")},
		{"IV37",_T("Intel Indeo 3.7")},
		{"IV38",_T("Intel Indeo 3.8")},
		{"IV39",_T("Intel Indeo 3.9")},
		{"IV40",_T("Intel Indeo 4.0")},
		{"IV41",_T("Intel Indeo 4.1")},
		{"IV42",_T("Intel Indeo 4.2")},
		{"IV43",_T("Intel Indeo 4.3")},
		{"IV44",_T("Intel Indeo 4.4")},
		{"IV45",_T("Intel Indeo 4.5")},
		{"IV46",_T("Intel Indeo 4.6")},
		{"IV47",_T("Intel Indeo 4.7")},
		{"IV48",_T("Intel Indeo 4.8")},
		{"IV49",_T("Intel Indeo 4.9")},
		{"IV50",_T("Intel Indeo 5.0")},
		{"IYUV",_T("Intel Indeo iYUV R2.0")},
		{"JBYR",_T("Kensington?")},
		{"JPEG",_T("Microsoft StillImage JPEG")},
		{"JPGL",_T("DIVIO JPEG Light")},
		{"KMVC",_T("Karl Morton's Video")},
		{"L261",_T("Lead H.261")},
		{"L263",_T("Lead H.263")},
		{"LBYR",_T("?(LBYR)")},
		{"LCMW",_T("Lead Motion CMW")},
		{"LEAD",_T("LEAD Video")},
		{"LJ2K",_T("Lead JPEG 2000")},
		{"LJPG",_T("LEAD MJPEG")},
		{"LGRY",_T("Lead Grayscale Image")},
		{"LSVM",_T("Vianet Lighting Strike Vmail")},
		{"LZO1",_T("Lempel-Ziv-Oberhumer")},
		{"M261",_T("Microsoft H.261")},
		{"M263",_T("Microsoft H.263")},
		{"M4CC",_T("Divio MPEG-4")},
		{"M4S2",_T("MPEG-4 version 2 simple profile")},
		{"MC12",_T("ATI Motion Compensation Format(MC12)")},
		{"MCAM",_T("ATI Motion Compensation Format(MCAM)")},
		{"MJ2C",_T("Motion JPEG2000")},
		{"MJPA",_T("Pinnacle ReelTime MJPG-A Software")},
		{"MJPG",_T("Motion JPEG(MJPG)")},
		{"MMES",_T("Matrox MPEG-2 ES")},
		{"MP2A",_T("Media Excel MPEG-2 Audio")},
		{"MP2T",_T("Media Excel MPEG-2 Transport Stream")},
		{"MP2V",_T("Media Excel MPEG-2 Video")},
		{"MP42",_T("Microsoft MPEG4-V2")},
		{"MP43",_T("Microsoft MPEG4-V3")},
		{"MP4A",_T("Media Excel MPEG-4 Audio")},
		{"MP4S",_T("Microsoft MPEG4(MP4S)")},
		{"MP4T",_T("Media Excel MPEG-4 Transport Stream")},
		{"MP4V",_T("Media Excel MPEG-4 Video")},
		{"MPEG",_T("Chromatic MPEG1 Video I Frame")},
		{"MPG3",_T("DivX3/MS MPEG4-V3(MPG3)")},
		{"MPG4",_T("Microsoft MPEG4-V1")},
		{"MPGI",_T("Sigma Designs MPEG")},
		{"MPNG",_T("PNG images decoder")},
		{"MR16",_T("?(MR16)")},
		{"MRCA",_T("FAST Multimedia Mrcodec")},
		{"MRLE",_T("Microsoft RLE")},
		{"MSVC",_T("Microsoft Video1(MSVC)")},
		{"MSZH",_T("AVImszh")},
		{"MTGA",_T("TGA images decoder")},
		{"MTX1",_T("Matrox(MTX1)")},
		{"MTX2",_T("Matrox(MTX2)")},
		{"MTX3",_T("Matrox(MTX3)")},
		{"MTX4",_T("Matrox(MTX4)")},
		{"MTX5",_T("Matrox(MTX5)")},
		{"MTX6",_T("Matrox(MTX6)")},
		{"MTX7",_T("Matrox(MTX7)")},
		{"MTX8",_T("Matrox(MTX8)")},
		{"MTX9",_T("Matrox(MTX9)")},
		{"MV12",_T("?(MV12)")},
		{"MVI1",_T("Motion Pixels MVI1")},
		{"MVI2",_T("Motion Pixels MVI2")},
		{"MWV1",_T("Aware Motion Wavelets")},
		{"NAVI",_T("NAVI")},
		{"NT00",_T("NewTek LigtWave HDTV")},
		{"NTN1",_T("Nogatech Video Compression 1")},
		{"NTN2",_T("Nogatech Video Compression 2")},
		{"NTZ0",_T("netsuzo")}, //Unconfirmed
		{"NTZO",_T("netsuzo")}, //Unconfirmed
		{"NUV1",_T("NuppelVideo")},
		{"NVDS",_T("nVIDIA GeForce3 Texture(NVDS)")},
		{"NVHS",_T("nVIDIA GeForce3 Texture(NVHS)")},
		{"NVHU",_T("nVIDIA GeForce3 Texture(NVHU)")},
		{"NVS0",_T("nVIDIA GeForce2 GTS Pro Texture(NVS0)")},
		{"NVS1",_T("nVIDIA GeForce2 GTS Pro Texture(NVS1)")},
		{"NVS2",_T("nVIDIA GeForce2 GTS Pro Texture(NVS2)")},
		{"NVS3",_T("nVIDIA GeForce2 GTS Pro Texture(NVS3)")},
		{"NVS4",_T("nVIDIA GeForce2 GTS Pro Texture(NVS4)")},
		{"NVS5",_T("nVIDIA GeForce2 GTS Pro Texture(NVS5)")},
		{"NVT0",_T("nVIDIA GeForce2 GTS Pro Texture(NVT0)")},
		{"NVT1",_T("nVIDIA GeForce2 GTS Pro Texture(NVT1)")},
		{"NVT2",_T("nVIDIA GeForce2 GTS Pro Texture(NVT2)")},
		{"NVT3",_T("nVIDIA GeForce2 GTS Pro Texture(NVT3)")},
		{"NVT4",_T("nVIDIA GeForce2 GTS Pro Texture(NVT4)")},
		{"NVT5",_T("nVIDIA GeForce2 GTS Pro Texture(NVT5)")},
		{"PDVC",_T("I/O DATA DVC")},
		{"PGVV",_T("Radius Video Vision")},
		{"PHMO",_T("IBM Photomotion")},
		{"PIM1",_T("Pegasus Imaging Lossless JPEG(PIM1)")},
		{"PIM2",_T("Pegasus Imaging Lossless JPEG(PIM2)")},
		{"PIMJ",_T("Pegasus Imaging Lossless JPEG(PIMJ)")},
		{"PIXL",_T("Pinnacle Video XL")},
		{"PNG1",_T("CorePNG v8")},	// 2004-01-21 追加 BBS4900
		{"PVEZ",_T("Horizons Technology PowerEZ")},
		{"PVMM",_T("PacketVideo Corporation MPEG-4")},
		{"PVWV",_T("Pegasus Wavelet Compression(PVWV)")},
		{"PVW2",_T("Pegasus Wavelet Compression(PVW2)")},
		{"Q1.0",_T("Q-Team QPEG(Q1.0)")},
		{"Q1.1",_T("Q-Team QPEG(Q1.1)")},
		{"QPEQ",_T("Q-Team QPEG(QPEQ)")},
		{"QPEG",_T("Q-Team QPEG(QPEG)")},
		{"RAVI",_T("GTRON ReferenceAVI(RAVI)")},
		{"RAV_",_T("GTRON ReferenceAVI(RAV_)")},
		{"RGBT",_T("Computer Concepts 32Bits RGB")},
		{"RLE ",_T("Microsoft RLE")},
		{"RLE4",_T("Microsoft RLE4")},
		{"RLE8",_T("Microsoft RLE8")},
		{"RMP4",_T("REALMagic MPEG4")},
		{"ROQV",_T("Id RoQ File Video Decoder")},
		{"RPZA",_T("Apple Video")},
		{"RT21",_T("Intel Real Time Video 2.1")},
		{"RTV0",_T("NewTek VideoToaster")},
		{"RUD0",_T("nico Rududu")},
		{"RUSH",_T("RushCodec")},
		{"RV10",_T("RealVideo 1.0")},
		{"RV20",_T("Real G2")},
		{"RV30",_T("Real 8")},
		{"RV40",_T("Real 9/10")},	//090627
		{"RVX ",_T("intel RDX(RVX)")},
		{"S422",_T("Tekram VideoCap C210 YUV")},
		{"SAN3",_T("DivX 3.11a Copy")},
		{"SDCC",_T("Sun Digital Camera")},
		{"SEDG",_T("Samsung MPEG-4")},
		{"SFMC",_T("CrystalNet Surface Fitting Method")},
		{"SHYU",_T("Huffyuvs v2.1.1")},
		{"SMSC",_T("Radius Proprietary")},
		{"SMSD",_T("Radius Proprietary")},
		{"SMSV",_T("WorldConnect Wavelet Video")},
		{"SNOW",_T("SNOW")},
		{"SP40",_T("Sunplus SP40")},
		{"SP44",_T("Sunplus SP44")},
		{"SP53",_T("Aiptek MegaCam(SP53)")},
		{"SP54",_T("Aiptek MegaCam(SP54)")},
		{"SP55",_T("Aiptek MegaCam(SP55)")},
		{"SP56",_T("Aiptek MegaCam(SP56)")},
		{"SP57",_T("Aiptek MegaCam(SP57)")},
		{"SP58",_T("Aiptek MegaCam(SP58)")},
		{"SPIG",_T("Radius Spigot")},
		{"SPLC",_T("Splash Studios ACM")},
		{"SQZ2",_T("Microsoft VXTreme V2")},
		{"SV10",_T("Sorenson Video R1")},
		{"STVA",_T("ST CMOS Imager Data (Bayer)")},
		{"STVB",_T("ST CMOS Imager Data (Nudged Bayer)")},
		{"STVC",_T("ST CMOS Imager Data (Bunched)")},
		{"STVX",_T("ST CMOS Imager Data (Extended CODEC Data Format)")},
		{"STVY",_T("ST CMOS Imager Data (Extended CODEC Data Format with Correction Data)")},
		{"SVQ1",_T("Sorenson Video")},
		{"TLMS",_T("TeraLogic Motion Intraframe(TLMS)")},
		{"TLST",_T("TeraLogic Motion Intraframe(TLST)")},
		{"TM20",_T("Duck TrueMotion 2.0")},
		{"TM2X",_T("Duck TrueMotion 2X")},
		{"TMIC",_T("TeraLogic Motion Intraframe(TMIC)")},
		{"TMOT",_T("Horizons Technology TrueMotion S")},
		{"TR20",_T("Duck TrueMotion RT 2.0")},
		{"TSCC",_T("TechSmith Screen Capture")},
		{"TV10",_T("Tecomac Low-Bit Rate")},
		{"TVJP",_T("Truevision TARGA 2000(TVJP)")},
		{"TVMJ",_T("Truevision TARGA 2000(TVMJ)")},
		{"TY2C",_T("Trident Decompression(TY2C)")},
		{"TY2N",_T("Trident Decompression(TY2N)")},
		{"TY0N",_T("Trident Decompression(TY0N)")},
		{"U263",_T("UB Video StreamForce")},
		{"UCOD",_T("eMagix ClearVideo")},
		{"ULTI",_T("IBM Ultimotion")},
		{"ULRG",_T("Ut Video Codec RGB")},
		{"ULY0",_T("Ut Video Codec YUV420")},
		{"ULY2",_T("Ut Video Codec YUV422")},
		{"UMP4",_T("DivX3/MS MPEG4-V1(UMP4)")},
		{"UYVY",_T("Microsoft UYVY 4:2:2")},
		{"V261",_T("Lucent VX2000S")},
		{"V422",_T("Vitec Multimedia 24bitYUV")},
		{"V655",_T("Vitec Multimedia 16bitYUV")},
		{"VCR1",_T("ATI Video Codec 1.0")},
		{"VCR2",_T("ATI Video Codec 2.0")},
		{"VCR3",_T("ATI Video Codec 3.0")},
		{"VCR4",_T("ATI Video Codec 4.0")},
		{"VCR5",_T("ATI Video Codec 5.0")},
		{"VCR6",_T("ATI Video Codec 6.0")},
		{"VCR7",_T("ATI Video Codec 7.0")},
		{"VCR8",_T("ATI Video Codec 8.0")},
		{"VCR9",_T("ATI Video Codec 9.0")},
		{"VDCT",_T("Vitec Video Maker Pro DIB")},
		{"VDEC",_T("?(VDEC)")},
		{"VDOM",_T("VDOWave")},
		{"VDOW",_T("VDOLive")},
		{"VDST",_T("VirtualDub remote frameclient")},
		{"VDTZ",_T("Darim Vision VideoTizer YUV")},
		{"VGPX",_T("Alaris Wee Cam")},
		{"VIDS",_T("Vitec YUV 4:2:2 CCIR 601 for V422")},
		{"VIFP",_T("?(VIFP)")},
		{"VIVO",_T("Vivo H.263")},
		{"VIXL",_T("Miro Video XL")},
		{"VLV1",_T("VideoLogic?")},
		{"VP30",_T("On2 Open-Source VP3(VP30)")},
		{"VP31",_T("On2 Open-Source VP3(VP31)")},
		{"VP40",_T("On2 VP4")},
		{"VP50",_T("On2 VP5")},
		{"VP60",_T("On2 VP6(Simple)")},
		{"VP61",_T("On2 VP6(Advanced)")},
		{"VP62",_T("On2 VP6(VP62)")},
		{"VP70",_T("On2 VP7")},
		{"VSSH",_T("H.264(VSSH)")},
		{"VQC1",_T("ViewQuest VideoQuest Codec 1")},
		{"VQC2",_T("ViewQuest VideoQuest Codec 2")},
		{"VSSV",_T("Vanguard Software VSS Video")},
		{"VTLP",_T("Alaris VideoGram QuickVideo VGPixel(VTLP)")},
		{"VUUU",_T("?(VUUU)")},
		{"VX1K",_T("Lucent VX1000S")},
		{"VX2K",_T("Lucent VX2000S")},
		{"VXSP",_T("Lucent VX1000SP")},
		{"VYU9",_T("ATI YUV(VYU9)")},
		{"VYUY",_T("ATI YUV(VYUY)")},
		{"WBVC",_T("Winbond W9960")},
		{"WHAM",_T("Microsoft Video 1(WHAM)")},
		{"WINX",_T("Winnov Software Compression")},
		{"WJPG",_T("AverMedia USB TV-tuner/capture")},
		{"WMV1",_T("Windows Media Video 7")},
		{"WMV2",_T("Windows Media Video 8")},
		{"WMV3",_T("Windows Media Video 9 (WMV3)")},
		{"WMVA",_T("WMV9 Advanced Profile (WMVA)")},
		{"WVC1",_T("WMV9 Advanced Profile (WVC1)")},
		{"WNIX",_T("WniWni Video3")},
		{"WNV1",_T("Winnov Hardware Compression")},
		{"WRPR",_T("VideoTools VideoServer Client Codec")},
		{"X263",_T("Xirlink H.263 Video Codec")},
		{"X264",_T("H.264(X264)")},
		{"XLV0",_T("NetXL XL Video Decoder")},
		{"XLV1",_T("Videologic VLCAP")},
		{"XMPG",_T("XING IFlameOnry MPEG")},
		{"XTOR",_T("Dxtory")},
		{"XVID",_T("XviD MPEG4")},
		{"XVIX",_T("DivX3/MS MPEG4-V2(XVIX)")},
		{"XVW0",_T("XiWave Video(XVW0)")},
		{"XVW1",_T("XiWave Video(XVW1)")},
		{"XVW2",_T("XiWave Video(XVW2)")},
		{"XVW3",_T("XiWave Video(XVW3)")},
		{"XVW4",_T("XiWave Video(XVW4)")},
		{"XVW5",_T("XiWave Video(XVW5)")},
		{"XVW6",_T("XiWave Video(XVW6)")},
		{"XVW7",_T("XiWave Video(XVW7)")},
		{"XVW8",_T("XiWave Video(XVW8)")},
		{"XVW9",_T("XiWave Video(XVW9)")},
		{"XXAN",_T("?(XXAN)")},
		{"Y41P",_T("Brooktree YUV 4:1:1")},
		{"Y8  ",_T("Grayscale video")},
		{"Y211",_T("Microsoft YUV 2:1:1")},
		{"Y411",_T("Microsoft YUV 4:1:1 Packed")},
		{"Y41B",_T("Microsoft YUV 4:1:1 Planar")},
		{"Y41P",_T("Brooktree PCI 4:1:1")},
		{"Y41T",_T("Brooktree PCI 4:1:1 with transparency")},
		{"Y42B",_T("Weitek YUV 4:2:2")},
		{"Y42T",_T("Brooktree PCI 4:2:2 with transparency")},
		{"YC12",_T("Intel YUV12")},
		{"YU92",_T("Intel YUV(YU92)")},
		{"YUV8",_T("Winnov Caviar YUV8")},
		{"YUV9",_T("Indeo Video Raw(YUV9)")},
		{"YUVP",_T("uncompressed YCrCb 4:2:2")},
		{"YUY2",_T("Microsoft Raw uncompressed YUV 4:2:2")},
		{"YUYV",_T("Canopus YUVY")},
		{"YV12",_T("Weitek YVU12")},
		{"YV16",_T("Elecard YUV 4:2:2 Planar")},
		{"YV92",_T("Intel Smart Video Recorder")},
		{"YVU9",_T("Indeo Video Raw(YVU9)")},
		{"YVYU",_T("Microsoft YVYU 4:2:2")},
		{"ZLIB",_T("AVIzlib(LCL)")},
		{"ZPEG",_T("Metheus Video Zipper")},
		{"ZYGO",_T("ZyGo ZyGoVideo")},
		{"ZYYY",_T("?(ZYYY)")},
//		{" ",_T("無圧縮")}
	};

//チャンクを検索します
//return=TRUEのときは見つかったチャンクの先頭+8の位置にいます
//return=FALSEのときは最終チャンクの最後尾+1
BOOL findChunk(HANDLE hFile,__int64 llFileSize,UINT flag,FOURCC type,DWORD *pdwSize)
{
	FOURCC id;
	FOURCC fType;
	DWORD dwRet;
	DWORD dwSize;
	// 1G = 1073741824
	// 2G = 2147483648
	// 4G = 4294967296

	while(1)
	{
		__int64 llChunkHead = SetFilePointer64(hFile,0,FILE_CURRENT);
		if(llChunkHead >= llFileSize)
		{
			break;
		}
		if(!ReadFile(hFile,&id,sizeof(id),&dwRet,NULL) || (dwRet != sizeof(id)))
		{
			SetFilePointer64(hFile,llChunkHead,FILE_BEGIN);
			return FALSE;
		}
		if(!ReadFile(hFile,&dwSize,sizeof(dwSize),&dwRet,NULL) || (dwRet != sizeof(dwSize)))
		{
			SetFilePointer64(hFile,llChunkHead,FILE_BEGIN);
			return FALSE;
		}
		if(dwSize%2)
		{
			dwSize++;
		}
		*pdwSize = dwSize;
		switch(id){
		case FOURCC_RIFF:
			if(!ReadFile(hFile,&fType,sizeof(fType),&dwRet,NULL) || (dwRet != sizeof(fType)))
			{
				SetFilePointer64(hFile,llChunkHead,FILE_BEGIN);
				return FALSE;
			}
			// 最後のRiffチャンク位置を保存
			*pdwSize -= sizeof(fType);
			if(flag != MMIO_FINDRIFF)
			{
				break;
			}
			if(fType == type)
			{
				// pdwSizeの補正(ありえない値を返さないように)
				__int64 ptr = llChunkHead + 12;
				if((llFileSize - ptr) < *pdwSize)
				{
					*pdwSize = (LONG )(llFileSize - ptr);
				}
				return TRUE;
			}
			break;
			
		case FOURCC_LIST:
			if(!ReadFile(hFile,&fType,sizeof(fType),&dwRet,NULL) || (dwRet != sizeof(fType)))
			{
				SetFilePointer64(hFile,llChunkHead,FILE_BEGIN);
				return FALSE;
			}
			*pdwSize -= sizeof(fType);
			if(flag != MMIO_FINDLIST)
			{
				break;
			}
			if(fType == type)
			{
				// pdwSizeの補正(ありえない値を返さないように)
				__int64 ptr = llChunkHead + 12;
				if((llFileSize - ptr) < *pdwSize)
				{
					*pdwSize = (LONG )(llFileSize - ptr);
				}
				return TRUE;
			}
			break;
			
		default:
			fType = 0x20202020;
			if(id == type)
			{
				// pdwSizeの補正(ありえない値を返さないように)
				__int64 ptr = llChunkHead + 8;
				if((llFileSize - ptr) < *pdwSize)
				{
					*pdwSize = (LONG )(llFileSize - ptr);
				}
				return TRUE;
			}
			break;
		}
		TRACE(_T("%c%c%c%c %c%c%c%c %I64u(%lu)\n"),
			(((LPTSTR)(&id))[0]),(((LPTSTR)(&id))[1]),(((LPTSTR)(&id))[2]),(((LPTSTR)(&id))[3]),
			(((LPTSTR)(&fType))[0]),(((LPTSTR)(&fType))[1]),(((LPTSTR)(&fType))[2]),(((LPTSTR)(&fType))[3]),
			llChunkHead,dwSize
			);
		SetFilePointer64(hFile,(__int64 )*pdwSize,FILE_CURRENT);

	}
	return FALSE;
}

BOOL findChunk2(HANDLE hFile,__int64 llFileSize,DWORD *pdwSize)
{
	FOURCC id;
	FOURCC fType;
	DWORD dwRet;
	DWORD dwSize;
	// 1G = 1073741824
	// 2G = 2147483648
	// 4G = 4294967296

	for(int i=0; i<100; i++)	// 2003-08-04 ファイルの先頭部分に無かったらあきらめる(破損ファイルのフリーズ対策
//	while(1)
	{
		__int64 llChunkHead = SetFilePointer64(hFile,0,FILE_CURRENT);
		if(!ReadFile(hFile,&id,sizeof(id),&dwRet,NULL) || (dwRet != sizeof(id)))
		{
			SetFilePointer64(hFile,llChunkHead,FILE_BEGIN);
			return FALSE;
		}
		if(!ReadFile(hFile,&dwSize,sizeof(dwSize),&dwRet,NULL) || (dwRet != sizeof(dwSize)))
		{
			SetFilePointer64(hFile,llChunkHead,FILE_BEGIN);
			return FALSE;
		}
		if(dwSize%2)
		{
			dwSize++;
		}
		*pdwSize = dwSize;
		switch(id){
		case FOURCC_RIFF:
			break;
			
		case FOURCC_LIST:
			break;
		default:
			fType = 0x20202020;
			if((id == mmioFOURCC('0','0','d','b')) || (id == mmioFOURCC('0','0','d','c')))
			{
				// pdwSizeの補正(ありえない値を返さないように)
				__int64 ptr = llChunkHead + 8;
				if((llFileSize - ptr) < *pdwSize)
				{
					*pdwSize = (LONG )(llFileSize - ptr);
				}
				return TRUE;
			}
			break;
		}
		SetFilePointer64(hFile,(__int64 )*pdwSize,FILE_CURRENT);
		TRACE(_T("%c%c%c%c %c%c%c%c %I64u(%lu)\n"),
			(((LPTSTR)(&id))[0]),(((LPTSTR)(&id))[1]),(((LPTSTR)(&id))[2]),(((LPTSTR)(&id))[3]),
			(((LPTSTR)(&fType))[0]),(((LPTSTR)(&fType))[1]),(((LPTSTR)(&fType))[2]),(((LPTSTR)(&fType))[3]),
			llChunkHead,dwSize
			);

	}
	return FALSE;
}

static BOOL findAudioCodecName1(CString &strFormatName,DWORD wFormatTag,GUID guid)
{
	for(int i=0; i<sizeof_array(audio_codecs); i++)
	{
		if(audio_codecs[i].dwType == wFormatTag)
		{
			strFormatName = audio_codecs[i].format;
			return TRUE;
		}
	}
	if(wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		if(guid == KSDATAFORMAT_SUBTYPE_PCM)
		{
			strFormatName = _T("PCM");
			return TRUE;
		}
		else if(guid == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
		{
			strFormatName = _T("PCM(IEEE FLOAT)");
			return TRUE;
		}
		else if(guid == KSDATAFORMAT_SUBTYPE_ALAW)
		{
			strFormatName = _T("CCITT A-Law");
			return TRUE;
		}
		else if(guid == KSDATAFORMAT_SUBTYPE_MULAW)
		{
			strFormatName = _T("CCITT u-Law");
			return TRUE;
		}
		else if(guid == KSDATAFORMAT_SUBTYPE_ADPCM)
		{
			strFormatName = _T("ADPCM");
			return TRUE;
		}
		else if(guid == KSDATAFORMAT_SUBTYPE_MPEG)
		{
			strFormatName = _T("MPEG");
			return TRUE;
		}
	}
	return FALSE;
}

static BOOL findAudioCodecName2(CString &strFormatName,DWORD formatTag,GUID guid)
{
	ACMFORMATTAGDETAILS	aftd;
	memset(&aftd,0,sizeof(ACMFORMATTAGDETAILS));
	aftd.cbStruct = sizeof(ACMFORMATTAGDETAILS);
	aftd.dwFormatTag = formatTag;
	if(acmFormatTagDetails(NULL,&aftd,ACM_FORMATTAGDETAILSF_FORMATTAG) == 0)
	{
		strFormatName = aftd.szFormatTag;
		return TRUE;
	}
	return FALSE;
}

static void findAudioCodecName(int iCodecFind,CString &strFormatName,DWORD formatTag,GUID guid)
{
	switch(iCodecFind){
	case 0:
	default:
		if(!findAudioCodecName1(strFormatName,formatTag,guid))
		{
			findAudioCodecName2(strFormatName,formatTag,guid);
		}
		break;
	case 1:
		if(!findAudioCodecName2(strFormatName,formatTag,guid))
		{
			findAudioCodecName1(strFormatName,formatTag,guid);
		}
		break;
	case 2:
		findAudioCodecName1(strFormatName,formatTag,guid);
		break;
	case 3:
		findAudioCodecName2(strFormatName,formatTag,guid);
		break;
	}
}

BOOL findVideoCodecName1(CString &strVideoFormat,DWORD biCompression,DWORD fccHeader)
{
	if(mmioFOURCC('D','I','V','X') == fccHeader)
	{
		if(memcmp(&biCompression,"DX50",4) == 0)
		{
			strVideoFormat = _T("DivX5");
		}
		else
		{
			strVideoFormat = _T("DivX4");
		}
		return TRUE;
	}
	int i=0;
	for(; i<sizeof(video_codecs)/sizeof(video_codecs[0]); i++)
	{
		if(mmioFOURCC(video_codecs[i].type[0],video_codecs[i].type[1],video_codecs[i].type[2],video_codecs[i].type[3]) == fccHeader)
		{
			strVideoFormat = video_codecs[i].format;
			return TRUE;
		}
	}
	for(i=0; i<sizeof(video_codecs)/sizeof(video_codecs[0]); i++)
	{
		if(mmioFOURCC(video_codecs[i].type[0],video_codecs[i].type[1],video_codecs[i].type[2],video_codecs[i].type[3]) == biCompression)
		{
			strVideoFormat = video_codecs[i].format;
			return TRUE;
		}
	}
	return FALSE;
}

static BOOL findVideoCodecName2(CString &strVideoFormat,DWORD biCompression,DWORD fccHeader)
{
	BOOL ret = FALSE;

	HIC hic = ICOpen(ICTYPE_VIDEO,fccHeader,ICMODE_QUERY);
	if(hic)
	{
		ICINFO icinfo;
		memset(&icinfo,0,sizeof(icinfo));
		icinfo.dwSize = sizeof(ICINFO);
		if(ICGetInfo(hic,&icinfo,sizeof(ICINFO)) > 0)
		{
			strVideoFormat = icinfo.szDescription;
			ret = TRUE;
		}
		ICClose(hic);
	}
	if(!ret)
	{
		hic = ICOpen(ICTYPE_VIDEO,biCompression,ICMODE_QUERY);
		if(hic)
		{
			ICINFO icinfo;
			memset(&icinfo,0,sizeof(icinfo));
			icinfo.dwSize = sizeof(ICINFO);
			if(ICGetInfo(hic,&icinfo,sizeof(ICINFO)) > 0)
			{
				strVideoFormat = icinfo.szDescription;
				ret = TRUE;
			}
			ICClose(hic);
		}
	}
	return ret;
}

static void findVideoCodecName(int iCodecFind,CString &strFormatName,DWORD biCompression,DWORD fccHeader)
{
	switch(iCodecFind){
	case 0:
	default:
		if(!findVideoCodecName1(strFormatName,biCompression,fccHeader))
		{
			findVideoCodecName2(strFormatName,biCompression,fccHeader);
		}
		break;
	case 1:
		if(!findVideoCodecName2(strFormatName,biCompression,fccHeader))
		{
			findVideoCodecName1(strFormatName,biCompression,fccHeader);
		}
		break;
	case 2:
		findVideoCodecName1(strFormatName,biCompression,fccHeader);
		break;
	case 3:
		findVideoCodecName2(strFormatName,biCompression,fccHeader);
		break;
	}
}

void GetAviFormat(IN LPCTSTR szFileName,
					OUT CString &strAuidoFormat,
					OUT CString &strVideoFormat,
					OUT CString &strStreamFormat,
					OUT CString &strTime,
					OUT BOOL &bAvi2,
					IN int iAviCodecFind)
{
	HANDLE hFile;
	DWORD dwRet,dwSize;
	__int64 llFileSize;
	__int64 llPtr;
	DWORD dwChunkSize;
	DWORD dwRiffAviSize;
	FOURCC id,type;
	DWORD dwSec;

	bAvi2 = FALSE;
	//「なし」
	strAuidoFormat.LoadString(IDS_NO_AUDIO_STREAM);
	//「なし」
	strVideoFormat.LoadString(IDS_NO_VIDEO_STREAM);
	
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//ファイルを開く
	hFile = CreateFile(szFileName,
					GENERIC_READ,
					FILE_SHARE_READ,NULL,
					OPEN_EXISTING,	//ファイルを開きます。指定したファイルが存在していない場合、この関数は失敗します。 
					FILE_ATTRIBUTE_NORMAL,
					NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		goto exit;
	}
	
	llFileSize = GetFileSize64(hFile);

	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//RIFF-AVI を確認
	if(!ReadFile(hFile,&id,sizeof(id),&dwRet,NULL))
	{
		goto exit;
	}
	// 総サイズ
	if(!ReadFile(hFile,&dwRiffAviSize,sizeof(dwRiffAviSize),&dwRet,NULL))
	{
		goto exit;
	}
	// Riff type
	if(!ReadFile(hFile,&type,sizeof(type),&dwRet,NULL))
	{
		goto exit;
	}
	if((FOURCC_RIFF != id) && (mmioFOURCC('A','V','I',' ') != type))
	{
		goto exit;
	}

	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	// LIST-hdrlを検索
	if(!findChunk(hFile,dwRiffAviSize+8,MMIO_FINDLIST,mmioFOURCC('h','d','r','l'),&dwSize))
	{
		goto exit;
	}
	llPtr = SetFilePointer64(hFile,0,FILE_CURRENT);
	dwChunkSize = dwSize;
	
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	// LIST-avihを検索
	if(!findChunk(hFile,llPtr+dwChunkSize,MMIO_FINDLIST,mmioFOURCC('a','v','i','h'),&dwSize))
	{
		goto exit;
	}
	
	MainAVIHeader AVIHeader;
	memset(&AVIHeader,0,sizeof(AVIHeader));
	if(!ReadFile(hFile,&AVIHeader,sizeof(AVIHeader),&dwRet,NULL))
	{
		goto exit;
	}
	//0除算防止
	if(AVIHeader.dwMicroSecPerFrame == 0)
	{
		AVIHeader.dwMicroSecPerFrame = -1;
	}
	SetFilePointer64(hFile,llPtr,FILE_BEGIN);
	while(1)
	{
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// LIST-strlを検索
		if(!findChunk(hFile,llPtr+dwChunkSize,MMIO_FINDLIST,mmioFOURCC('s','t','r','l'),&dwSize))
		{
			break;	// while(1)から抜ける
		}
		__int64 llPtrStrl = SetFilePointer64(hFile,0,FILE_CURRENT);
		DWORD dwStrlSize = dwSize;
		
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// LIST-strhを検索
		if(!findChunk(hFile,llPtrStrl+dwStrlSize,MMIO_FINDCHUNK,mmioFOURCC('s','t','r','h'),&dwSize))
		{
			goto exit;
		}

		AVIStreamHeader streamHeader;
		memset(&streamHeader,0,sizeof(streamHeader));
		if(!ReadFile(hFile,&streamHeader,sizeof(streamHeader),&dwRet,NULL))
		{
			goto exit;
		}
		
		SetFilePointer64(hFile,llPtrStrl,FILE_BEGIN);
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// LIST-strfを検索
		if(!findChunk(hFile,llPtrStrl+dwStrlSize,MMIO_FINDCHUNK,mmioFOURCC('s','t','r','f'),&dwSize))
		{
			goto exit;
		}
		llPtrStrl = SetFilePointer64(hFile,0,FILE_CURRENT) + dwSize;
		
		if(streamHeader.fccType == streamtypeAUDIO)
		{
			WAVEFORMATEX wfx;
			memset(&wfx,0,sizeof(wfx));
			if(!ReadFile(hFile,&wfx,sizeof(wfx),&dwRet,NULL))
			{
				goto exit;
			}
			CString strFormatName;
//			//「不明」
			strFormatName.LoadString(IDS_UNKNOWN);
			findAudioCodecName(iAviCodecFind,strFormatName,wfx.wFormatTag,((WAVEFORMATEXTENSIBLE *)&wfx)->SubFormat);
			if(wfx.wBitsPerSample)
			{
				strAuidoFormat.Format(_T("%s, %ldbit, %ldHz, %s, %ldkbps"),
							strFormatName,
							wfx.wBitsPerSample,
							wfx.nSamplesPerSec,
							(wfx.nChannels == 1)?_T("Mono"):_T("Stereo"),
							wfx.nAvgBytesPerSec*8/1000
							);
			}
			else
			{
				strAuidoFormat.Format(_T("%s, %ldHz, %s, %ldkbps"),
							strFormatName,
							wfx.nSamplesPerSec,
							(wfx.nChannels == 1)?_T("Mono"):_T("Stereo"),
							wfx.nAvgBytesPerSec*8/1000
							);
			}
		}
		else if(streamHeader.fccType == streamtypeVIDEO)
		{
			BITMAPINFOHEADER bi;
			memset(&bi,0,sizeof(bi));
			if(!ReadFile(hFile,&bi,sizeof(bi),&dwRet,NULL))
			{
				goto exit;
			}
			//「不明」
			strVideoFormat.LoadString(IDS_UNKNOWN);
			streamHeader.fccHandler = (
							(toupper((streamHeader.fccHandler & 0xff000000) >> 24) << 24) |
							(toupper((streamHeader.fccHandler & 0x00ff0000) >> 16) << 16) |
							(toupper((streamHeader.fccHandler & 0x0000ff00) >> 8) << 8) |
							(toupper((streamHeader.fccHandler & 0x000000ff) >> 0) << 0)
						);
			findVideoCodecName(iAviCodecFind,strVideoFormat,bi.biCompression,streamHeader.fccHandler);
		}
		SetFilePointer64(hFile,llPtrStrl,FILE_BEGIN);
	}
	
	/////////////////////////////////////////////////////////////
	// OpenDML判別
	
	SetFilePointer64(hFile,llPtr,FILE_BEGIN);
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	// LIST-odmlを検索
	if(findChunk(hFile,llPtr+dwChunkSize,MMIO_FINDLIST,mmioFOURCC('o','d','m','l'),&dwSize))
	{
		bAvi2 = TRUE;
		// dmlhを検索
		__int64 llodmlPtr = SetFilePointer64(hFile,0,FILE_CURRENT);
		if(findChunk(hFile,llodmlPtr+dwSize,MMIO_FINDCHUNK,mmioFOURCC('d','m','l','h'),&dwSize))
		{
			DWORD totalFrames;
			ReadFile(hFile,&totalFrames,sizeof(totalFrames),&dwRet,NULL);
			AVIHeader.dwTotalFrames = totalFrames;
		}
	}
	strStreamFormat.Format(_T("%ldx%ld, %.03lffps, %ldFrames"),
						AVIHeader.dwWidth,
						AVIHeader.dwHeight,
						1000000.0/(double )AVIHeader.dwMicroSecPerFrame,
						AVIHeader.dwTotalFrames/*-AVIHeader.dwInitialFrames*/);
	dwSec = (DWORD )(((__int64 )AVIHeader.dwMicroSecPerFrame*(__int64 )(AVIHeader.dwTotalFrames/*-AVIHeader.dwInitialFrames*/))/(1000*1000));
	strTime.Format(_T("%02ld:%02ld:%02ld (%ldsec)"),
					dwSec/60/60,
					dwSec/60%60,
					dwSec%60,
					dwSec);
	
	/////////////////////////////////////////////////////////////
	// Divx5のバージョンを区別する処理
	if(strVideoFormat.Compare(_T("DivX5")) == 0)
	{
		SetFilePointer64(hFile,llPtr,FILE_BEGIN);
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// LIST-moviを検索
		if(!findChunk(hFile,dwRiffAviSize+8,MMIO_FINDLIST,mmioFOURCC('m','o','v','i'),&dwSize))
		{
			goto exit;
		}
		__int64 llPtrMovi = SetFilePointer64(hFile,0,FILE_CURRENT);
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// 00dbチャンクを検索
		DWORD dwMoviSize = dwSize;
		CString strDivx5Version;
		CString strDivxVerEtc;
		for(int i=0; i<50; i++)
//		while(1)
		{
			if(!findChunk2(hFile,llPtrMovi+dwMoviSize,&dwSize))
			{
				break;
			}
			__int64 ptr = SetFilePointer64(hFile,0,FILE_CURRENT) + 8;
			// 先頭の0x30バイト読み取る
			char tmp[0x30];
			memset(tmp,0,sizeof(tmp));
			if(!ReadFile(hFile,&tmp,sizeof(tmp),&dwRet,NULL))
			{
				break;
			}
			SetFilePointer64(hFile,ptr,FILE_BEGIN);

			// divxバージョン情報を検索する
			for(int ver=0; ver<sizeof_array(divx_vers); ver++)
			{
				for(DWORD tmpPtr=0; tmpPtr<(sizeof(tmp)-strlen(divx_vers[ver].type)); tmpPtr++)
				{
					if(memcmp(&tmp[tmpPtr],divx_vers[ver].type,strlen(divx_vers[ver].type)) == 0)
					{
						strDivx5Version = divx_vers[ver].format;
						break;
					}
					else if(memcmp(&tmp[tmpPtr],"DivX5",5) == 0)
					{
						char buildNo[15];
						memset(buildNo,0,sizeof(buildNo));
						for(int a=0; a<14; a++)
						{
							int c = (int )((&tmp[tmpPtr])[a]) & 0x000000ff;
							if(!__isascii(c))
							{
								break;
							}
							buildNo[a] = c;
						}
						strDivxVerEtc = _T("DivX5/6.\?.\?(");
						strDivxVerEtc += buildNo;
						strDivxVerEtc += _T(")");
						break;
					}
				}
				if(strDivx5Version.GetLength() > 0)
				{
					break;
				}
			}
			if(strDivx5Version.GetLength() > 0)
			{
				break;
			}
		}
		if(strDivx5Version.GetLength() == 0)
		{
			strDivx5Version = strDivxVerEtc;
		}
		if(strDivx5Version.GetLength())
		{
			strVideoFormat = strDivx5Version;
		}
	}

	/////////////////////////////////////////////////////////////
	// XviDのバージョンを区別する処理
	if(strVideoFormat.Compare(_T("XviD MPEG4")) == 0)
	{
		SetFilePointer64(hFile,llPtr,FILE_BEGIN);
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// LIST-moviを検索
		if(!findChunk(hFile,dwRiffAviSize+8,MMIO_FINDLIST,mmioFOURCC('m','o','v','i'),&dwSize))
		{
			goto exit;
		}
		__int64 llPtrMovi = SetFilePointer64(hFile,0,FILE_CURRENT);
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// 00dbチャンクを検索
		DWORD dwMoviSize = dwSize;
		CString strDivx5Version;
		CString strDivxVerEtc;
		for(int i=0; i<50; i++)
//		while(1)
		{
			if(!findChunk2(hFile,llPtrMovi+dwMoviSize,&dwSize))
			{
				break;
			}
			__int64 ptr = SetFilePointer64(hFile,0,FILE_CURRENT) + 8;
			// 先頭の0x30バイト読み取る
			char tmp[0x30];
			memset(tmp,0,sizeof(tmp));
			if(!ReadFile(hFile,&tmp,sizeof(tmp),&dwRet,NULL))
			{
				break;
			}
			SetFilePointer64(hFile,ptr,FILE_BEGIN);

			// divxバージョン情報を検索する
			for(int ver=0; ver<sizeof_array(xvid_vers); ver++)
			{
				for(DWORD tmpPtr=0; tmpPtr<(sizeof(tmp)-strlen(xvid_vers[ver].type)); tmpPtr++)
				{
					if(memcmp(&tmp[tmpPtr],xvid_vers[ver].type,strlen(xvid_vers[ver].type)) == 0)
					{
						strDivx5Version = xvid_vers[ver].format;
						break;
					}
					else if(memcmp(&tmp[tmpPtr],"XviD0",5) == 0)
					{
						char buildNo[15];
						memset(buildNo,0,sizeof(buildNo));
						for(int a=0; a<8; a++)
						{
							int c = (int )((&tmp[tmpPtr])[a]) & 0x000000ff;
							if(!__isascii(c))
							{
								break;
							}
							buildNo[a] = c;
						}
						strDivxVerEtc = _T("XviD (");
						strDivxVerEtc += buildNo;
						strDivxVerEtc += _T(")");
						break;
					}
				}
				if(strDivx5Version.GetLength() > 0)
				{
					break;
				}
			}
			if(strDivx5Version.GetLength() > 0)
			{
				break;
			}
		}
		if(strDivx5Version.GetLength() == 0)
		{
			strDivx5Version = strDivxVerEtc;
		}
		if(strDivx5Version.GetLength())
		{
			strVideoFormat = strDivx5Version;
		}
	}
	
exit:

	if(hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
	}
}

void GetWaveAudioFormat(IN LPCTSTR szFileName,
					IN DWORD dwStreamSize,
					OUT CString &strFormat,
					OUT CString &strTime,
					int iWaveCodecFind)
{
	//初期値
	//「不明」
	strFormat.LoadString(IDS_UNKNOWN);
	//「不明」
	strTime.LoadString(IDS_UNKNOWN);
	
	HMMIO hmmio = mmioOpen((LPTSTR)szFileName,NULL,MMIO_COMPAT);
	if(!hmmio)
	{
		return;
	}
	//WAVEチャンクへ移動
	MMCKINFO	mmckOutinfoParent;
	memset(&mmckOutinfoParent,0,sizeof(mmckOutinfoParent));
	mmckOutinfoParent.fccType = mmioFOURCC('W','A','V','E');
	if(mmioDescend(hmmio,&mmckOutinfoParent,NULL,MMIO_FINDRIFF) != MMSYSERR_NOERROR)
	{
		mmioClose(hmmio,0);
		return;
	}
	//fmt チャンクへ移動
	MMCKINFO	mmckOutinfoSubchunk;
	memset(&mmckOutinfoSubchunk,0,sizeof(mmckOutinfoSubchunk));
	mmckOutinfoSubchunk.ckid = mmioFOURCC('f','m','t',' ');
	if(mmioDescend(hmmio,&mmckOutinfoSubchunk,&mmckOutinfoParent,MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
	{
		mmioClose(hmmio,0);
		return;
	}

	WAVEFORMATEX *pwfx;
	pwfx = (WAVEFORMATEX *)malloc(mmckOutinfoSubchunk.cksize);
	if(!pwfx)
	{
		mmioClose(hmmio,0);
		return;
	}
	memset(pwfx,0,mmckOutinfoSubchunk.cksize);
	
	//WAVEFORMATをリード
	if(mmioRead(hmmio,(HPSTR)pwfx,mmckOutinfoSubchunk.cksize) == -1)
	{
		free(pwfx);
		mmioClose(hmmio,0);
		return;
	}
	mmioClose(hmmio,0);
	
	CString format;
	//「不明」
	format.LoadString(IDS_UNKNOWN);
	findAudioCodecName(iWaveCodecFind,format,pwfx->wFormatTag,((WAVEFORMATEXTENSIBLE *)pwfx)->SubFormat);

	if(pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		pwfx->wBitsPerSample = ((WAVEFORMATEXTENSIBLE *)pwfx)->Samples.wValidBitsPerSample;
	}
	if(pwfx->wBitsPerSample)
	{
		strFormat.Format(_T("%s, %ubit, %luHz, %uch, %lukbps"),
					format,
					pwfx->wBitsPerSample,
					pwfx->nSamplesPerSec,
					pwfx->nChannels,
					pwfx->nAvgBytesPerSec*8/1000
					);
	}
	else
	{
		strFormat.Format(_T("%s, %luHz, %uch, %lukbps"),
					format,
					pwfx->nSamplesPerSec,
					pwfx->nChannels,
					pwfx->nAvgBytesPerSec*8/1000
					);
	}
	if(pwfx->nAvgBytesPerSec)
	{
		DWORD dwSec = dwStreamSize/(pwfx->nAvgBytesPerSec);
		strTime.Format(_T("%ld:%02ld (%ldsec)"),
						dwSec/60,
						dwSec%60,
						dwSec);
	}
	free(pwfx);
}

static LPCTSTR WINAMP_CLASS	= _T("Winamp v1.x");
static LPCTSTR WINAMP_PE_CLASS	= _T("Winamp PE");
static LPCTSTR SCMPX_CLASS = _T("SCMPX");

void PlayWinamp0()
{
	HWND hwndWinamp = FindWindow(WINAMP_CLASS,NULL);
	DWORD_PTR dwRet;

	if(hwndWinamp)
	{
		SendMessageTimeout(hwndWinamp,WM_WA_IPC,0,IPC_STARTPLAY,
			SMTO_ABORTIFHUNG | SMTO_BLOCK,100,&dwRet);
	}
}

void PlayWinamp(HWND hWnd,LPCTSTR szPlayFile)
{
	HWND hwndWinamp = FindWindow(WINAMP_CLASS,NULL);
	DWORD_PTR dwRet;
	if(hwndWinamp)
	{
		//Winamp専用再生コマンド
		COPYDATASTRUCT cds;

#ifdef UNICODE
		cds.dwData = IPC_PLAYFILEW; //Starts playback. A lot like hitting 'play' in Winamp, but not exactly the same
#else
		cds.dwData = IPC_PLAYFILE;  //Starts playback. A lot like hitting 'play' in Winamp, but not exactly the same
#endif
		cds.lpData = (void*)szPlayFile;
		cds.cbData = (lstrlen(szPlayFile) + 1) * sizeof(TCHAR);
		if(!SendMessageTimeout(hwndWinamp, WM_WA_IPC, 0, IPC_DELETE,
			SMTO_ABORTIFHUNG | SMTO_BLOCK,100,&dwRet)) // Clears Winamp's internal playlist. 
			return;
		if(!SendMessageTimeout(hwndWinamp, WM_COPYDATA, NULL, (LPARAM)&cds,
			SMTO_ABORTIFHUNG | SMTO_BLOCK,100,&dwRet))
			return;
		if(!SendMessageTimeout(hwndWinamp, WM_COMMAND, WINAMP_BUTTON2, 0,
			SMTO_ABORTIFHUNG | SMTO_BLOCK,100,&dwRet)) // Play button
			return;
		return;
	}
	hwndWinamp = FindWindow(SCMPX_CLASS,NULL);
	if(hwndWinamp)
	{
		char bufPlayFile[MAX_PATH];
		TstrToData(szPlayFile, -1, bufPlayFile, sizeof(bufPlayFile), DTC_CODE_ANSI);
		SendMessageTimeout(hwndWinamp,WM_WA_IPC,0,IPC_DELETE,
			SMTO_ABORTIFHUNG | SMTO_BLOCK,100,&dwRet);
		for(DWORD i=0; i<=strlen(bufPlayFile); i++)
		{
			SendMessageTimeout(hwndWinamp,WM_WA_IPC,(LPARAM)bufPlayFile[i],IPC_PLAYFILE,
				SMTO_ABORTIFHUNG | SMTO_BLOCK,100,&dwRet);
		}
		SendMessageTimeout(hwndWinamp,WM_WA_IPC,0,IPC_STARTPLAY,
			SMTO_ABORTIFHUNG | SMTO_BLOCK,100,&dwRet);
		return;
	}
	ShellExecute(hWnd,_T("open"),szPlayFile,NULL,NULL,SW_SHOWNORMAL);
}

void PauseWinamp()
{
	HWND hwndWinamp = FindWindow(WINAMP_CLASS,NULL);
	DWORD_PTR dwRet;

	if(hwndWinamp)
	{
		SendMessageTimeout(hwndWinamp,WM_COMMAND,WINAMP_BUTTON3,0,
			SMTO_ABORTIFHUNG | SMTO_BLOCK,100,&dwRet);
	}
	else
	{
		hwndWinamp = FindWindow(SCMPX_CLASS,NULL);
		if(hwndWinamp)
		{
			SendMessageTimeout(hwndWinamp,WM_LBUTTONUP,0x0,
				MAKELPARAM((UINT )269,(UINT )57),
				SMTO_ABORTIFHUNG | SMTO_BLOCK,100,&dwRet);
		}
	}
}

void StopWinamp()
{
	HWND hwndWinamp = FindWindow(WINAMP_CLASS,NULL);
	DWORD_PTR dwRet;
	DWORD dwID = WINAMP_BUTTON4;

	if(hwndWinamp == NULL)
	{
		hwndWinamp = FindWindow(SCMPX_CLASS,NULL);
		dwID = 40022;
	}
	if(hwndWinamp)
	{
		SendMessageTimeout(hwndWinamp,WM_COMMAND,dwID,0,
			SMTO_ABORTIFHUNG | SMTO_BLOCK,100,&dwRet);
	}
}

void Rew5Winamp()
{
	HWND hwndWinamp = FindWindow(WINAMP_CLASS,NULL);
	DWORD_PTR dwRet;

	if(hwndWinamp)
	{
		SendMessageTimeout(hwndWinamp,WM_COMMAND,WINAMP_REW5S,0,
			SMTO_ABORTIFHUNG | SMTO_BLOCK,100,&dwRet);
	}
}

void Ffw5Winamp()
{
	HWND hwndWinamp = FindWindow(WINAMP_CLASS,NULL);
	DWORD_PTR dwRet;

	if(hwndWinamp)
	{
		SendMessageTimeout(hwndWinamp,WM_COMMAND,WINAMP_FFWD5S,0,
			SMTO_ABORTIFHUNG | SMTO_BLOCK,100,&dwRet);
	}
}

// 1 = 再生中
// 3 = 一時停止中
// 0 = 停止中
int IsPlayingWinamp(LPTSTR szFileName)
{
	HWND hwndWinamp = FindWindow(WINAMP_CLASS,NULL);
//	HWND hwndWinampPE = FindWindow(WINAMP_PE_CLASS,NULL);
	DWORD_PTR dwRet;

	// If it returns 1, it is playing. if it returns 3, it is paused, if it returns 0, it is not playing.
	if(hwndWinamp)
	{
		long isplaying = SendMessageTimeout(hwndWinamp,WM_WA_IPC,0,IPC_ISPLAYING,
			SMTO_ABORTIFHUNG | SMTO_BLOCK,100,&dwRet);
/*		if((isplaying == 1) || (isplaying == 3))
		{
			if(szFileName)
			{
				// 編集中のファイルを再生中?
				DWORD	iNowPlayNum=0;
				DWORD	iAllPlayNum=0;
				//プレイリストとカレント行を取得
				if(!SendMessageTimeout(hwndWinamp,WM_WA_IPC,0,IPC_WRITEPLAYLIST,SMTO_ABORTIFHUNG,1000,&iNowPlayNum))
				{
					return 0;
				}
				if(!SendMessageTimeout(hwndWinamp,WM_WA_IPC,0,IPC_GETLISTLENGTH,SMTO_ABORTIFHUNG,1000,&iAllPlayNum))
				{
					return 0;
				}
				
				//プレイリストからmp3ファイルのフルパスを取得する
				LPTSTR szWinampPl;
				if(!SendMessageTimeout(hwndWinamp,WM_WA_IPC,0,IPC_GETINIDIRECTORY,SMTO_ABORTIFHUNG,1000,(DWORD *)&szWinampPl))
				{
					return 0;
				}
				if(strcmp(szWinampPl,szFileName) == 0)
				{
					return isplaying;
				}
				return 0;
			}
		}*/
		return isplaying;
	}
	return 0;
}

int GetPosWinamp()
{
	HWND hwndWinamp = FindWindow(WINAMP_CLASS,NULL);
	DWORD_PTR dwRet;

	if(hwndWinamp)
	{
		long song_pos = SendMessageTimeout(hwndWinamp,WM_WA_IPC,0,IPC_GETOUTPUTTIME,
			SMTO_ABORTIFHUNG | SMTO_BLOCK,100,&dwRet);
		return song_pos;
	}
	return -1;
}

void SetPosWinamp(int pos)
{
	HWND hwndWinamp = FindWindow(WINAMP_CLASS,NULL);
	DWORD_PTR dwRet;

	if(hwndWinamp)
	{
		SendMessageTimeout(hwndWinamp,WM_WA_IPC,pos,IPC_JUMPTOTIME,
			SMTO_ABORTIFHUNG | SMTO_BLOCK,100,&dwRet);
	}
}

