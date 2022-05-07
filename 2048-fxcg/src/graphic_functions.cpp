#include <stdlib.h>
#include <fxcg/display.h>
#include "graphic_functions.h"

void FillVRAM(short color)
{
	int i, color32bits, * VRAM = (int*)GetVRAMAddress();
	color32bits = (color << 16) | color;
	for (i = LCD_WIDTH_PX * LCD_HEIGHT_PX / 2; i; i--)
		*(VRAM++) = color32bits;
}

void Pixel(int x, int y, short color)
{
	short* VRAM = (short*)GetVRAMAddress();
	if (x >= 0 && x < LCD_WIDTH_PX && y >= 0 && y < LCD_HEIGHT_PX)
		*(VRAM + y * LCD_WIDTH_PX + x) = color;
}

void PixelAlpha(int x, int y, short color, char alpha)
{
	short* VRAM = (short*)GetVRAMAddress() + (y * LCD_WIDTH_PX + x);
	if (x >= 0 && x < LCD_WIDTH_PX && y >= 0 && y < LCD_HEIGHT_PX)
		*VRAM = ((((color & 0xF81F) * alpha + (*VRAM & 0xF81F) * (32 - alpha)) >> 5) & 0xF81F) |
		((((color & 0x07E0) * alpha + (*VRAM & 0x07E0) * (32 - alpha)) >> 5) & 0x07E0);
}

void HorizontalLine(int y, int x1, int x2, unsigned short color)
{
	unsigned int i, color32, * VRAM32;
	short* VRAM = (short*)GetVRAMAddress();
	if (y<0 || y>LCD_HEIGHT_PX || (x1 < 0 && x2 < 0) || (x1 > LCD_WIDTH_PX && x2 > LCD_WIDTH_PX)) return;
	if (x1 > x2) { i = x1; x1 = x2; x2 = i; }
	if (x1 < 0) x1 = 0;
	if (x2 >= LCD_WIDTH_PX) x2 = LCD_WIDTH_PX - 1;
	VRAM32 = (unsigned int*)((VRAM += y * LCD_WIDTH_PX));
	if (x1 != x2) {
		color32 = (color << 16) | color;
		VRAM32 += (x1 + 1) >> 1;
		if (x1 & 1) *(VRAM + x1) = color;
		if (!(x2 & 1)) *(VRAM + x2) = color;
		for (i = ((x2 + 1) >> 1) - ((x1 + 1) >> 1); i; i--)
			(*VRAM32++) = color32;
	}
	else *(VRAM + x1) = color;
}

void VerticalLine(int x, int y1, int y2, unsigned short color)
{
	short* VRAM = (short*)GetVRAMAddress() + y1 * LCD_WIDTH_PX + x;
	int y_index;
	for (y_index = y1; y_index < y2; y_index++, VRAM += LCD_WIDTH_PX)
		*VRAM = color;
}

void HorizontalLineAlpha(int y, int x1, int x2, short color, char alpha)
{
	unsigned int i;
	short* VRAM = (short*)GetVRAMAddress();
	if (y<0 || y>LCD_HEIGHT_PX || (x1 < 0 && x2 < 0) || (x1 > LCD_WIDTH_PX && x2 > LCD_WIDTH_PX)) return;
	if (x1 > x2) { i = x1; x1 = x2; x2 = i; }
	if (x1 < 0) x1 = 0;
	if (x2 >= LCD_WIDTH_PX) x2 = LCD_WIDTH_PX - 1;
	VRAM += y * LCD_WIDTH_PX + x1;
	for (i = x2 - x1 + 1; i; i--) {
		*VRAM = ((((color & 0xF81F) * alpha + (*VRAM & 0xF81F) * (32 - alpha)) >> 5) & 0xF81F) |
			((((color & 0x07E0) * alpha + (*VRAM & 0x07E0) * (32 - alpha)) >> 5) & 0x07E0);
		++VRAM;
	}
}

void Line(int x1, int y1, int x2, int y2, short color)
{
	int i, x, y, dx, dy, sx, sy, cumul;
	x = x1;
	y = y1;
	dx = x2 - x1;
	dy = y2 - y1;
	sx = (dx > 0) ? 1 : -1;
	sy = (dy > 0) ? 1 : -1;
	dx = (dx > 0) ? dx : -dx;
	dy = (dy > 0) ? dy : -dy;
	Pixel(x, y, color);
	if (dx > dy) {
		cumul = dx >> 1;
		for (i = 1; i < dx; i++) {
			x += sx;
			cumul += dy;
			if (cumul > dx) {
				cumul -= dx;
				y += sy;
			}
			Pixel(x, y, color);
		}
	}
	else {
		cumul = dy >> 1;
		for (i = 1; i < dy; i++) {
			y += sy;
			cumul += dx;
			if (cumul > dy) {
				cumul -= dy;
				x += sx;
			}
			Pixel(x, y, color);
		}
	}
}

void Polygon(const int* x, const int* y, int nb_vertices, short color)
{
	int i;
	if (nb_vertices < 3) return;
	for (i = 0; i < nb_vertices - 1; i++)
		Line(x[i], y[i], x[i + 1], y[i + 1], color);
	Line(x[i], y[i], x[0], y[0], color);
}

void FilledConvexPolygon(const int* x, const int* y, int nb_vertices, short color)
{
	int i, ymin, ymax, xmin2, xmax2, * xmin, * xmax;
	char* empty;
	if (nb_vertices < 3) return;
	ymin = ymax = y[0];
	xmin2 = xmax2 = x[0];
	for (i = 0; i < nb_vertices; i++) {
		if (y[i] < ymin) ymin = y[i];
		if (y[i] > ymax) ymax = y[i];
		if (x[i] < xmin2) xmin2 = x[i];
		if (x[i] > xmax2) xmax2 = x[i];
	}
	if (xmax2<0 || xmin2>LCD_WIDTH_PX || ymax<0 || ymin>LCD_HEIGHT_PX) return;
	xmin = (int*)malloc((ymax - ymin + 1) * sizeof(int));
	xmax = (int*)malloc((ymax - ymin + 1) * sizeof(int));
	empty = (char*)malloc(ymax - ymin + 1);
	if (xmin && xmax && empty) {
		for (i = 0; i < ymax - ymin + 1; i++) empty[i] = 1;
		for (i = 0; i < nb_vertices; i++) {
			int j, px, py, dx, dy, sx, sy, cumul;
			px = x[i];
			py = y[i];
			dx = x[(i + 1) % nb_vertices] - px;
			dy = y[(i + 1) % nb_vertices] - py;
			sx = (dx > 0) ? 1 : -1;
			sy = (dy > 0) ? 1 : -1;
			dx = (dx > 0) ? dx : -dx;
			dy = (dy > 0) ? dy : -dy;
			if (empty[py - ymin]) xmax[py - ymin] = xmin[py - ymin] = px, empty[py - ymin] = 0; else xmax[py - ymin] = px;
			if (dx > dy) {
				cumul = dx >> 1;
				for (j = 1; j < dx; j++) {
					px += sx;
					cumul += dy;
					if (cumul > dx) cumul -= dx, py += sy;
					if (empty[py - ymin]) xmax[py - ymin] = xmin[py - ymin] = px, empty[py - ymin] = 0; else xmax[py - ymin] = px;
				}
			}
			else {
				cumul = dy >> 1;
				for (j = 1; j < dy; j++) {
					py += sy;
					cumul += dx;
					if (cumul > dy) cumul -= dy, px += sx;
					if (empty[py - ymin]) xmax[py - ymin] = xmin[py - ymin] = px, empty[py - ymin] = 0; else xmax[py - ymin] = px;
				}
			}
		}
		for (i = 0; i < ymax - ymin + 1; i++)
			HorizontalLine(ymin + i, xmin[i], xmax[i], color);
	}
	free(xmin);
	free(xmax);
	free(empty);
}

void CopySpriteMasked(const short* bitmap, int x, int y, int width, int height, short mask)
{
	short* VRAM = (short*)GetVRAMAddress();

	int y_index;
	int x_index;
	short* base = y * LCD_WIDTH_PX + x + VRAM;
	for (y_index = height; y_index > 0; --y_index, base += LCD_WIDTH_PX - width) {
		for (x_index = width; x_index > 0; --x_index, ++base, ++bitmap) {
			if (*bitmap != mask) *base = *bitmap;
		}
	}
}

void RoundedSquareAlpha(int x, int y, int w, int h, int radius, short color, char alpha)
{
	int plot_x, plot_y, d;
	if (radius < 0) radius = 0;
	if ((radius << 1) > w) radius = w >> 1;
	if ((radius << 1) > h) radius = h >> 1;
	plot_x = 0;
	plot_y = radius;
	d = 1 - radius;
	while (plot_y > plot_x) {
		if (d < 0)
			d += 2 * plot_x + 3;
		else {
			d += 2 * (plot_x - plot_y) + 5;
			plot_y--;
			HorizontalLineAlpha(y + h - 1 - radius + plot_y + 1, x + radius - plot_x, x + w - 1 - radius + plot_x, color, alpha);
			HorizontalLineAlpha(y + radius - plot_y - 1, x + radius - plot_x, x + w - 1 - radius + plot_x, color, alpha);
		}
		plot_x++;
		if (plot_y >= plot_x)
		{
			HorizontalLineAlpha(y + h - 1 - radius + plot_x, x + radius - plot_y, x + w - 1 - radius + plot_y, color, alpha);
			HorizontalLineAlpha(y + radius - plot_x, x + radius - plot_y, x + w - 1 - radius + plot_y, color, alpha);
		}
	}
	for (d = y + radius; d < y + h - radius; d++)
		HorizontalLineAlpha(d, x, x + w - 1, color, alpha);
}

void FillRectangle(unsigned int x, unsigned int y, unsigned int w, unsigned int h, short color)
{
	if (w == 0 || h == 0 || x >= LCD_WIDTH_PX || y > LCD_HEIGHT_PX)
		return;

	unsigned int x_start = x;
	unsigned int x_end = x + w;
	unsigned int y_start = y;
	unsigned int y_end = y + h;

	if (x_end > LCD_WIDTH_PX)
		x_end = LCD_WIDTH_PX;
	if (y_end > LCD_HEIGHT_PX)
		y_end = LCD_HEIGHT_PX;

	unsigned int y_index;
	x_end--;
	for (y_index = y_start; y_index < y_end; y_index++)
		HorizontalLine(y_index, x_start, x_end, color);
}
