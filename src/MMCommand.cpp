#include "stdafx.h"

#include "wa_ipc.h"
#include "ipc_pe.h"
#include "winampcmd.h"
#include "GlobalCommand.h"
#include "resource.h"		// メイン シンボル

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
	char *format;
}AUDIO_CODEC_TYPE;

typedef struct _VIDEO_CODEC_TYPE{
	char *type;
	char *format;
}VIDEO_CODEC_TYPE;

/*
	コーデック情報は"真空波動研"(http://www.kurohane.net/)の黒羽さん提供していただきました。2002-09-16
*/

AUDIO_CODEC_TYPE audio_codecs[] =
	{
		{0x0000,"Unknown"},
		{0x0001,"Microsoft PCM"},
		{0x0002,"Microsoft ADPCM"},
		{0x0003,"IEEE Float"},
		{0x0004,"Compaq Computer's VSELP"},
		{0x0005,"IBM CVSD"},
		{0x0006,"CCITT A-Law"},
		{0x0007,"CCITT u-Law"},
		{0x0008,"Microsoft DTS"},
		{0x0009,"Microsoft DRM"},
		{0x000A,"Windows Media Audio 9 Voice"},
		{0x0010,"OKI ADPCM"},
		{0x0011,"IMA ADPCM"},
		{0x0012,"MediaSpace ADPCM"},
		{0x0013,"Sierra ADPCM"},
		{0x0014,"CCITT G.723 ADPCM"},
		{0x0015,"DSP Group DigiSTD"},
		{0x0016,"DSP Group DigiFIX"},
		{0x0017,"Dialogic OKI ADPCM"},
		{0x0018,"MediaVision ADPCM"},
		{0x0019,"HP CU"},
		{0x0020,"YAMAHA ADPCM"},
		{0x0021,"Sonarc(TM) Compression"},
		{0x0022,"DSP Group TrueSpeech(TM)"},
		{0x0023,"Echo Speech"},
		{0x0024,"AUDIOFILE AF36"},
		{0x0025,"Audio Processing Technology"},
		{0x0026,"AUDIOFILE AF10"},
		{0x0027,"Prosody 1612"},
		{0x0028,"LRC"},
		{0x0030,"Dolby AC-2"},
		{0x0031,"GSM 6.10"},
		{0x0032,"MSNAudio"},
		{0x0033,"ADPCME"},
		{0x0034,"Control Resources Limited VQLPC"},
		{0x0035,"DSP Group REAL"},
		{0x0036,"DSP Group ADPCM"},
		{0x0037,"Control Resources Limited CR10"},
		{0x0038,"NMS VBXADPCM"},
		{0x0039,"Roland RDAC"},
		{0x003a,"EchoSC3"},
		{0x003b,"Rockwell ADPCM"},
		{0x003c,"Rockwell Digit LK"},
		{0x003d,"Xebec"},
		{0x0040,"G.721 ADPCM"},
		{0x0041,"G.728 CELP"},
		{0x0042,"MS G.723"},
		{0x0043,"IBM AVC ADPCM"},
		{0x0045,"SHARP G.726"},
		{0x004f,"Ogg Vorbis(mode1)"},
		{0x0050,"MPEG-1 layer 1, 2"},
	//選択	{0x0050,"Ogg Vorbis(mode2)"},
		{0x0051,"Ogg Vorbis(mode3)"},
		{0x0052,"RT24"},
		{0x0053,"PAC"},
		{0x0055,"MPEG1-Layer3"},
		{0x0059,"Lucent G.723"},
		{0x0060,"Cirrus"},
		{0x0061,"ESPCM"},
		{0x0062,"Voxware"},
		{0x0063,"Canopus Atrac"},
		{0x0064,"G.726 ADPCM"},
		{0x0065,"G.722 ADPCM"},
		{0x0066,"DSAT"},
		{0x0067,"DSAT Display"},
		{0x0069,"Voxware Byte Aligned"},
		{0x006f,"Ogg Vorbis(mode1+)"},
		{0x0070,"Ogg Vorbis(mode2+)"},
	//選択		{0x0070,"Voxware AC8"},
		{0x0071,"Ogg Vorbis(mode3+)"},
	//選択		{0x0071,"Voxware AC10"},
		{0x0072,"Voxware AC16"},
		{0x0073,"Voxware AC20"},
		{0x0074,"Voxware MetaVoice"},
		{0x0075,"Voxware MetaSound"},
		{0x0076,"Voxware RT29HW"},
		{0x0077,"Voxware VR12"},
		{0x0078,"Voxware VR18"},
		{0x0079,"Voxware TQ40"},
		{0x0080,"Softsound"},
		{0x0081,"Voxware TQ60"},
		{0x0082,"MSRT24"},
		{0x0083,"G.729A"},
		{0x0084,"MVI MV12"},
		{0x0085,"DF G.726"},
		{0x0086,"DF GSM610"},
		{0x0088,"ISIAudio"},
		{0x0089,"Onlive"},
		{0x0091,"Siemens SBC24"},
		{0x0092,"Dolby AC3 SPDIF"},
		{0x0097,"ZyXEL ADPCM"},
		{0x0098,"Philips LPCBB"},
		{0x0099,"Packed"},
		{0x00A0,"Malden Electronics PHONYTALK"},
		{0x00e1,"Microsoft ADPCM"},
		{0x00FF,"Advanced Audio Coding"},
		{0x0100,"Rhetorex ADPCM"},
		{0x0101,"BeCubed Software's IRAT"},
		{0x0111,"Vivo G.723"},
		{0x0112,"Vivo Siren"},
		{0x0123,"Digital G.723"},
		{0x0125,"Sanyo ADPCM"},
		{0x0130,"ACELP.net Sipro Lab Audio"},
		{0x0131,"Sipro Lab Telecom ACELP.4800"},
		{0x0132,"Sipro Lab Telecom ACELP.8V3"},
		{0x0133,"Sipro Lab Telecom ACELP.G.729"},
		{0x0134,"Sipro Lab Telecom ACELP.G.729A"},
		{0x0135,"Sipro Lab Telecom ACELP.KELVIN"},
		{0x0140,"Dictaphone G.726 ADPCM"},
		{0x0150,"Qualcomm PUREVOICE"},
		{0x0151,"Qualcomm HALFRATE"},
		{0x0155,"Ring Zero Systems TUBGSM"},
		{0x0160,"Windows Media Audio1"},
		{0x0161,"DivX Audio(WMA)"},
		{0x0162,"Windows Media Audio9 Professional"},
		{0x0163,"Windows Media Audio9 Lossless"},
		{0x0170,"UNISYS NAP ADPCM"},
		{0x0171,"UNISYS NAP ULAW"},
		{0x0172,"UNISYS NAP ALAW"},
		{0x0173,"UNISYS NAP 16K"},
		{0x0200,"Creative ADPCM"},
		{0x0202,"Creative FastSpeech8"},
		{0x0203,"Creative FastSpeech10"},
		{0x0220,"Quarterdeck"},
		{0x0210,"UHER informatic GmbH ADPCM"},
		{0x0215,"Ulead DV ACM"},
		{0x0216,"Ulead DV ACM"},
		{0x0220,"Quarterdeck"},
		{0x0300,"Fujitsu FM-TOWNS SND"},
		{0x0230,"I-link Worldwide ILINK VC"},
		{0x0240,"Aureal Semiconductor RAW SPORT"},
		{0x0241,"ESST AC3"},
		{0x0250,"Interactive Products HSX"},
		{0x0251,"Interactive Products RPELP"},
		{0x0260,"Consistent Software CS2"},
		{0x0270,"Sony ATRAC3 (SCX, same as MiniDisk LP2)"},
		{0x0400,"BTV Digital"},
		{0x0401,"Intel Music Coder"},
		{0x0402,"Ligos Indeo Audio"},
		{0x0450,"QDesign Music"},
		{0x0680,"AT&T Labs VME VMPCM"},
		{0x0681,"AT&T Labs TPC"},
		{0x1000,"Olivetti GSM"},
		{0x1001,"Olivetti ADPCM"},
		{0x1002,"Olivetti CELP"},
		{0x1003,"Olivetti SBC"},
		{0x1004,"Olivetti OPR"},
		{0x1100,"LH Codec"},
		{0x1101,"Lernout & Hauspie CELP codec"},
		{0x1102,"Lernout & Hauspie SBC codec"},
		{0x1103,"Lernout & Hauspie SBC codec"},
		{0x1104,"Lernout & Hauspie SBC codec"},
		{0x1400,"Norris"},
		{0x1401,"ISIAudio"},
		{0x1500,"Soundspace Music Compression"},
		{0x2000,"FAST Multimedia AG DVM (Dolby AC3)"},
		{0x181C,"VoxWare RT24 speech codec"},
		{0x181E,"Lucent elemedia AX24000P Music codec"},
		{0x1C07,"Lucent SX8300P speech codec"},
		{0x1C0C,"Lucent SX5363S G.723 compliant codec"},
		{0x1F03,"CUseeMe DigiTalk (ex-Rocwell)"},
		{0x1FC4,"NTC ALF2CD ACM"},
		{0x2001,"DTS"},
		{0x2002,"RealAudio?(14_4)"},
		{0x2003,"RealAudio?(28_8)"},
		{0x2004,"RealAudio?(COOK)"},
		{0x2005,"RealAudio?(DNET)"},
		{0x674f,"Ogg Vorbis (mode 1)"},
		{0x6750,"Ogg Vorbis (mode 2)"},
		{0x6751,"Ogg Vorbis (mode 3)"},
		{0x676f,"Ogg Vorbis (mode 1+)"},
		{0x6770,"Ogg Vorbis (mode 2+)"},
		{0x6771,"Ogg Vorbis (mode 3+)"},
		{0x7A21,"GSM-AMR (CBR no SID)"},
		{0x7A22,"GSM-AMR (VBR including SID)"},
		{0x77A1,"The True Audio"},
		{0xC0CC,"GigaLink Audio Codec"},
		{0xDFAC,"DebugMode SonicFoundry Vegas FrameServer ACM Codec"},
		{0xF1AC,"CoreFLAC ACM"},
		{0xFFFE,"Extensible wave format"},
		{0xFFFF,"In Development / Unregistered"}
	};

VIDEO_CODEC_TYPE divx_vers[] =
{
		{"DivX999b000","XviD MPEG4"},
		{"DivX500Build413","DivX 5.0.0"},
		{"DivX501b413","DivX 5.0.0"}, //Unconfirmed
		{"DivX501b450","DivX 5.0.1"},
		{"DivX501b481","DivX 5.0.2"},
		{"DivX501b484","DivX 5.0.2"},
		{"DivX501b487","DivX 5.0.2"},
		{"DivX503b688","DivX 5.0.3 Beta"},
		{"DivX503b696","DivX 5.0.3 Beta"},	// 2004-07-21 (自信なし)
		{"DivX503b740","DivX 5.0.3"},
		{"DivX503b795","DivX 5.0.4 Beta1-2"},
		{"DivX503b804","DivX 5.0.4 Beta3"},
		{"DivX503b814","DivX 5.0.4 Beta4"},
		{"DivX503b822","DivX 5.0.4"},
		{"DivX503b830","DivX 5.0.5.830"},
		{"DivX503b894","DivX 5.0.5 Kauehi"},
		{"DivX503b922","DivX 5.1.0 Beta1"},
		{"DivX503b936","DivX 5.1.0 Beta2"},
		{"DivX503b959","DivX 5.1.0.959"},// 2003-09-05
		{"DivX503b0985","DivX 5.1(DivX HD)"},
		{"DivX503b1009","DivX 5.1.1 Beta1"},
		{"DivX503b1025","DivX 5.1.1 Beta2"},
		{"DivX503b1031","DivX 5.1.1.1031"},// 2003-11-24
		{"DivX503b1263","DivX 5.2.0"},// 2004-07-19
		{"DivX503b1272","DivX 5.2.0(Dr.DivX 1.0.5)"},
		{"DivX503b1307","DivX 5.2.1 Alpha(Dr.DivX)"},
		{"DivX503b1314","DivX Pro 5.2.1 Codec (X-Transcoder)"},	// 2006-11-13
		{"DivX503b1328","DivX 5.2.1"},	// 2004-09-24
		{"DivX503b1338","DivX 5.2.1(Dr.DivX 1.0.6)"},	// 2004-11-07
		{"DivX503b1394","DivX Pro Plasma Codec Ver.5.3.0 Build 1394"},
		{"DivX503b1408","DivX Pro Plasma Codec Ver.5.3.0 Build 1408"},
		{"DivX503b1453","DivX Pro Fusion Beta Build 1453 (DivX Ver.5.9)"},
		{"DivX503b1461","DivX Pro Fusion Beta Build 1461 (DivX Ver.5.9)"},
		{"DivX503b1528","DivX 5.9 Fusion"},
		{"DivX503b1571","DivX 6.0.0"},	// 2005-07-08 BBS7592
		{"DivX503b1594","DivX 6.0.0 (DivX Converter 1.0)"},
		{"DivX503b1599","DivX 6 Helium"},
		{"DivX503b1612","DivX 6 Helium"},
		{"DivX503b1670","Dr.DivX 1.07"},
		{"DivX503b1697","DivX 6.0.3"},	// 2005-12-04 BBS8167
		{"DivX503b1737","DivX He-3"},
		{"DivX503b1786","DivX 6.1.0"},		// 2006-03-12 BBS8433
		{"DivX503b1807","DivX 6.1.0 Patch 1 Beta"},
		{"DivX503b1814","DivX 6.1.0 Patch 2 Beta"},
		{"DivX503b1828","DivX 6.1.1"},	// 2006-03-12 BBS8433
		{"DivX503b1838","DivX 6 (TMPGEnc)"},	// 2007-07-17 BBS8655
		{"DivX503b1893","DivX 6.2.0 Beta 1"},	// 2006-04-01 BBS8463
		{"DivX503b1910","DivX 6.2.0"},	// 2006-04-12
		{"DivX503b1913","DivX 6.2.1"},	// 2006-04-24 BBS8489
		{"DivX503b1915","DivX 6.2.1 Patch 1 Beta"},
		{"DivX503b1920","DivX 6.2.2"},	// 2006-04-24
		{"DivX503b1974","DivX 6.2.5"},	// 2007-07-26 BBS8688 
		{"DivX503b1977","DivX 6.2.5"},	// 2007-07-17 BBS8670
		{"DivX503b1988","DivX 6.2.5 (DivX Converter 6.2.1)"},	// 2007-08-27 BBS8715
		{"DivX503b2081","DivX 6.4 Beta 1"},	// 2007-10-03 BBS8757
		{"DivX503b2086","DivX 6.4"},		// 2007-10-03 BBS8781
		{"DivX503b2151","DivX 6.4 (DivX Author)"},
		{"DivX503b2201","DivX 6.5.0"},		// 2007-08-26 BBS9011
		{"DivX503b2207","DivX6.5.1"},	// 2007-08-26 BBS9019
		{"DivX503b2292","DivX6.6.0"},	// 2007-08-26 BBS9114
		{"DivX503b2306","DivX6.6.1"},	// 2007-08-26 BBS9125
		{"DivX503b2309","DivX 6.6.1"},
		{"DivX503b2318","DivX 6.6.1.4"},
		{"DivX503b2376","DivX 6 (Stage6)"},
		{"DivX503b2396","DivX 6.7 Beta"},
		{"DivX503b2432","DivX 6.7.0.28"},
		{"DivX503b2510","DivX 6.8.0.14"},
		{"DivX503b2521","DivX 6.8.0 (DivX Converter 6.6)"},
		{"DivX503b2559","DivX 6.8.2.6"},
		{"DivX503b2676","DivX 6.8.3.13"},
		{"DivX503b2816","DivX 6.8.5.5"},	//090626 BBS10017
};

VIDEO_CODEC_TYPE xvid_vers[] =
{
		{"XviD0009","XviD 0.9 BitStream 9"},
		{"XviD0011","XviD 0.9 Patch-66"},
		{"XviD0012","XviD 0.9 BitStream 12"},
		{"XviD0016","XviD 1.0 Patch-13"},
		{"XviD0019","XviD 1.0 Patch-47"},
		{"XviD0020","XviD 1.0 Patch-61"},
		{"XviD0021","XviD 1.0.0 Beta1"},
		{"XviD0022","XviD 1.0.0 Beta1.5"},
		{"XviD0023","XviD 1.0.0 Beta2"},
		{"XviD0024","XviD 1.0.0 Beta2.5"},
		{"XviD0025","XviD 1.0.0 Beta3"},
		{"XviD0026","XviD 1.0.0 RC1"},
		{"XviD0027","XviD 1.0.0 RC1b"},
		{"XviD0028","XviD 1.0.0 RC2"},
		{"XviD0029","XviD 1.0.0 RC3"},
		{"XviD0030","XviD 1.0.0 RC4"},
		{"XviD0031","XviD 1.0.0 RC4b"},
		{"XviD0032","XviD 1.0.0 RC4c"},
		{"XviD0033","XviD 1.0.0 RC4d"},
		{"XviD0034","XviD 1.0.0"},
		{"XviD0035","XviD 1.0.1"},
		{"XviD0036","XviD 1.0.2"},
		{"XviD0037","XviD 1.0.3"},
		{"XviD0038","XviD 1.1.0 Beta1"},
		{"XviD0039","XviD 1.1.0 Beta2"},
		{"XviD0040","XviD 1.1.0"},
		{"XviD0041","XviD 1.1.0 Final"},
		{"XviD0042","XviD 1.2.-127"},
		{"XviD0043","XviD 1.2 SMP"},
		{"XviD0044","XviD 1.1 1"},
		{"XviD0045","XviD 1.2.-127"},
		{"XviD0046","XviD 1.1.2 Final"},
		{"XviD0047","XviD 1.1.-127"},
		{"XviD0048","XviD 1.2.-127"},
		{"XviD0049","XviD 1.2.0"},
		{"XviD0050","XviD 1.2.1"},
		{"XviD0055","XviD 1.3.-127"},
};

VIDEO_CODEC_TYPE video_codecs[] =
	{
		//非圧縮
		{"DIB ","NoCompress(DIB)"},
		{"RGB ","NoCompress(RGB)"},
		{"BGR ","NoCompress(RGB32)"},
		{"RAW ","NoCompress(RAW)"},
		//圧縮
		{"3IVX","3ivx Delta x.x"},
		{"3IVD","DivX3/MS MPEG4-V3(3IVD)"},
		{"3IV0","3ivx Delta 1.0-3.5(3IV0)"},
		{"3IV1","3ivx Delta 1.0-3.5(3IV1)"},
		{"3IV2","3ivx Delta 4.0"},
		{"AAS4","Autodesk Animator (AAS4)"},
		{"AASC","Autodesk Animator (AASC)"},
		{"ABYR","Kensington?"},
		{"ADV1","Loronix WaveCodec"},
		{"ADVJ","Avid M-JPEG(ADVJ)"},
		{"AEIK","Intel Indeo 3.2"},
		{"AEMI","Array VideoONE MPEG1-I Capture"},
		{"AFLI","Autodesk Animator(FLI)"},
		{"AFLC","Autodesk Animator(FLC)"},
		{"AJPG","Animated JPEG"}, //Unconfirmed
		{"AMM2","AMV2 MT Codec"},
		{"AMPG","Array VideoONE MPEG"},
		{"AMV3","AMV3 Codec"},
		{"ANIM","Intel RDX(ANIM)"},
		{"AP41","AngelPotion MPEG4-V3 Hacked"},
		{"ASLC","Alparysoft Lossless Video Codec"},
		{"ASV1","Asus Video"},
		{"ASV2","Asus Video(2)"},
		{"ASVX","Asus Video 2.0"},
		{"AUR2","Aura 2 YUV 422"},
		{"AURA","Aura 1 YUV 411"},
		{"AVC1","H.264(AVC1)"},
		{"AVRN","Avid M-JPEG(AVRN)"},
		{"BINK","Bink Video"},
		{"BLZ0","DivX 5.x(BLZ0)"},
		{"BT20","Conexant Prosumer Video"},
		{"BTCV","Conexant Composite Video"},
		{"BW10","Broadway MPEG Capture/Compression"},
		{"CC12","Intel YUV12"},
		{"CDVC","Canopus DV"},
		{"CFCC","DPS Perception"},
		{"CFHD","Adobe Premiere HDV"},
		{"CGDI","Microsoft Camcorder Video"},
		{"CHAM","Winnov Caviara Champagne"},
		{"CJPG","Creative WebCam JPEG"},
		{"CLJR","Cirrus Logic YUV"},
		{"CMYK","Colorgraph 32Bit CMYK"},
		{"COL0","DivX3/MS MPEG4-V3(COL0)"},
		{"COL1","DivX3/MS MPEG4-V3(COL1)"},
		{"CPLA","Weitek YUV 4:2:0"},
		{"CRAM","Microsoft Video 1(CRAM)"},
		{"CVID","Radius Cinepak"},
		{"CWLT","Microsoft Color WLT DIB"},
		{"CYUV","Creative YUV"},
		{"CYUY","ATI YUV"},
		{"D261","DEC H.261"},
		{"D263","DEC H.263"},
		{"DAVC","H.264(DAVC)"},
		{"DCL1","Data Connection Conferencing"},
		{"DIVA","DivA MPEG-4"},

		{"DIVX","DivX4"},	// biCommpress を見てDivx5 or Divx4を確定
		{"DIV2","Microsoft MPEG4-V1/V2"},
		{"DIV3","DivX ;-) MPEG-4 Video Codec (low motion)"},
		{"DIV4","DivX ;-) MPEG-4 Video Codec (fast motion)"},
		{"DIV5","DivX5 (DIV5)"},
		{"DIV6","DivX (DIV6)"},
		
		{"DVIS","DV Codec \"Iris\""},	// 2004-03-13
		{"DVSD","DV Codec \"Iris\" [vdsd]"},	// 2004-03-13
		{"DVRS","DV Codec \"Iris\" Reference AVI"},	// 2004-03-13

		{"DM4V","Dicas MPEGable MPEG-4"},
		{"DMB1","Matrox Rainbow Runner MJPEG"},
		{"DMB2","Paradigm MJPG?"},
		{"DMK2","ViewSonic V36"},
		{"DOHC","DummyCodec"},
		{"DP02","DynaPel MPEG-4"},
		{"DPS0","DPS Reality"},
		{"DPSC","DPS PAR"},
		{"DSVD","DV Codec(DSVD)"},
		{"DV25","Matrox DVCPRO"},
		{"DV50","Matrox DVCPRO50"},
		{"DVAN","?(DVAN)"},
		{"DVC ","DV Codec(DVC )"},
		{"DVCP","DV Codec(DVCP)"},
		{"DVCS","DV Codec(DVCS)"},
		{"DVE2","Insoft DVE-2 Videoconferencing"},
		{"DVH1","Panasonic SMPTE 370M"},
		{"DVHD","DV Codec(DVHD)"},
		{"DVMA","Darim Vision DVMPEG"},
		{"DVSL","DV Codec(DVSL)"},
		{"DUCK","Duck TrueMotion 1.0"},
		{"DVX1","Lucent DVX1000SP"},
		{"DVX2","Lucent DVX2000S"},
		{"DVX3","Lucent DVX3000S"},
		{"DXGM","EA/CinemaWare Game Movie"},
		{"DXT1","DirectX Compressed Texture(DXT1)"},
		{"DXT2","DirectX Compressed Texture(DXT2)"},
		{"DXT3","DirectX Compressed Texture(DXT3)"},
		{"DXT4","DirectX Compressed Texture(DXT4)"},
		{"DXT5","DirectX Compressed Texture(DXT5)"},
		
		{"DX50","DivX5"},	//
		
		{"DXTN","DirectX Compressed Texture"},
		{"DXTC","DirectX Texture Compression"},
		{"ELK0","Elsa YUV(ELK0)"},
		{"EKQ0","Elsa YUV(EKQ0)"},
		{"EM2V","Etymonix MPEG-2 I-frame"},
		{"ESCP","Eidos Escape"},
		{"ETV1","eTreppid Video(ETV1)"},
		{"ETV2","eTreppid Video(ETV2)"},
		{"ETVC","eTreppid Video(ETVC)"},
		{"FFV1","FFMPEG"},
		{"FLJP","D-Vision Field Encoded Motion JPEG"},
		{"FMP4","FFmpeg MPEG4"},
		{"FPS1","Fraps Movie Capture"},
		{"FRWA","SoftLab-Nsk Forward Motion JPEG with alpha channel"},
		{"FRWD","SoftLab-Nsk Forward Motion JPEG(FRWD)"},
		{"FRWT","SoftLab-Nsk Forward Motion JPEG(FRWT)"},
		{"FRWU","Darim Vision Forward"},
		{"FVF1","Fractal Video Frame"},
		{"FVFW","FFVFW"},
		{"GLCC","GigaLink Video Codec"},
		{"GLZW","Motion LZW"},
		{"GPEG","Motion JPEG(GPEG)"},
		{"GWLT","Microsoft Greyscale WLT DIB"},
		{"H260","Intel ITU H.260"},
		{"H261","Intel ITU H.261"},
		{"H262","Intel ITU H.262"},
		{"H263","Intel ITU H.263"},
		{"H264","Intel ITU H.264"},
		{"H265","Intel ITU H.265"},
		{"H266","Intel ITU H.266"},
		{"H267","Intel ITU H.267"},
		{"H268","Intel ITU H.268"},
		{"H269","Intel ITU H.269"},
		{"HFYU","Ben Rudiak-Gould Huffyuv"},
		{"HMCR","Rendition Motion Compensation Format"},
		{"HMRR","Rendition Motion Compensation Format"},
		{"I263","Intel ITU H.263"},
		{"I420","Intel Indeo 4"},
		{"IAN ","Intel RDX(IAN)"},
		{"ICLB","InSoft CellB Videoconferencing"},
		{"IF09","Intel Intermediate YUV9"},
		{"IGOR","Power DVD"},
		{"IJPG","Intergraph JPEG"},
		{"ILVC","Intel Layered Video"},
		{"ILVR","ITU H.263+"},
		{"IPDV","I/O DATA Giga AVI DV"},
		{"IPJ2","Image Power JPEG2000"},
		{"IR21","Intel Indeo 2.1"},
		{"IRAW","Intel Uncompressed UYUV"},
		{"IR45","?(IR45)"},
		{"IV30","Intel Indeo 3"},
		{"IV31","Intel Indeo 3.1"},
		{"IV32","Intel Indeo 3.2"},
		{"IV33","Intel Indeo 3.3"},
		{"IV34","Intel Indeo 3.4"},
		{"IV35","Intel Indeo 3.5"},
		{"IV36","Intel Indeo 3.6"},
		{"IV37","Intel Indeo 3.7"},
		{"IV38","Intel Indeo 3.8"},
		{"IV39","Intel Indeo 3.9"},
		{"IV40","Intel Indeo 4.0"},
		{"IV41","Intel Indeo 4.1"},
		{"IV42","Intel Indeo 4.2"},
		{"IV43","Intel Indeo 4.3"},
		{"IV44","Intel Indeo 4.4"},
		{"IV45","Intel Indeo 4.5"},
		{"IV46","Intel Indeo 4.6"},
		{"IV47","Intel Indeo 4.7"},
		{"IV48","Intel Indeo 4.8"},
		{"IV49","Intel Indeo 4.9"},
		{"IV50","Intel Indeo 5.0"},
		{"IYUV","Intel Indeo iYUV R2.0"},
		{"JBYR","Kensington?"},
		{"JPEG","Microsoft StillImage JPEG"},
		{"JPGL","DIVIO JPEG Light"},
		{"KMVC","Karl Morton's Video"},
		{"L261","Lead H.261"},
		{"L263","Lead H.263"},
		{"LBYR","?(LBYR)"},
		{"LCMW","Lead Motion CMW"},
		{"LEAD","LEAD Video"},
		{"LJ2K","Lead JPEG 2000"},
		{"LJPG","LEAD MJPEG"},
		{"LGRY","Lead Grayscale Image"},
		{"LSVM","Vianet Lighting Strike Vmail"},
		{"LZO1","Lempel-Ziv-Oberhumer"},
		{"M261","Microsoft H.261"},
		{"M263","Microsoft H.263"},
		{"M4CC","Divio MPEG-4"},
		{"M4S2","MPEG-4 version 2 simple profile"},
		{"MC12","ATI Motion Compensation Format(MC12)"},
		{"MCAM","ATI Motion Compensation Format(MCAM)"},
		{"MJ2C","Motion JPEG2000"},
		{"MJPA","Pinnacle ReelTime MJPG-A Software"},
		{"MJPG","Motion JPEG(MJPG)"},
		{"MMES","Matrox MPEG-2 ES"},
		{"MP2A","Media Excel MPEG-2 Audio"},
		{"MP2T","Media Excel MPEG-2 Transport Stream"},
		{"MP2V","Media Excel MPEG-2 Video"},
		{"MP42","Microsoft MPEG4-V2"},
		{"MP43","Microsoft MPEG4-V3"},
		{"MP4A","Media Excel MPEG-4 Audio"},
		{"MP4S","Microsoft MPEG4(MP4S)"},
		{"MP4T","Media Excel MPEG-4 Transport Stream"},
		{"MP4V","Media Excel MPEG-4 Video"},
		{"MPEG","Chromatic MPEG1 Video I Frame"},
		{"MPG3","DivX3/MS MPEG4-V3(MPG3)"},
		{"MPG4","Microsoft MPEG4-V1"},
		{"MPGI","Sigma Designs MPEG"},
		{"MPNG","PNG images decoder"},
		{"MR16","?(MR16)"},
		{"MRCA","FAST Multimedia Mrcodec"},
		{"MRLE","Microsoft RLE"},
		{"MSVC","Microsoft Video1(MSVC)"},
		{"MSZH","AVImszh"},
		{"MTGA","TGA images decoder"},
		{"MTX1","Matrox(MTX1)"},
		{"MTX2","Matrox(MTX2)"},
		{"MTX3","Matrox(MTX3)"},
		{"MTX4","Matrox(MTX4)"},
		{"MTX5","Matrox(MTX5)"},
		{"MTX6","Matrox(MTX6)"},
		{"MTX7","Matrox(MTX7)"},
		{"MTX8","Matrox(MTX8)"},
		{"MTX9","Matrox(MTX9)"},
		{"MV12","?(MV12)"},
		{"MVI1","Motion Pixels MVI1"},
		{"MVI2","Motion Pixels MVI2"},
		{"MWV1","Aware Motion Wavelets"},
		{"NAVI","NAVI"},
		{"NT00","NewTek LigtWave HDTV"},
		{"NTN1","Nogatech Video Compression 1"},
		{"NTN2","Nogatech Video Compression 2"},
		{"NTZ0","netsuzo"}, //Unconfirmed
		{"NTZO","netsuzo"}, //Unconfirmed
		{"NUV1","NuppelVideo"},
		{"NVDS","nVIDIA GeForce3 Texture(NVDS)"},
		{"NVHS","nVIDIA GeForce3 Texture(NVHS)"},
		{"NVHU","nVIDIA GeForce3 Texture(NVHU)"},
		{"NVS0","nVIDIA GeForce2 GTS Pro Texture(NVS0)"},
		{"NVS1","nVIDIA GeForce2 GTS Pro Texture(NVS1)"},
		{"NVS2","nVIDIA GeForce2 GTS Pro Texture(NVS2)"},
		{"NVS3","nVIDIA GeForce2 GTS Pro Texture(NVS3)"},
		{"NVS4","nVIDIA GeForce2 GTS Pro Texture(NVS4)"},
		{"NVS5","nVIDIA GeForce2 GTS Pro Texture(NVS5)"},
		{"NVT0","nVIDIA GeForce2 GTS Pro Texture(NVT0)"},
		{"NVT1","nVIDIA GeForce2 GTS Pro Texture(NVT1)"},
		{"NVT2","nVIDIA GeForce2 GTS Pro Texture(NVT2)"},
		{"NVT3","nVIDIA GeForce2 GTS Pro Texture(NVT3)"},
		{"NVT4","nVIDIA GeForce2 GTS Pro Texture(NVT4)"},
		{"NVT5","nVIDIA GeForce2 GTS Pro Texture(NVT5)"},
		{"PDVC","I/O DATA DVC"},
		{"PGVV","Radius Video Vision"},
		{"PHMO","IBM Photomotion"},
		{"PIM1","Pegasus Imaging Lossless JPEG(PIM1)"},
		{"PIM2","Pegasus Imaging Lossless JPEG(PIM2)"},
		{"PIMJ","Pegasus Imaging Lossless JPEG(PIMJ)"},
		{"PIXL","Pinnacle Video XL"},
		{"PNG1","CorePNG v8"},	// 2004-01-21 追加 BBS4900
		{"PVEZ","Horizons Technology PowerEZ"},
		{"PVMM","PacketVideo Corporation MPEG-4"},
		{"PVWV","Pegasus Wavelet Compression(PVWV)"},
		{"PVW2","Pegasus Wavelet Compression(PVW2)"},
		{"Q1.0","Q-Team QPEG(Q1.0)"},
		{"Q1.1","Q-Team QPEG(Q1.1)"},
		{"QPEQ","Q-Team QPEG(QPEQ)"},
		{"QPEG","Q-Team QPEG(QPEG)"},
		{"RAVI","GTRON ReferenceAVI(RAVI)"},
		{"RAV_","GTRON ReferenceAVI(RAV_)"},
		{"RGBT","Computer Concepts 32Bits RGB"},
		{"RLE ","Microsoft RLE"},
		{"RLE4","Microsoft RLE4"},
		{"RLE8","Microsoft RLE8"},
		{"RMP4","REALMagic MPEG4"},
		{"ROQV","Id RoQ File Video Decoder"},
		{"RPZA","Apple Video"},
		{"RT21","Intel Real Time Video 2.1"},
		{"RTV0","NewTek VideoToaster"},
		{"RUD0","nico Rududu"},
		{"RUSH","RushCodec"},
		{"RV10","RealVideo 1.0"},
		{"RV20","Real G2"},
		{"RV30","Real 8"},
		{"RV40","Real 9/10"},	//090627
		{"RVX ","intel RDX(RVX)"},
		{"S422","Tekram VideoCap C210 YUV"},
		{"SAN3","DivX 3.11a Copy"},
		{"SDCC","Sun Digital Camera"},
		{"SEDG","Samsung MPEG-4"},
		{"SFMC","CrystalNet Surface Fitting Method"},
		{"SHYU","Huffyuvs v2.1.1"},
		{"SMSC","Radius Proprietary"},
		{"SMSD","Radius Proprietary"},
		{"SMSV","WorldConnect Wavelet Video"},
		{"SNOW","SNOW"},
		{"SP40","Sunplus SP40"},
		{"SP44","Sunplus SP44"},
		{"SP53","Aiptek MegaCam(SP53)"},
		{"SP54","Aiptek MegaCam(SP54)"},
		{"SP55","Aiptek MegaCam(SP55)"},
		{"SP56","Aiptek MegaCam(SP56)"},
		{"SP57","Aiptek MegaCam(SP57)"},
		{"SP58","Aiptek MegaCam(SP58)"},
		{"SPIG","Radius Spigot"},
		{"SPLC","Splash Studios ACM"},
		{"SQZ2","Microsoft VXTreme V2"},
		{"SV10","Sorenson Video R1"},
		{"STVA","ST CMOS Imager Data (Bayer)"},
		{"STVB","ST CMOS Imager Data (Nudged Bayer)"},
		{"STVC","ST CMOS Imager Data (Bunched)"},
		{"STVX","ST CMOS Imager Data (Extended CODEC Data Format)"},
		{"STVY","ST CMOS Imager Data (Extended CODEC Data Format with Correction Data)"},
		{"SVQ1","Sorenson Video"},
		{"TLMS","TeraLogic Motion Intraframe(TLMS)"},
		{"TLST","TeraLogic Motion Intraframe(TLST)"},
		{"TM20","Duck TrueMotion 2.0"},
		{"TM2X","Duck TrueMotion 2X"},
		{"TMIC","TeraLogic Motion Intraframe(TMIC)"},
		{"TMOT","Horizons Technology TrueMotion S"},
		{"TR20","Duck TrueMotion RT 2.0"},
		{"TSCC","TechSmith Screen Capture"},
		{"TV10","Tecomac Low-Bit Rate"},
		{"TVJP","Truevision TARGA 2000(TVJP)"},
		{"TVMJ","Truevision TARGA 2000(TVMJ)"},
		{"TY2C","Trident Decompression(TY2C)"},
		{"TY2N","Trident Decompression(TY2N)"},
		{"TY0N","Trident Decompression(TY0N)"},
		{"U263","UB Video StreamForce"},
		{"UCOD","eMagix ClearVideo"},
		{"ULTI","IBM Ultimotion"},
		{"ULRG","Ut Video Codec RGB"},
		{"ULY0","Ut Video Codec YUV420"},
		{"ULY2","Ut Video Codec YUV422"},
		{"UMP4","DivX3/MS MPEG4-V1(UMP4)"},
		{"UYVY","Microsoft UYVY 4:2:2"},
		{"V261","Lucent VX2000S"},
		{"V422","Vitec Multimedia 24bitYUV"},
		{"V655","Vitec Multimedia 16bitYUV"},
		{"VCR1","ATI Video Codec 1.0"},
		{"VCR2","ATI Video Codec 2.0"},
		{"VCR3","ATI Video Codec 3.0"},
		{"VCR4","ATI Video Codec 4.0"},
		{"VCR5","ATI Video Codec 5.0"},
		{"VCR6","ATI Video Codec 6.0"},
		{"VCR7","ATI Video Codec 7.0"},
		{"VCR8","ATI Video Codec 8.0"},
		{"VCR9","ATI Video Codec 9.0"},
		{"VDCT","Vitec Video Maker Pro DIB"},
		{"VDEC","?(VDEC)"},
		{"VDOM","VDOWave"},
		{"VDOW","VDOLive"},
		{"VDST","VirtualDub remote frameclient"},
		{"VDTZ","Darim Vision VideoTizer YUV"},
		{"VGPX","Alaris Wee Cam"},
		{"VIDS","Vitec YUV 4:2:2 CCIR 601 for V422"},
		{"VIFP","?(VIFP)"},
		{"VIVO","Vivo H.263"},
		{"VIXL","Miro Video XL"},
		{"VLV1","VideoLogic?"},
		{"VP30","On2 Open-Source VP3(VP30)"},
		{"VP31","On2 Open-Source VP3(VP31)"},
		{"VP40","On2 VP4"},
		{"VP50","On2 VP5"},
		{"VP60","On2 VP6(Simple)"},
		{"VP61","On2 VP6(Advanced)"},
		{"VP62","On2 VP6(VP62)"},
		{"VP70","On2 VP7"},
		{"VSSH","H.264(VSSH)"},
		{"VQC1","ViewQuest VideoQuest Codec 1"},
		{"VQC2","ViewQuest VideoQuest Codec 2"},
		{"VSSV","Vanguard Software VSS Video"},
		{"VTLP","Alaris VideoGram QuickVideo VGPixel(VTLP)"},
		{"VUUU","?(VUUU)"},
		{"VX1K","Lucent VX1000S"},
		{"VX2K","Lucent VX2000S"},
		{"VXSP","Lucent VX1000SP"},
		{"VYU9","ATI YUV(VYU9)"},
		{"VYUY","ATI YUV(VYUY)"},
		{"WBVC","Winbond W9960"},
		{"WHAM","Microsoft Video 1(WHAM)"},
		{"WINX","Winnov Software Compression"},
		{"WJPG","AverMedia USB TV-tuner/capture"},
		{"WMV1","Windows Media Video 7"},
		{"WMV2","Windows Media Video 8"},
		{"WMV3","Windows Media Video 9 (WMV3)"},
		{"WMVA","WMV9 Advanced Profile (WMVA)"},
		{"WVC1","WMV9 Advanced Profile (WVC1)"},
		{"WNIX","WniWni Video3"},
		{"WNV1","Winnov Hardware Compression"},
		{"WRPR","VideoTools VideoServer Client Codec"},
		{"X263","Xirlink H.263 Video Codec"},
		{"X264","H.264(X264)"},
		{"XLV0","NetXL XL Video Decoder"},
		{"XLV1","Videologic VLCAP"},
		{"XMPG","XING IFlameOnry MPEG"},
		{"XTOR","Dxtory"},
		{"XVID","XviD MPEG4"},
		{"XVIX","DivX3/MS MPEG4-V2(XVIX)"},
		{"XVW0","XiWave Video(XVW0)"},
		{"XVW1","XiWave Video(XVW1)"},
		{"XVW2","XiWave Video(XVW2)"},
		{"XVW3","XiWave Video(XVW3)"},
		{"XVW4","XiWave Video(XVW4)"},
		{"XVW5","XiWave Video(XVW5)"},
		{"XVW6","XiWave Video(XVW6)"},
		{"XVW7","XiWave Video(XVW7)"},
		{"XVW8","XiWave Video(XVW8)"},
		{"XVW9","XiWave Video(XVW9)"},
		{"XXAN","?(XXAN)"},
		{"Y41P","Brooktree YUV 4:1:1"},
		{"Y8  ","Grayscale video"},
		{"Y211","Microsoft YUV 2:1:1"},
		{"Y411","Microsoft YUV 4:1:1 Packed"},
		{"Y41B","Microsoft YUV 4:1:1 Planar"},
		{"Y41P","Brooktree PCI 4:1:1"},
		{"Y41T","Brooktree PCI 4:1:1 with transparency"},
		{"Y42B","Weitek YUV 4:2:2"},
		{"Y42T","Brooktree PCI 4:2:2 with transparency"},
		{"YC12","Intel YUV12"},
		{"YU92","Intel YUV(YU92)"},
		{"YUV8","Winnov Caviar YUV8"},
		{"YUV9","Indeo Video Raw(YUV9)"},
		{"YUVP","uncompressed YCrCb 4:2:2"},
		{"YUY2","Microsoft Raw uncompressed YUV 4:2:2"},
		{"YUYV","Canopus YUVY"},
		{"YV12","Weitek YVU12"},
		{"YV16","Elecard YUV 4:2:2 Planar"},
		{"YV92","Intel Smart Video Recorder"},
		{"YVU9","Indeo Video Raw(YVU9)"},
		{"YVYU","Microsoft YVYU 4:2:2"},
		{"ZLIB","AVIzlib(LCL)"},
		{"ZPEG","Metheus Video Zipper"},
		{"ZYGO","ZyGo ZyGoVideo"},
		{"ZYYY","?(ZYYY)"},
//		{" ","無圧縮"}
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
		TRACE("%c%c%c%c %c%c%c%c %I64u(%lu)\n",
			(((char *)(&id))[0]),(((char *)(&id))[1]),(((char *)(&id))[2]),(((char *)(&id))[3]),
			(((char *)(&fType))[0]),(((char *)(&fType))[1]),(((char *)(&fType))[2]),(((char *)(&fType))[3]),
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
		TRACE("%c%c%c%c %c%c%c%c %I64u(%lu)\n",
			(((char *)(&id))[0]),(((char *)(&id))[1]),(((char *)(&id))[2]),(((char *)(&id))[3]),
			(((char *)(&fType))[0]),(((char *)(&fType))[1]),(((char *)(&fType))[2]),(((char *)(&fType))[3]),
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
			strFormatName = "PCM";
			return TRUE;
		}
		else if(guid == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
		{
			strFormatName = "PCM(IEEE FLOAT)";
			return TRUE;
		}
		else if(guid == KSDATAFORMAT_SUBTYPE_ALAW)
		{
			strFormatName = "CCITT A-Law";
			return TRUE;
		}
		else if(guid == KSDATAFORMAT_SUBTYPE_MULAW)
		{
			strFormatName = "CCITT u-Law";
			return TRUE;
		}
		else if(guid == KSDATAFORMAT_SUBTYPE_ADPCM)
		{
			strFormatName = "ADPCM";
			return TRUE;
		}
		else if(guid == KSDATAFORMAT_SUBTYPE_MPEG)
		{
			strFormatName = "MPEG";
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
			strVideoFormat = "DivX5";
		}
		else
		{
			strVideoFormat = "DivX4";
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
			char szTmp[256];
			WideCharToMultiByte(CP_ACP,
								0,
								icinfo.szDescription,
										-1,
										szTmp,
										sizeof(TCHAR)*256,NULL,NULL);
			strVideoFormat = szTmp;
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
				char szTmp[256];
				WideCharToMultiByte(CP_ACP,
									0,
									icinfo.szDescription,
											-1,
											szTmp,
											sizeof(TCHAR)*256,NULL,NULL);
				strVideoFormat = szTmp;
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

void GetAviFormat(IN const char *szFileName,
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
				strAuidoFormat.Format("%s, %ldbit, %ldHz, %s, %ldKbps",
							strFormatName,
							wfx.wBitsPerSample,
							wfx.nSamplesPerSec,
							(wfx.nChannels == 1)?"Mono":"Stereo",
							wfx.nAvgBytesPerSec*8/1000
							);
			}
			else
			{
				strAuidoFormat.Format("%s, %ldHz, %s, %ldKbps",
							strFormatName,
							wfx.nSamplesPerSec,
							(wfx.nChannels == 1)?"Mono":"Stereo",
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
	strStreamFormat.Format("%ldx%ld, %.03lffps, %ldFrames",
						AVIHeader.dwWidth,
						AVIHeader.dwHeight,
						1000000.0/(double )AVIHeader.dwMicroSecPerFrame,
						AVIHeader.dwTotalFrames/*-AVIHeader.dwInitialFrames*/);
	dwSec = (DWORD )(((__int64 )AVIHeader.dwMicroSecPerFrame*(__int64 )(AVIHeader.dwTotalFrames/*-AVIHeader.dwInitialFrames*/))/(1000*1000));
	strTime.Format("%02ld:%02ld:%02ld (%ldsec)",
					dwSec/60/60,
					dwSec/60%60,
					dwSec%60,
					dwSec);
	
	/////////////////////////////////////////////////////////////
	// Divx5のバージョンを区別する処理
	if(strVideoFormat.Compare("DivX5") == 0)
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
						strDivxVerEtc = "DivX5/6.\?.\?(";
						strDivxVerEtc += buildNo;
						strDivxVerEtc += ")";
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
	if(strVideoFormat.Compare("XviD MPEG4") == 0)
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
						strDivxVerEtc = "XviD (";
						strDivxVerEtc += buildNo;
						strDivxVerEtc += ")";
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

void GetWaveAudioFormat(IN const char *szFileName,
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
	
	HMMIO hmmio = mmioOpen((char *)szFileName,NULL,MMIO_COMPAT);
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
	if(mmioRead(hmmio,(char *)pwfx,mmckOutinfoSubchunk.cksize) == -1)
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
		strFormat.Format("%s, %ubit, %luHz, %uch, %luKbps",
					format,
					pwfx->wBitsPerSample,
					pwfx->nSamplesPerSec,
					pwfx->nChannels,
					pwfx->nAvgBytesPerSec*8/1000
					);
	}
	else
	{
		strFormat.Format("%s, %luHz, %uch, %luKbps",
					format,
					pwfx->nSamplesPerSec,
					pwfx->nChannels,
					pwfx->nAvgBytesPerSec*8/1000
					);
	}
	if(pwfx->nAvgBytesPerSec)
	{
		DWORD dwSec = dwStreamSize/(pwfx->nAvgBytesPerSec);
		strTime.Format("%ld:%02ld (%ldsec)",
						dwSec/60,
						dwSec%60,
						dwSec);
	}
	free(pwfx);
}

static const char *WINAMP_CLASS	= "Winamp v1.x";
static const char *WINAMP_PE_CLASS	= "Winamp PE";
static const char *SCMPX_CLASS = "SCMPX";

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

void PlayWinamp(HWND hWnd,char *szPlayFile)
{
	HWND hwndWinamp = FindWindow(WINAMP_CLASS,NULL);
	DWORD_PTR dwRet;
	if(hwndWinamp)
	{
		//Winamp専用再生コマンド
		COPYDATASTRUCT cds;

		cds.dwData = IPC_PLAYFILE; //Starts playback. A lot like hitting 'play' in Winamp, but not exactly the same
		cds.lpData = (void*)szPlayFile;
		cds.cbData = strlen(szPlayFile) + 1;
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
		SendMessageTimeout(hwndWinamp,WM_WA_IPC,0,IPC_DELETE,
			SMTO_ABORTIFHUNG | SMTO_BLOCK,100,&dwRet);
		for(DWORD i=0; i<=strlen(szPlayFile); i++)
		{
			SendMessageTimeout(hwndWinamp,WM_WA_IPC,(LPARAM)szPlayFile[i],IPC_PLAYFILE,
				SMTO_ABORTIFHUNG | SMTO_BLOCK,100,&dwRet);
		}
		SendMessageTimeout(hwndWinamp,WM_WA_IPC,0,IPC_STARTPLAY,
			SMTO_ABORTIFHUNG | SMTO_BLOCK,100,&dwRet);
		return;
	}
	ShellExecute(hWnd,"open",szPlayFile,NULL,NULL,SW_SHOWNORMAL);
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
int IsPlayingWinamp(char *szFileName)
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
				char *szWinampPl;
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

