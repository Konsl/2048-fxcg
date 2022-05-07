#pragma once
#include <fxcg/display.h>

void RenderRoundedRect2Alpha(unsigned int x, unsigned int y, unsigned int w, unsigned int h, color_t color);
void RenderRoundedRect3(unsigned int x, unsigned int y, unsigned int w, unsigned int h, color_t color, color_t color_bg_1, color_t color_bg_2, color_t color_bg_3);
void CopySpriteAlpha(const color_t* data, const unsigned char* alpha, unsigned int x, unsigned int y, unsigned int w, unsigned int h);

void RenderBoard();
void RenderBoardOverlay(color_t color, unsigned char alpha);
void RenderBackground();

// Doesn't check for invalid cutouts
void RenderTileBackground(unsigned int screenX, unsigned int screenY, color_t color, color_t color_bg_1, color_t color_bg_2, unsigned char cutoutX, unsigned char cutoutY, unsigned char cutoutW, unsigned char cutoutH);
void RenderTileBackground(unsigned int screenX, unsigned int screenY, unsigned char type, unsigned char cutoutX, unsigned char cutoutY, unsigned char cutoutW, unsigned char cutoutH);
void RenderTile(unsigned int x, unsigned int y, unsigned char type);

void RenderScoreText(unsigned int x, unsigned int y, const unsigned char* digits, unsigned char count);
unsigned int GetScoreTextWidth(const unsigned char* digits, unsigned char count);