#include <proto/exec.h>
#include <proto/graphics.h>
#include <hardware/custom.h>
#include <stdio.h>

#define custom ((struct Custom *) (0xdff000))

struct GfxBase *GfxBase;

void CalcShadowMask(UWORD *imgdata, UWORD *shadowmask, int depth, int width, int height);

int main()
{
	int width = 1; /* image width in words */
	int height = 3; /* image height in rows */
	int depth = 3; /* number of bitplanes */

	UWORD *imgdata = AllocMem(depth*2*width*height, MEMF_CHIP|MEMF_CLEAR); /* allocate memory for source image */
	UWORD *shadowmask = AllocMem(2*width*height, MEMF_CHIP|MEMF_CLEAR); /* allocate memory for shadow mask */

	GfxBase = (struct GfxBase *)OpenLibrary ("graphics.library", 0L);
	
	/* example image data */
	
	/* bitplane 0 */
	imgdata[0] = 0x8001; /* row 0 */
	imgdata[1] = 0x0000; /* row 1 */
	imgdata[2] = 0x0000; /* row 2 */
	
	/* bitplane 1 */
	imgdata[3] = 0x0000; /* row 0 */
	imgdata[4] = 0x4002; /* row 1 */
	imgdata[5] = 0x0000; /* row 2 */
	
	/* bitplane 2 */
	imgdata[6] = 0x0000; /* row 0 */
	imgdata[7] = 0x0000; /* row 1 */
	imgdata[8] = 0x2004; /* row 2 */
	
	CalcShadowMask(imgdata, shadowmask, depth, width, height); /* calculate shadow mask for source image */
	
	printf("row 0: %x\nrow 1: %x\nrow 2: %x\n",shadowmask[0],shadowmask[1],shadowmask[2]); /* print calculated shadow mask values */
	
	CloseLibrary ((struct Library *) GfxBase);
}

void CalcShadowMask(UWORD *imgdata, UWORD *shadowmask, int depth, int width, int height)
{
	int planesize;
	int blittersize;
	
	OwnBlitter(); /* take control over blitter */
	WaitBlit();
	
	custom->bltcon0 = 0x0dfc; /* use source A, source B and destination D (0x0d); minterm 0xfc : A|B = D */
	custom->bltcon1 = 0x0000; /* no shift of source B */
	custom->bltafwm = 0xffff; /* full first word mask */
	custom->bltalwm = 0xffff; /* full last word mask */
	custom->bltamod = 0x0000; /* no modulus for source A */
	custom->bltbmod = 0x0000; /* no modulus for source B */
	custom->bltdmod = 0x0000; /* no modulus for destination D */
	
	planesize = width * height; /* size of one bitplane in words */
	
	height = height & 0x03ff; /* for special case heigh = 1024 */
	width = width & 0x003f; /* for special case width = 64 */
	
	blittersize = height<<6; /* shift height to bits 15 - 6 */
	blittersize = blittersize + width; /* width in bits 5 - 0 */
	do
	{
		custom->bltapt = imgdata; /* source A points to source image bitplane */
		custom->bltbpt = shadowmask; /* source B points to shadowmask image */
		custom->bltdpt = shadowmask; /* destination D points to shadowmask image */
		
		custom->bltsize = blittersize; /* start blitter operation */
		WaitBlit(); /* wait until blitter has finished operation */
		
		depth = depth - 1; /* loop counter */
		imgdata = imgdata + planesize; /* set source image pointer to next bitplane address */
				
	} while (depth); /* loop through all source image bitplanes */
	
	DisownBlitter(); /* free blitter */
}