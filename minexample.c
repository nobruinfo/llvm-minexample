// llvm-minexample

#include <stdlib.h>
#include <string.h>
#include <stdio.h>  // llvm
#include <mega65/conio.h>  // llvm instead of <printf.h>
#include <mega65/memory.h>  // mega65-libc

#include <mega65.h>  // taken from KickC

// #define DEBUG

#include "regions.h"
#include "fileio.h"

static char* p2sbuf = (char*)0x334;

#define COLOR_RAM_BASE 0xFF80000UL
// to by replaced by a getter function
void cputln(void)  {
  unsigned char width;
  unsigned char height;
  unsigned int screenbytes;
  
  getscreensize(&width, &height);
  screenbytes = width * height;
  if (wherey() + 1 >= height)  {
	lcopy(getscreenaddr() + width, getscreenaddr(), screenbytes - width);
    lcopy(COLOR_RAM_BASE + width, COLOR_RAM_BASE, screenbytes - width);
    lfill(getscreenaddr() + screenbytes - width, ' ', width);
    lfill(COLOR_RAM_BASE + screenbytes - width, COLOUR_WHITE, width);
		// COLOUR_WHITE to be replaced by a getter function
		// COLOR_RAM_BASE calculation supports one-byte colours only
	gotoxy(0, wherey());
  } else {
	gotoxy(0, wherey() + 1);
  }
}

char asciitoscreencode(char c)
{
//    if (c >= 64 && c <= 95) {
//        return c - 64;
//    }

    if (c >= 192) {
        return c - 128;
    }

    if (c >= 96 && c < 192) {
        return c - 96;
    }

    if (c == '[') return 27;
    if (c == '\\') return 105;
    if (c == ']') return 29;
    if (c == '^') return 100;
    if (c == '_') return 100;

    return c;
}

char* asciitoscreencode_s(char* s)
{
    char* src = s;
    char* dest = p2sbuf;
    while ((*dest++ = asciitoscreencode(*src++)))
        ;
    return p2sbuf;
}

int main() {
  // Backup ZP:
  lcopy(0, ATTICZPBACKUP, BLOCKSIZE);

  conioinit();
  // ASCII lowercase is in conioinit()
  setuppercase();  // to activate PETSCII
  POKE(0xD018U, PEEK(0xD018U) ^ 0x02); // toggle case as if [Mega][Alt] was pressed
  flushkeybuf();
//  mega65_io_enable();

  // Disable 48MHz fast mode
  VICIV->CONTROLB &= ~(VICIV_FAST);
  VICIV->CONTROLC &= ~(VICIV_VFAST);

  clrscr();
  gohome();

  cputs(asciitoscreencode_s("ATTICZPBACKUP 32addr is: "));
  cputhex(ATTICZPBACKUP >> 16, 4);
  cputs(" ");
  cputhex(ATTICZPBACKUP & 0xffff, 4);
  cputln();
  printf("ATTICZPBACKUP 32addr is: %08lx", (unsigned long) ATTICZPBACKUP);
  cputln();
  cgetc();

  cputs(asciitoscreencode_s("here goes the code"));
  cputln();
  cgetc();
  flushkeybuf();

  // Restore ZP:
  lcopy(ATTICZPBACKUP, 0, BLOCKSIZE);

  return 0;
}
