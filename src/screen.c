/*******************************************
    screen.c
*******************************************/

#include <_string.h> // for memcpy() only
#include "screen.h"
#include "../hal/hal.h"


#define	VBE_MODE_INFO   0x00001500  /* location where information was stored */
#define VBE_VERSION     0x00001600  /* known from the bootloader. */
#define SCREEN_BUFFER   0x00800000  /* 0x00800000 <= screen buffer <= 0x01000000 */
/* kernel.c and screen.c must agree on value of SCREEN_BUFFER */


struct _vbeModeInfo                 // VESA Bios Extension
{
    uint16b ModeAttributes;         // mode attributes (flags)
    uint8b  WinAAttributes;         // window A attributes
    uint8b  WinBAttributes;         // window B attributes
    uint16b WinGranularity;         // window granularity
    uint16b WinSize;                // window size
    uint16b WinASegment;            // window A start segment
    uint16b WinBSegment;            // window B start segment
    uintptrb WinFuncPtr;            // pointer to window function
    uint16b BytesPerScanLine;       // bytes per scan line
    uint16b XResolution;            // horizontal resolution in pixels or chars
    uint16b YResolution;            // vertical resolution in pixels or chars
    uint8b  XCharSize;              // character cell width in pixels
    uint8b  YCharSize;              // character cell height in pixels
    uint8b  NumberOfPlanes;         // number of memory planes
    uint8b  BitsPerPixel;           // bits per pixel
    uint8b  NumberOfBanks;          // number of banks
    uint8b  MemoryModel;            // memory model type
    uint8b  BankSize;               // bank size in KB
    uint8b  NumberOfImagePages;     // number of images
    uint8b  _Reserved1;             // reserved for page function
    uint8b  RedMaskSize;            // size of direct colour red mask in bits
    uint8b  RedFieldPosition;       // bit position of lsb of red mask
    uint8b  GreenMaskSize;          // size of direct colour green mask in bits
    uint8b  GreenFieldPosition;     // bit position of lsb of green mask
    uint8b  BlueMaskSize;           // size of direct colour blue mask in bits
    uint8b  BlueFieldPosition;      // bit position of lsb of blue mask
    uint8b  RsvdMaskSize;           // size of direct colour reserved mask in bits
    uint8b  RsvdFieldPosition;      // bit position of lsb of reserved mask
    uint8b  DirectColorModeInfo;    // direct colour mode attributes
    uintptrb PhysBasePtr;           // physical address for flat frame buffer
    uint32b OffScreenMemOffset;     // pointer to start of off screen memory
    uint16b OffScreenMemSize;       // amount of off screen memory in 1k units
    //    remainder of ModeInfoBlock = 206 bytes

    uint8b  BPP;                    // Bytes per pixel, not bits per pixel
    uint16b VbeVersion;
};
struct _vbeModeInfo VBE;


struct _screen_info SCR;        // SCReen information, also used outside this file

uint32b* ScreenBuffer;



/* Load features from bootloader
 * Note: MUST NOT CALL MALLOC */
void screen_install ()
{
    int i;
    memcpy( &VBE, (const void*)VBE_MODE_INFO, sizeof(VBE) );    // get VBE information from bootloader
    VBE.BPP = VBE.BitsPerPixel/8;
    VBE.VbeVersion = *((uint16b *)VBE_VERSION);

    ScreenBuffer = (uint32b*)SCREEN_BUFFER;
    i = VBE.XResolution*VBE.YResolution;
    for(i--;i>=0;i--) ScreenBuffer[i]=0;

    SCR.XRes = VBE.XResolution;
    SCR.YRes = VBE.YResolution - FREE_YZONE;
}



void screen_putpixels (const int* pixels_array, int XStart, int YStart, int XSize, int YSize, int row_stride)
{
    int y;
    if(pixels_array == NULL) return;

    if(XStart < 0) { XSize += XStart; XStart = 0; }
    if(YStart < 0) { YSize += YStart; YStart = 0; }
    if(XSize >= SCR.XRes) XSize = SCR.XRes-1;
    if(YSize >= SCR.YRes) YSize = SCR.YRes-1;

    for(y = YStart; y < YSize+YStart; y++)
    {
        memcpy (ScreenBuffer + y*SCR.XRes + XStart, pixels_array, XSize*sizeof(int));
        pixels_array += row_stride;
    }
}



void screen_putpixel(int x, int y, int colour)  // put one pixel on screen buffer
{ ScreenBuffer[y*SCR.XRes + x] = (uint32b)colour; }



extern uint8b inportb(uint16b port_id); // from hal/hal.c

void screen_refresh()                   // update changes of screen buffer to real screen
{
    int i, j;
    char *ptr1, *ptr2;

    if(VBE.BitsPerPixel==32)
    {
        i = VBE.XResolution*VBE.YResolution*sizeof(uint32b);
        while((inportb(0x3DA) & 0x08));
        while(!(inportb(0x3DA) & 0x08));
        memcpy((void *)VBE.PhysBasePtr, (void *)ScreenBuffer, i);
    }
    else if(VBE.BitsPerPixel==24)
    {
        ptr1 = (char *)VBE.PhysBasePtr;
        ptr2 = (char *)ScreenBuffer;
        while((inportb(0x3DA) & 0x08));
        while(!(inportb(0x3DA) & 0x08));

        for(i=0; i<VBE.XResolution; i++)
        for(j=0; j<VBE.YResolution; j++)
          { *ptr1++ = *ptr2++;
            *ptr1++ = *ptr2++;
            *ptr1++ = *ptr2++;
            ptr2++;
          }
    }
}
