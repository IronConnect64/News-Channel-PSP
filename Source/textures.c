#include "textures.h"

extern unsigned char arrow_down_png_start[], arrow_up_png_start[], background_png_start[];

extern unsigned int arrow_down_png_size, arrow_up_png_size, background_png_size;

void Textures_Load(void)
{
	arrow_down = g2dTexLoadMemory(arrow_down_png_start, arrow_down_png_size, G2D_SWIZZLE);
	arrow_up = g2dTexLoadMemory(arrow_up_png_start, arrow_up_png_size, G2D_SWIZZLE);
	background = g2dTexLoadMemory(background_png_start, background_png_size, G2D_SWIZZLE);
}

void Textures_Free(void)
{
	g2dTexFree(&arrow_down);
	g2dTexFree(&arrow_up);
	g2dTexFree(&background);
}
