#include "MAGFX.h"
#include "string.h"
#include "gfx.h"
#include "colours.h"
#include "diskalphamask_bin.h"
#include "MAFontRobotoRegular.h"

int panelAlphaTop;
int panelAlphaBottom;
bool panelsDrawn = false;

void MAGFXImageWithRGBAndAlphaMask(u8 r, u8 g, u8 b, u8* GRBAAlphaSource, u8* dest, int width, int height) {
    int alphaSourceByte = 0;
    int destByte = 0;
    int len = width*height;
    
    while (alphaSourceByte < len) {
        memcpy(&dest[destByte], &b, 1); destByte++;
        memcpy(&dest[destByte], &g, 1); destByte++;
        memcpy(&dest[destByte], &r, 1); destByte++;
        memcpy(&dest[destByte], &GRBAAlphaSource[alphaSourceByte], 1); destByte++;
        
        alphaSourceByte ++;
    }
}

void MAGFXApplyAlphaMask(u8* GRBImageSource, u8* GRBAAlphaSource, u8* dest, int width, int height) {
    int GRBSourceByte = 0;
    int alphaSourceByte = 0;
    int destByte = 0;
    int len = width*height*3;
    
    while (GRBSourceByte < len) {
        memcpy(&dest[destByte], &GRBImageSource[GRBSourceByte], 3);
        destByte += 3;
        GRBSourceByte += 3;
        
        memcpy(&dest[destByte], &GRBAAlphaSource[alphaSourceByte], 1);
        destByte += 1;
        alphaSourceByte += 1;
    }
}

void MAGFXTranslucentRect(int width, int height, int r, int g, int b, int a, u8* dest) {
    int len = width * height * 4;
    int byte = 0;
    
    while (byte < len) {
        memcpy(&dest[byte], &b, 1); byte++;
        memcpy(&dest[byte], &g, 1); byte++;
        memcpy(&dest[byte], &r, 1); byte++;
        memcpy(&dest[byte], &a, 1); byte++;
    }
}

int panelEdgeOffset = 20;
int panelHeightTop = 360;
int panelHeightBottom = 280;
int panelWidthTop = 182;
int panelWidthBottom = 200;
u8 panelPixel[4]; // 360 * 182 * 4
//u8 panelBottom[203840]; // 280 * 182 * 4

void MAGFXDrawPanel(gfxScreen_t screen) {
    if (!panelsDrawn) {
        int panelColour = 128;
        int panelAlpha = (screen == GFX_TOP) ? panelAlphaTop : panelAlphaBottom;
        
        panelPixel[0] = panelColour;
        panelPixel[1] = panelColour;
        panelPixel[2] = panelColour;
        panelPixel[3] = panelAlpha;
    }
    
    if (screen == GFX_TOP && panelAlphaTop == 0) {
        return;
    }
    
    if (screen == GFX_BOTTOM && panelAlphaBottom == 0) {
        return;
    }

    int drawWidth;
    int drawHeight;
    
    if (screen == GFX_TOP) {
        drawWidth = panelWidthTop;
        drawHeight = panelHeightTop;
    }
    else {
        drawWidth = panelWidthBottom;
        drawHeight = panelHeightBottom;
    }
    
    int x, y;
    
    int totalWidth = drawWidth + panelEdgeOffset;
    int totalHeight = drawHeight + panelEdgeOffset;
    
    for (y = panelEdgeOffset; y < totalHeight; y++) {
        for (x = panelEdgeOffset; x < totalWidth; x++) {
            gfxDrawSpriteAlphaBlend(screen, GFX_LEFT, panelPixel, 1, 1, x, y);
        }
    }
}

u8 disk[64*64*4];
bool diskDrawn = false;

void drawDisk(char * text) {
    int diskX = (240-64)/2;
    
    if (!diskDrawn) {
        diskDrawn = true;
        rgbColour * tint = tintColour();
        MAGFXImageWithRGBAndAlphaMask(tint->r, tint->g, tint->b, (u8*)diskalphamask_bin, disk, 64, 64);
    }
    
    gfxDrawSpriteAlphaBlend(GFX_BOTTOM, GFX_LEFT, disk, 64, 64, diskX, (320-64)/2);
    
    int len = MATextWidthInPixels(text, &MAFontRobotoRegular16);
    rgbColour * dark = darkTextColour();
    MADrawText(GFX_BOTTOM, GFX_LEFT, diskX - 32, (320/2)-(len/2), text, &MAFontRobotoRegular16, dark->r, dark->g, dark->b);
}


//u8 * MAGFXScaleSprite(u8* spriteData, u8* newSpriteData, u16 width, u16 height, int scaleFactor, bool alpha) {
//    int sourcePos = 0;
//    int destPos = 0;
//    int len = alpha ? 4 : 3;
//    int lineCount = 0;
//    int lineSize = height * len;
//    int newLineSize = lineSize * scaleFactor;
//    int start = 0;
//    int count;
//
//    //While there are still lines left to process
//    while (lineCount < width) {
//        //Repeat scaleFactor times
//        for (count=0; count<scaleFactor; count++) {
//            //Start at the source position in the current line, plus all the previous lines
//            start = sourcePos + (lineSize * lineCount);
//            //Copy one pixel to the end of the new data
//            memcpy(&newSpriteData[destPos], &spriteData[start], len);
//            //Move to the next pixel in the destination (but not the source - the same one might need to be drawn again)
//            destPos += len;
//        }
//
//        //That pixel has been drawn enough times for the line. Move to the next pixel
//        sourcePos += len;
//
//        //If the source position is now off the end of a line...
//        if (sourcePos >= lineSize) {
//            //Move back to the beginning of the line
//            sourcePos = 0;
//            //Move up a line
//            lineCount++;
//
//            //Repeat scaleFactor-1 times
//            for (count=0; count<(scaleFactor-1); count++) {
//                //Copy the whole of the line just completed in the destination to the end of the destination again
//                memcpy(&newSpriteData[destPos], &newSpriteData[destPos-newLineSize], newLineSize);
//                //Move to the next line in the destination
//                destPos += newLineSize;
//            }
//        }
//    }
//
//    return newSpriteData;
//}