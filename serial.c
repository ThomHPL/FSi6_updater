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



#include "serial.h"
#undef _DEBUG


static COMMTIMEOUTS g_cto =
{
    MAX_WAIT_READ,  /* ReadIntervalTimeOut          */
    0,              /* ReadTotalTimeOutMultiplier   */
    MAX_WAIT_READ,  /* ReadTotalTimeOutConstant     */
    0,              /* WriteTotalTimeOutMultiplier  */
    0               /* WriteTotalTimeOutConstant    */
};

static DCB g_dcb =
{
    sizeof(DCB),        /* DCBlength            */
    115200,             /* BaudRate             */
    TRUE,               /* fBinary              */
    FALSE,              /* fParity              */
    FALSE,              /* fOutxCtsFlow         */
    FALSE,              /* fOutxDsrFlow         */
    DTR_CONTROL_DISABLE, /* fDtrControl          */
    FALSE,              /* fDsrSensitivity      */
    FALSE,              /* fTXContinueOnXoff    */
    FALSE,              /* fOutX                */
    FALSE,              /* fInX                 */
    FALSE,              /* fErrorChar           */
    FALSE,              /* fNull                */
    RTS_CONTROL_DISABLE, /* fRtsControl          */
    FALSE,              /* fAbortOnError        */
    0,                  /* fDummy2              */
    0,                  /* wReserved            */
    0x100,              /* XonLim               */
    0x100,              /* XoffLim              */
    8,                  /* ByteSize             */
    NOPARITY,           /* Parity               */
    ONESTOPBIT,         /* StopBits             */
    0x11,               /* XonChar              */
    0x13,               /* XoffChar             */
    '?',                /* ErrorChar            */
    0x1A,               /* EofChar              */
    0x10                /* EvtChar              */
};


/* Functions */
BOOL sendFrame(unsigned char frame[],int nbytes,HANDLE *hdlr)
{
    unsigned int checksum=0xFFFF;
    int i=0;
    for(i=0;i<nbytes-2;i++)
    {
        checksum=checksum-frame[i];
    }

    frame[nbytes-2]=(unsigned char)checksum;
    frame[nbytes-1]=(unsigned char)(checksum>>8);
    i =1;
    for(i=1;i<=nbytes;i++)
    {
        sendByte(&frame[i-1],hdlr);
    }
    return TRUE;
}
BOOL sendByte(unsigned char *byte, HANDLE *hdlr)
{

    PDWORD nBytesWritten=0;
    if(!WriteFile(*hdlr, byte, 1, nBytesWritten, NULL))
    {
        return FALSE;
    }
    /*
    #ifdef _DEBUG
        printf("[DEBUG] Bytes written : %d\r\n",nBytesWritten);
    #endif // _DEBUG
    */
    return TRUE;
}

BOOL readFrame(unsigned short *frame_size,unsigned short *checksum,unsigned char frame[],HANDLE *hdlr)
{

    unsigned char byte=0;

    if(!readByte(&byte,hdlr))
        return FALSE;

    if(byte==0x55)
    {
        if(!readShort(frame_size,hdlr))
            return FALSE;
    }
    else
    {
        while(byte!=0x55)
        {
            if(!readByte(&byte,hdlr))
                return FALSE;
        }
        if(!readShort(frame_size,hdlr))
            return FALSE;
    }

    unsigned char t_frame[*frame_size];
    t_frame[0] = byte;
    t_frame[1] = *frame_size&0x00FF;
    t_frame[2] = (*frame_size&0xFF00)>>8;

    //printf("__Frame size: %d bytes \r\n",*frame_size);
    int i = 3;
    do
    {
        if(!readByte(&byte,hdlr))
            return FALSE;
        //printf("%d:%x  ",i+1,byte);
        t_frame[i]=byte;
        i=i+1;
    }while(i<*frame_size);

    memcpy(frame,t_frame,*frame_size);

    #ifdef _DEBUG
    printf("[DEBUG] ");
    printf("Frame  received: \n");
    printf("[DEBUG] ");
    printf("_Frame size: %d \n",*frame_size);
    printf("[DEBUG] ");
    printf("_Frame type: 0x%x \n",frame[3]);
    #endif

    return TRUE;
}
BOOL readFramedbg(unsigned short *frame_size,unsigned short *checksum,unsigned char frame[],HANDLE *hdlr)
{

    unsigned char byte=0;

    if(!readByte(&byte,hdlr))
        return FALSE;

    if(byte==0x55)
    {
        if(!readShort(frame_size,hdlr))
            return FALSE;
    }
    else
    {
        while(byte!=0x55)
        {
            if(!readByte(&byte,hdlr))
                return FALSE;
        }
        if(!readShort(frame_size,hdlr))
            return FALSE;
    }

    unsigned char t_frame[*frame_size];
    t_frame[0] = byte;
    t_frame[1] = *frame_size&0x00FF;
    t_frame[2] = (*frame_size&0xFF00)>>8;

    //printf("__Frame size: %d bytes \r\n",*frame_size);
    int i = 3;
    do
    {
        if(!readByte(&byte,hdlr))
            return FALSE;
        //printf("%d:%x  ",i+1,byte);
        t_frame[i]=byte;
        i=i+1;
    }while(i<*frame_size);

    memcpy(frame,t_frame,*frame_size);
    printArray(frame,*frame_size);
    #ifdef _DEBUG
    printf("[DEBUG] ");
    printf("Frame  received: \n");
    printf("[DEBUG] ");
    printf("_Frame size: %d \n",*frame_size);
    printf("[DEBUG] ");
    printf("_Frame type: 0x%x \n",frame[3]);
    #endif

    return TRUE;
}


BOOL readChar(char *byte, HANDLE *hdlr)
{
    PDWORD nBytesRead=0;
    return ReadFile(*hdlr, byte, 1, nBytesRead, NULL);
}

BOOL readByte(unsigned char *byte, HANDLE *hdlr)
{
    PDWORD nBytesRead=0;
    return ReadFile(*hdlr, byte, 1, nBytesRead, NULL);
}

BOOL readShort(unsigned short *data, HANDLE *hdlr)
{
    unsigned char byte_msb=0;
    unsigned char byte_lsb=0;
    if(!readByte(&byte_lsb,hdlr))
        return FALSE;
    if(!readByte(&byte_msb,hdlr))
        return FALSE;
    *data=(byte_msb<<8)|byte_lsb;
    return TRUE;
}

BOOL openCom(int com_n,DWORD baudrate,HANDLE *hdlr)
{

    #ifdef _DEBUG
    printf("[DEBUG] Connecting to COM%d at %d bauds : " ,com_n,(int)baudrate);
    #endif

    char com_name[16];

    sprintf(com_name, "COM%d", com_n);
    *hdlr = CreateFile(com_name, GENERIC_READ|GENERIC_WRITE, 0, NULL,
                        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(*hdlr == INVALID_HANDLE_VALUE)
    {
        #ifdef _DEBUG
        printf("ERROR - invalid handle (COM%d)\r\n", com_n);
        #endif
        return FALSE;
    }

    // size of RX and TX buffers
    SetupComm(*hdlr, RX_SIZE, TX_SIZE);

    // com port config
    g_dcb.BaudRate=baudrate;
    if(!SetCommTimeouts(*hdlr, &g_cto) || !SetCommState(*hdlr, &g_dcb))
    {
        #ifdef _DEBUG
        printf("ERROR - setting timeouts (COM%d)\r\n", com_n);
        #endif
        CloseHandle(*hdlr);
        return FALSE;
    }

    // empty the buffers
    PurgeComm(*hdlr, PURGE_TXCLEAR|PURGE_RXCLEAR|PURGE_TXABORT|PURGE_RXABORT);

    #ifdef _DEBUG
    printf("OK\r\n");
    #endif

    return TRUE;
}

BOOL closeCom(HANDLE *hdlr)
{
    CloseHandle(*hdlr);
    return TRUE;
}

BOOL printArray(unsigned char *array,int nbytes)
{
    int i=0;
    for(i=0;i<nbytes;i++)
    {
        printf("%x : ",i);
        printf("%x \n",array[i]);
    }
    return TRUE;
}

