#include "renderer.h"
#include "resources.h"
#include "graphic_functions.h"

void RenderRoundedRect2Alpha(unsigned int x, unsigned int y, unsigned int w, unsigned int h, color_t color)
{
	HorizontalLine(y, x + 2, x + w - 3, color);
	HorizontalLine(y + 1, x + 1, x + w - 1, color);
	FillRectangle(x, y + 2, w, h - 4, color);
	HorizontalLine(y + h - 2, x + 1, x + w - 1, color);
	HorizontalLine(y + h - 1, x + 2, x + w - 3, color);

	PixelAlpha(x, y, color, 10);
	PixelAlpha(x + w - 1, y, color, 10);
	PixelAlpha(x, y + h - 1, color, 10);
	PixelAlpha(x + w - 1, y + h - 1, color, 10);

	PixelAlpha(x + 1, y, color, 29);
	PixelAlpha(x, y + 1, color, 29);
	PixelAlpha(x + w - 2, y, color, 29);
	PixelAlpha(x + w - 1, y + 1, color, 29);
	PixelAlpha(x + 1, y + h - 1, color, 29);
	PixelAlpha(x, y + h - 2, color, 29);
	PixelAlpha(x + w - 2, y + h - 1, color, 29);
	PixelAlpha(x + w - 1, y + h - 2, color, 29);
}

void RenderRoundedRect3(unsigned int x, unsigned int y, unsigned int w, unsigned int h, color_t color, color_t color_bg_1, color_t color_bg_2, color_t color_bg_3)
{
	FillRectangle(x, y, w, h, color);

	Pixel(x, y, color_bg_1);
	Pixel(x + w - 1, y, color_bg_1);
	Pixel(x, y + h - 1, color_bg_1);
	Pixel(x + w - 1, y + h - 1, color_bg_1);

	Pixel(x + 1, y, color_bg_2);
	Pixel(x, y + 1, color_bg_2);
	Pixel(x + w - 2, y, color_bg_2);
	Pixel(x + w - 1, y + 1, color_bg_2);
	Pixel(x + 1, y + h - 1, color_bg_2);
	Pixel(x, y + h - 2, color_bg_2);
	Pixel(x + w - 2, y + h - 1, color_bg_2);
	Pixel(x + w - 1, y + h - 2, color_bg_2);

	Pixel(x + 2, y, color_bg_3);
	Pixel(x, y + 2, color_bg_3);
	Pixel(x + w - 3, y, color_bg_3);
	Pixel(x + w - 1, y + 2, color_bg_3);
	Pixel(x + 2, y + h - 1, color_bg_3);
	Pixel(x, y + h - 3, color_bg_3);
	Pixel(x + w - 3, y + h - 1, color_bg_3);
	Pixel(x + w - 1, y + h - 3, color_bg_3);
}

void CopySpriteAlpha(const color_t* data, const unsigned char* alpha, unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
	color_t* VRAM = (color_t*)GetVRAMAddress();
	VRAM += (LCD_WIDTH_PX * y) + x;

	for (unsigned int j = y; j < y + h; j++) {
		for (unsigned int i = x; i < x + w; i++) {
			if (*alpha)
				*VRAM = ((((*data & 0xF81F) * *alpha + (*VRAM & 0xF81F) * (32 - *alpha)) >> 5) & 0xF81F) |
				((((*data & 0x07E0) * *alpha + (*VRAM & 0x07E0) * (32 - *alpha)) >> 5) & 0x07E0);
			++VRAM;
			++data;
			++alpha;
		}
		VRAM += LCD_WIDTH_PX - w;
	}
}

void RenderBoard()
{
	RenderRoundedRect3(96, 24, 192, 192, COLOR_FIELD, COLOR_BG_FIELD_1, COLOR_BG_FIELD_2, COLOR_BG_FIELD_3);
}

void RenderBoardOverlay(color_t color, unsigned char alpha)
{
	HorizontalLineAlpha(24, 97, 286, color, alpha);
	for (unsigned int y = 25; y < 215; y++)
		HorizontalLineAlpha(y, 96, 287, color, alpha);
	HorizontalLineAlpha(215, 97, 286, color, alpha);
}

void RenderBackground()
{
	// Background left (with cutout for logo)
	FillRectangle(0, 24, 96, 6, COLOR_BG);
	FillRectangle(0, 30, 6, 24, COLOR_BG);
	FillRectangle(90, 30, 6, 24, COLOR_BG);
	FillRectangle(0, 54, 96, 162, COLOR_BG);

	// Background right
	FillRectangle(288, 24, 96, 192, COLOR_BG);

	// Board
	RenderBoard();

	// Background for Score & Best
	RenderRoundedRect3(6, 60, 84, 27, COLOR_FIELD, COLOR_BG_FIELD_1, COLOR_BG_FIELD_2, COLOR_BG_FIELD_3);
	RenderRoundedRect3(6, 93, 84, 27, COLOR_FIELD, COLOR_BG_FIELD_1, COLOR_BG_FIELD_2, COLOR_BG_FIELD_3);

	// Logo, Score & Best
	VRAM_CopySprite(LOGO_TEXTURE, RECT(LOGO));
	VRAM_CopySprite(SCORE_TEXTURE, RECT(SCORE));
	VRAM_CopySprite(BEST_TEXTURE, RECT(BEST));
}

void RenderTileBackground(unsigned int screenX, unsigned int screenY, color_t color, color_t color_bg_1, color_t color_bg_2, unsigned char cutoutX, unsigned char cutoutY, unsigned char cutoutW, unsigned char cutoutH)
{
	FillRectangle(screenX, screenY, 40, cutoutY, color);
	FillRectangle(screenX, screenY + cutoutY, cutoutX, cutoutH, color);
	FillRectangle(screenX + cutoutX + cutoutW, screenY + cutoutY, 40 - cutoutX - cutoutW, cutoutH, color);
	FillRectangle(screenX, screenY + cutoutY + cutoutH, 40, 40 - cutoutY - cutoutH, color);

	Pixel(screenX, screenY, color_bg_2);
	Pixel(screenX + 39, screenY, color_bg_2);
	Pixel(screenX, screenY + 39, color_bg_2);
	Pixel(screenX + 39, screenY + 39, color_bg_2);

	Pixel(screenX + 1, screenY, color_bg_1);
	Pixel(screenX, screenY + 1, color_bg_1);
	Pixel(screenX + 38, screenY, color_bg_1);
	Pixel(screenX + 39, screenY + 1, color_bg_1);
	Pixel(screenX + 1, screenY + 39, color_bg_1);
	Pixel(screenX, screenY + 38, color_bg_1);
	Pixel(screenX + 38, screenY + 39, color_bg_1);
	Pixel(screenX + 39, screenY + 38, color_bg_1);
}

void RenderTileBackground(unsigned int screenX, unsigned int screenY, unsigned char type, unsigned char cutoutX, unsigned char cutoutY, unsigned char cutoutW, unsigned char cutoutH)
{
	RenderTileBackground(screenX, screenY, TILE_COLORS[type * 3], TILE_COLORS[type * 3 + 1], TILE_COLORS[type * 3 + 2], cutoutX, cutoutY, cutoutW, cutoutH);
}

void RenderTile(unsigned int x, unsigned int y, unsigned char type)
{
	unsigned int targetX = 103 + x * 46;
	unsigned int targetY = 31 + y * 46;

	RenderTileBackground(targetX, targetY, type,
		TILE_CUTOUTS[type * 4],
		TILE_CUTOUTS[type * 4 + 1],
		TILE_CUTOUTS[type * 4 + 2],
		TILE_CUTOUTS[type * 4 + 3]);

	if (TILE_TEXTURES[type])
		VRAM_CopySprite(TILE_TEXTURES[type],
			targetX + TILE_CUTOUTS[type * 4],
			targetY + TILE_CUTOUTS[type * 4 + 1],
			TILE_CUTOUTS[type * 4 + 2],
			TILE_CUTOUTS[type * 4 + 3]);
}

void RenderScoreText(unsigned int x, unsigned int y, const unsigned char* digits, unsigned char count)
{
	for (unsigned char i = 0; i < count; i++) {
		unsigned char digit = digits[i];

		VRAM_CopySprite(SCORE_FONT_TEXTURES[digit], x, y, SCORE_FONT_WIDTHS[digit], SCORE_FONT_HEIGHT);

		x += SCORE_FONT_WIDTHS[digit];
	}
}

unsigned int GetScoreTextWidth(const unsigned char* digits, unsigned char count)
{
	unsigned int width = 0;

	for (unsigned char i = 0; i < count; i++)
		width += SCORE_FONT_WIDTHS[digits[i]];

	return width;
}
