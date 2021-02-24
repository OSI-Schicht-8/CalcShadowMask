# CalcShadowMask
Calculation of a shadow mask of an image

## source code
calcshadowmask.c can be compiled with vbcc for OS1.3
```shell
vc +kick13 calcshadowmask.c -o calcshadowmask
```

## description
The CalcShadowMask function uses the blitter to calculate a shadow mask. The following parameters are needed:
+UWORD *imgdata - a pointer to the source image data
+UWORD *shadowmask - a pointer to the destination image data (the final shadow mask)
+int depth - the number of bitplanes of the source image
+int width - the width (in words) of the source image
+int height - the height of the source images (number or rows)

OwnBlitter() takes exclusive access to the hardware blitter.

WaitBlit() waits until the blitter is ready.

BLTCON0 register contains the shift distance for source A (bits 15-12), sets the DMA channels (A-D) to be enabled (bits 11-8), sets the minterm (bits 7-0).
The minterm to be uses is A|B = D (0xfc), where A points to the source image and B points to the destination image D (the final shadow mask).

BLTCON1 register contains the shift distance for source B (bits 15-12) and some control bits for the blitter operation mode.

BLTAFWM and BLTALWM registers contain a mask for the first and last word of each line to be copied.

BLTAMOD, BLTBMOD and BLTDMOD registers contain the modulo for blitter source A,B and destination D. The number is this register is added to the address at the end of each line.

BLTAPT, BLTBPT and BLTDPT registers contain the pointers to the source image data and the destination image data.

BLTSIZE register contains the height and width of the blitter area to be processed. IF BLTSIZE register is set, the blitter stats its operation.

The function loops through all bitplanes of the source image. The shadow mask is updated each loop until it is completed after the last loop.

DisownBlitter() release exclusive access to the hardware blitter.
