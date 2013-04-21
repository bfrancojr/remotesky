#include <stdio.h>
#include <stdlib.h>
#include <wand/MagickWand.h>
#include "camerad_thumb.h"

static void throwWandException(MagickWand *wand)  { 
  ExceptionType severity; 
  char *description = MagickGetException(wand, &severity); 

  fprintf(stderr,"%s %s %lu %s\n", GetMagickModule(), description); 

  description = (char *) MagickRelinquishMemory(description); 
}

int camerad_thumb(char *photo_path, char *thumb_path) {
  MagickBooleanType status;
  MagickWand *magick_wand;

  MagickWandGenesis();
  magick_wand = NewMagickWand();
  status = MagickReadImage(magick_wand, photo_path);
  if (status == MagickFalse) {
    throwWandException(magick_wand);
    return -1;
  }

  MagickResetIterator(magick_wand);
  while (MagickNextImage(magick_wand) != MagickFalse) {
    MagickResizeImage(magick_wand, 300, 200, LanczosFilter, 1.0);
  }

  status = MagickWriteImages(magick_wand, thumb_path, MagickTrue);

  if (status == MagickFalse) {
    throwWandException(magick_wand);
    return -1;
  }

  magick_wand = DestroyMagickWand(magick_wand);
  MagickWandTerminus();

  return 0;
}
