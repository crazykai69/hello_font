/*
Copyright (c) 2012, Broadcom Europe Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// Test app for VG font library.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "bcm_host.h"
#include "vgfont.h"

int32_t render_subtitle(GRAPHICS_RESOURCE_HANDLE img, int justify, const char *text, const uint32_t text_size_in, const uint32_t y_offset, uint cr, uint cg, uint cb, uint32_t left_offset)
{
   uint32_t text_length = strlen(text);
   uint32_t width=0, height=0;
   const char *split = text;
   int32_t s=0;
   uint32_t img_w, img_h, left;
   uint32_t text_size = text_size_in;
   
   graphics_get_resource_size(img, &img_w, &img_h);
   
   text_size = img_h * text_size_in / 100;

   if (text_length==0)
      return 0;
  
   s = graphics_resource_text_dimensions_ext(img, split, text_length, &width, &height, text_size);
   if (s != 0) return s;
   
   if (width) {
    
     if (justify == 0) left = left_offset + 60;
     if (justify == 1) left = (img_w - width - 60);
    
     s = graphics_resource_render_text_ext(img, left, y_offset * img_h / 100,
                                     GRAPHICS_RESOURCE_WIDTH,
                                     GRAPHICS_RESOURCE_HEIGHT,
                                     GRAPHICS_RGBA32(cb,cg,cr,0xff), /* fg BGRA */
                                     GRAPHICS_RGBA32(0,0,0,0x00), /* bg */
                                     split, text_length, text_size);
      if (s!=0) return s;
   }
   return text_size;
}

int getText(char* txt, uint32_t buflen, char* filename, uint* red, uint* green, uint* blue)
{
   FILE * pFile;
   
   int size = 0;
   char * tok;
   int i = 0;
   char colCode[50];
   memset(colCode,0,50);

   memset(txt,0,buflen);
   
   pFile = fopen (filename , "r");
   if (pFile == NULL) perror ("Error opening file");
   else {
     if ( fgets (colCode , 50 , pFile) != NULL ) 
	 {	 
	   fgets (txt , buflen, pFile);
	   
	   //if( strstr( colCode, "red" ) != 0 ) retCol = 1;
	   //if( strstr( colCode, "green" ) != 0 ) retCol = 2;
//       puts (txt);
           tok = strtok(colCode, "#");
           while( tok != NULL ) {
             if( i==0 ) *red = atoi(tok);
             if( i==1 ) *green = atoi(tok);
             if( i==2 ) *blue = atoi(tok);
             if( i==3 ) size = atoi(tok);
			 //ts(tok);
             i++;
             tok = strtok( NULL, "#" );
           }
	 }
     fclose (pFile);
   }
   if( size == 0 ) size = 10;
   return size;
}

uint32_t renderStat(GRAPHICS_RESOURCE_HANDLE img, char* filename, uint32_t yoff_in)
{
   FILE * pFile;
   
   char * tok;
   int i = 0;
   char line[100];
   memset(line,0,100);
   int red = 0;
   int green = 0;
   int blue = 0;
   int size = 5;
   char txt1[100];
   char txt2[100];
   
   uint32_t yoff = yoff_in;

   pFile = fopen (filename , "r");
   if (pFile == NULL) perror ("Error opening file");
   else {
     while ( fgets (line , 100 , pFile) != NULL ) 
	 {	 
       i = 0;
       red = green = blue = 0;
       tok = strtok(line, "#");
       while( tok != NULL ) {
//         if( i==0 ) strcpy(txt1,tok);
         if( i==2 ) strcpy(txt1,tok);
         if( i==3 ) red = atoi(tok);
         if( i==4 ) green = atoi(tok);
         if( i==5 ) blue = atoi(tok);
         if( i==6 ) size = atoi(tok);
         i++;
         tok = strtok( NULL, "#" );
       }
	   puts( txt1 );
	         sprintf(txt2,"%u",yoff);
			 puts( txt2 );

//	   render_subtitle(img, 0, txt1, size, yoff, red, green, blue, 0 );
	   render_subtitle(img, 0, txt1, size, yoff, red, green, blue, 0 );
	   yoff = yoff + size * 1.2;
	   
	   }
     fclose (pFile);
   }
   return yoff;
}

int main(int argc, char *argv[])
{
   GRAPHICS_RESOURCE_HANDLE img;
   uint32_t width, height;
   int LAYER=1;
   bcm_host_init();
   int s;
   int carg = 0;
   char info[50];
   memset(info,0,50);

   uint32_t loops = 6000;

   while( carg < argc-1 )
   {
     if(strcmp(argv[carg],"-l")==0) LAYER=atoi(argv[carg+1]);
     if(strcmp(argv[carg],"-t")==0) loops=atoi(argv[carg+1]);
     carg ++;
   } 

   printf( "Layer: %u\n", LAYER);
   printf( "Seconds: %u\n", loops);

   s = gx_graphics_init(".");
   assert(s == 0);

   s = graphics_get_display_size(0, &width, &height);
   assert(s == 0);

   s = gx_create_window(0, width, height, GRAPHICS_RESOURCE_RGBA32, &img);
   assert(s == 0);

   // transparent before display to avoid screen flash
   graphics_resource_fill(img, 0, 0, width, height, GRAPHICS_RGBA32(0,0,0,0x00));

   graphics_display_resource(img, 0, LAYER, 0, 0, GRAPHICS_RESOURCE_WIDTH, GRAPHICS_RESOURCE_HEIGHT, VC_DISPMAN_ROT0, 1);

   uint32_t tSize[3];
   uint red[3];
   uint green[3];
   uint blue[3];
   uint32_t stat_off = 0;
   
   while ( loops > 0 ) {

	  char text1[50];
	  tSize[0] = getText(text1,50,"/var/tmp/rt1.txt", &red[0], &green[0], &blue[0] );
	  char text2[50];
	  tSize[1] = getText(text2,50,"/var/tmp/rt2.txt", &red[1], &green[1], &blue[1] );
	  char text3[50];
	  tSize[2] = getText(text3,50,"/var/tmp/rb2.txt", &red[2], &green[2], &blue[2] );

      graphics_resource_fill(img, 0, 0, width, height, GRAPHICS_RGBA32(0,0,0,0x00));

      sprintf(info,"%u / %u",LAYER,loops);

      // draw the subtitle text
      render_subtitle(img, 1, text1, tSize[0], 5, red[0], green[0], blue[0], 0 );
      render_subtitle(img, 1, text2, tSize[1], 5 + tSize[0] * 1.2, red[1], green[1], blue[1] ,0  );
      render_subtitle(img, 1, text3, tSize[2], 100 - tSize[2] * 1.5, red[2], green[2], blue[2] ,0 );
//      render_subtitle(img, 1, info,  7, height/2-15+(LAYER-1)*40, 128, 128, 128, 0 );

	  stat_off = renderStat( img, "/var/tmp/stat.txt", 12);
//	  stat_off = renderStat( img, "/var/tmp/stat.txt", stat_off);
	  
      graphics_update_displayed_resource(img, 0, 0, 0, 0);

      sleep(1);
      loops--;
   }

   graphics_display_resource(img, 0, LAYER, 0, 0, GRAPHICS_RESOURCE_WIDTH, GRAPHICS_RESOURCE_HEIGHT, VC_DISPMAN_ROT0, 0);
   graphics_delete_resource(img);

   return 0;
}

