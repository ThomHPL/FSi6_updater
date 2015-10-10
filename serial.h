
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#ifndef _SERIAL_H
#define _SERIAL_H

#define RX_SIZE         4096    /* taille tampon d'entrée  */
#define TX_SIZE         4096    /* taille tampon de sortie */
#define MAX_WAIT_READ   100    /* temps max d'attente pour lecture (en ms) */

BOOL openCom(int com_n,DWORD baudrate,HANDLE *hdlr);
BOOL closeCom();

BOOL sendByte(unsigned char *byte, HANDLE *hdlr);
BOOL sendFrame(unsigned char frame[],int nbytes,HANDLE *hdlr);

BOOL readChar(char *byte, HANDLE *hdlr);
BOOL readByte(unsigned char *byte, HANDLE *hdlr);
BOOL readFrame(unsigned short *frame_size,unsigned short *checksum,unsigned char frame[],HANDLE *hdlr);
BOOL readFramedbg(unsigned short *frame_size,unsigned short *checksum,unsigned char frame[],HANDLE *hdlr);
BOOL readShort(unsigned short *data, HANDLE *hdlr);
BOOL printArray(unsigned char *array,int nbytes);

#endif // _SERIAL_H


