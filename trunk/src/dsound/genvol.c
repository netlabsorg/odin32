/* $Id: genvol.c,v 1.2 2000-08-02 15:48:27 bird Exp $ */

/* A quick-n'-dirty DirectSound volume conversion table generator */

#include <stdio.h>
#include <math.h>

void main (void)
{
   FILE *f;
   int   i, j;

   if ((f = fopen("DSVolume.h", "w")) == NULL) {
      printf("Can't open output file\n");
      return;
   }

   fprintf(f, "\n// This is a generated file. Do not edit!\n");
   fprintf(f, "\n// Volume conversion table for Odin DirectSound\n");
   fprintf(f, "\nBYTE VolTable[1000] = {\n");

   for (i = 0; i < 100; i++) {
      fprintf(f, "   ");
      for (j = 0; j < 10; j++) {
         fprintf(f, "%3d", (unsigned char)(255.0 * pow(4, -(i * 40.0 + j * 4) / 1000.0)));
         if (i * j != 99 * 9)
            fprintf(f, ", ");
      }
      fprintf(f, "\n");
   }
   fprintf(f, "\n};\n");

   fclose(f);
}
