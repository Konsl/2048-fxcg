#ifndef _GRAPHIC_FUNCTIONS
#define _GRAPHIC_FUNCTIONS

#define LCD_WIDTH_PX 384
#define LCD_HEIGHT_PX 216

void FillVRAM(short color);
void Pixel(int x, int y, short color);
void PixelAlpha(int x, int y, short color, char alpha);
void HorizontalLine(int y, int x1, int x2, unsigned short color);
void VerticalLine(int x, int y1, int y2, unsigned short color);
void HorizontalLineAlpha(int y, int x1, int x2, short color, char alpha);
void Line(int x1, int y1, int x2, int y2, short color);
void Polygon(const int *x, const int *y, int nb_vertices, short color);
void FilledConvexPolygon(const int* x, const int* y, int nb_vertices, short color);
void CopySpriteMasked(const short* bitmap, int x, int y, int width, int height, short mask);
void RoundedSquareAlpha(int x, int y, int w, int h, int radius, short color, char alpha);
void FillRectangle(unsigned int x, unsigned int y, unsigned int w, unsigned int h, short color);

#endif //_GRAPHIC_FUNCTIONS
