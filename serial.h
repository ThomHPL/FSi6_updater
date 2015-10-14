// Copyright Thomas Herpoel 2015
//
// This file is part of FSi6_updater
//
// FSi6_updater is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// FSi6_updater is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.



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


