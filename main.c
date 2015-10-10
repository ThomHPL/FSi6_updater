#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "serial.h"

BOOL writePage(unsigned char *page,int nbytes,int offset,HANDLE *hdlr);
BOOL readPage(unsigned char page[],char filename[],int offset, int nbytes);
BOOL printArray(unsigned char *array,int nbytes);
BOOL sumcheck(unsigned char *frame,int nbytes);

int main(int argc, char *argv[])
{



    // ASCII header
    printf("    [===============================================]\r\n");
    printf("    |    FLYSKY FS-i6 CUSTOM FIRMWARE UPLOADER      |\r\n");
    printf("    |    by Thom				    |\r\n");
    printf("    | 					            |\r\n");
    printf("    |    check out my blog for more:		    |\r\n");
    printf("    |    basejunction.wordpress.com	            |\r\n");
    printf("    [===============================================]\r\n\r\n");
    printf("\n!!! Please make sure the firmware file comes from basejunction.wordpress.com !!!\n\n");

    HANDLE hdlr = NULL; //port handle
    int n_com=1;
    int n_baudrate=115200;

    char arg1[256],arg2[256],arg3[256],arg4[256],arg5[256];
    char cmd[256]="";

    if(argc>=2)
        n_com=(int)strtol(argv[1],NULL,10);
    if(argc>=3)
        n_baudrate=(int)strtol(argv[2],NULL,10);

    //init
    printf("Opening COM%d at %d bauds : ",n_com,n_baudrate);
    if(openCom(n_com,n_baudrate,&hdlr))
        printf("SUCCESS");
    else
        printf("FAILED - verify your COM port number and baudrate settings");
    printf("\n\n");



    //main loop
    do
    {

        printf(">> ");
        fgets(cmd, sizeof(cmd), stdin);

        sscanf(cmd,"%s %s %s %s %s",arg1,arg2,arg3,arg4,arg5);



        if(!strcmp(arg1,"help")|| !strcmp(arg1,"h"))
        {
            printf("-- open [port] [baudrate] : open com port number [port] at [baudrate] bauds\n");
            printf("-- readb : read byte \n");
            printf("-- readf : read frame \n");
            printf("-- sendb [format] [byte] : send [byte] with [format{-b,-d,-h}]\n");
            printf("-- write [filename] [offset:h] [nbytes:d] : write [nbytes](decimal) bytes page at [offset](hex)\n");
            printf("-- flash [filename] : program [filename] firmware into TX flash memory (bootloader untouched)\n");
            printf("-- ping : send a ping request\n");
            printf("-- reset : send a reset TX request\n");
            printf("-- quit : quit the updater\n");

        }

        else if(!strcmp(arg1,"readb"))
        {
            unsigned char byte=0;
            if(readByte(&byte,&hdlr))
                printf("%x \n",byte);
            else
                printf("ERROR - reading byte from serial buffer\n");
        }
        else if(!strcmp(arg1,"readf"))
        {
            unsigned short size=0;
            unsigned short checksum=0;
            unsigned char frame[256];
            if(!readFramedbg(&size,&checksum,frame,&hdlr))
                printf("ERROR - reading frame from serial buffer\n");
            if(!sumcheck(frame,size))
                printf("ERROR - checksum mismatch\n");

        }
        else if(!strcmp(arg1,"sendb"))
        {
            unsigned char msg=0;
            if(!strcmp(arg2,"-h"))
                msg=(unsigned char)strtoul(arg3,NULL,16);
            if(!strcmp(arg2,"-d"))
                msg=(unsigned char)strtoul(arg3,NULL,10);
            if(!strcmp(arg2,"-b"))
                msg=(unsigned char)strtoul(arg3,NULL,2);
            printf("Sending byte: %d 0x%x\n",msg,msg);
            if(!sendByte(&msg,&hdlr))
                printf("ERROR - sending byte in serial buffer\n");
        }
        else if(!strcmp(arg1,"reset"))
        {
            printf("reset . . . \n");
            unsigned char msg[]={0x06,0x00,0xC1,0x00,0xFF,0xFF};//38FF
            sendFrame(msg,sizeof(msg),&hdlr);
        }
        else if(!strcmp(arg1,"ping"))
        {
            unsigned char msg[]={0x05,0x00,0xC0,0x3A,0xFF};
            unsigned short size=0;
            unsigned short checksum=0;
            unsigned char frame[256];

            printf("ping . . . \n");
            sendFrame(msg,sizeof(msg),&hdlr);

            if(!readFrame(&size,&checksum,frame,&hdlr))
                printf("ERROR - reading frame from serial buffer\n");
            if(!sumcheck(frame,size))
                printf("ERROR - checksum mismatch\n");
            printf("\nConnected : FLYSKY i6 \nFirmware version : %d.%d \n",frame[6],frame[4]);
        }
        else if(!strcmp(arg1,"write"))
        {
            unsigned int offset=strtoul(arg3,NULL,16);
            unsigned char msg[256];
            if(!readPage(msg,arg2,offset,256))
                printf("ERROR - reading file\n");
            writePage(msg,sizeof(msg),offset,&hdlr);
        }
        else if(!strcmp(arg1,"flash"))
        {
            BOOL cont = TRUE;
            unsigned char msg[256];
            int i=0x1800;
            for(i=0x1800;(i<0xF300)&&cont;i=i+0x100)
            {
                //11 00 C2 00 18 00 09 00 00 00 00 00 00 00 00 0B FF
                unsigned char msg2[17]={0x11,0x00,0xC2,0xFF,0xFF,0x00,0x09,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF};
                unsigned short reply_size=0;
                unsigned short reply_checksum=0;
                unsigned char reply[19];
                //55 13 00 C2 80 00 18 00 09 00 00 00 00 00 00 00 00 34 FE

                msg2[3]=(unsigned char)i;
                msg2[4]=(unsigned char)(i>>8);
                sendFrame(msg2,sizeof(msg2),&hdlr);

                unsigned char ok_reply[19]={0x55,0x13,0x00,0xC2,0x80,0xFF,0xFF,0x00,0x09,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF};
                ok_reply[5]=(unsigned char)i;
                ok_reply[6]=(unsigned char)(i>>8);


                unsigned int checksum=0xFFFF;
                int j=0;
                for(j=0;j<19-2;j++)
                {
                    checksum=checksum-ok_reply[j];
                }

                ok_reply[19-2]=(unsigned char)checksum;
                ok_reply[19-1]=(unsigned char)(checksum>>8);


                readFrame(&reply_size,&reply_checksum,reply,&hdlr);

                unsigned char reply_s[19];
                memcpy(reply_s,reply,reply_size);

                // compare received reply with correct one
                if(memcmp(reply_s,ok_reply,sizeof(reply_s))!=0)
                {
                    printf("ERROR - bad reply from TX\n");
                    cont=FALSE;
                }

                if(!readPage(msg,arg2,i,256))
                {
                    printf("ERROR - reading file\n");
                    cont=FALSE;
                }
                writePage(msg,sizeof(msg),i,&hdlr);
                i=i+0x100;
                if(!readPage(msg,arg2,i,256))
                {
                    printf("ERROR - reading file\n");
                    cont=FALSE;
                }
                writePage(msg,sizeof(msg),i,&hdlr);
                i=i+0x100;
                if(!readPage(msg,arg2,i,256))
                {
                    printf("ERROR - reading file\n");
                    cont=FALSE;
                }
                writePage(msg,sizeof(msg),i,&hdlr);
                i=i+0x100;
                if(!readPage(msg,arg2,i,256))
                {
                    printf("ERROR - reading file\n");
                    cont=FALSE;
                }
                writePage(msg,sizeof(msg),i,&hdlr);
            }
            //reset
            if(cont)
            {
                printf("Reset . . .\n");
                unsigned char msg3[]={0x06,0x00,0xC1,0x00,0xFF,0xFF};//38FF
                sendFrame(msg3,sizeof(msg3),&hdlr);
            }

        }

        else if(!strcmp(arg1,"open"))
        {
            closeCom(&hdlr);
            unsigned int n_com=strtoul(arg2,NULL,10);
            unsigned int n_baudrate=strtoul(arg3,NULL,10);
            printf("Opening COM%d at %d bauds : ",n_com,n_baudrate);
            if(openCom(n_com,n_baudrate,&hdlr))
                printf("SUCCESS");
            else
                printf("FAILED - verify your COM port number and baudrate settings");
            printf("\n\n");
        }

        else if(strcmp(arg1,"quit"))
        {
            printf("-- Unknown command \"%s\"\n",arg1);
            printf("-- Type \"help\" for a command list\n");
        }
    }while(strcmp(arg1,"quit"));

    closeCom(&hdlr);
    return 0;
}

BOOL writePage(unsigned char page[],int nbytes,int offset,HANDLE *hdlr)
{
    if(offset<0x1800||offset>0xF200)
    {
        //a autoriser: F400 F500 F800 FA00
        printf("Ignoring page at offset 0x%x\n",offset);
        return FALSE;
    }
    if(offset==0xF200)
    {
        nbytes=0xD5;
    }

    unsigned char header[9];
    unsigned int total_size=nbytes+sizeof(header)+2;
    unsigned char frame[total_size];
    unsigned int checksum=0xFFFF;

    unsigned short reply_size=0;
    unsigned short reply_checksum=0;
    unsigned char reply[256];

    unsigned char ok_reply[10]={0x55,0x0A,0x00,0xC3,0x00,0x00,0x00,0x00,0xDD,0xFE};
    header[0]=(unsigned char)(total_size);
    header[1]=(unsigned char)((total_size)>>8);
    header[2]=0xC3;
    header[3]=(unsigned char)(offset);
    header[4]=(unsigned char)((offset)>>8);
    header[5]=0x00;
    header[6]=0x00;
    header[7]=(unsigned char)(nbytes);
    header[8]=(unsigned char)((nbytes)>>8);

    memcpy(frame,header,sizeof(header));
    memcpy(frame+sizeof(header),page,nbytes);

    int i=0;
    for(i=0;i<sizeof(frame)-2;i++)
    {
        checksum=checksum-frame[i];
    }

    frame[total_size-2]=(unsigned char)checksum;
    frame[total_size-1]=(unsigned char)(checksum>>8);
    //printf("READING PAGE AT 0x%x\n",offset);
    /*printf("[Header]\n");
    for(i=0;i<sizeof(frame);i++)
    {
        //printf("%x ",frame[i]);
        if((i+1)==sizeof(header))
            printf("\n[Data]\n");
        if((i+1)==sizeof(header)+nbytes)
            printf("\n[Checksum]\n");
    }*/
    //printf("\n");
    printf("SENDING PAGE AT 0x%x . . .\n",offset);
    sendFrame(frame,sizeof(frame),hdlr);
    printf("WAITING REPLY . . .\n");

    //testing the replied frame to check proper transfer
    // should be 55 0A 00 C3 00 00 00 00 DD FE
    readFrame(&reply_size,&reply_checksum,reply,hdlr);

    unsigned char reply_s[10];
    memcpy(reply_s,reply,reply_size);

    if(!sumcheck(reply_s,sizeof(reply_s)))
    {
        printf("ERROR - checksum mismatch\n");
        return FALSE;
    }

    if(memcmp(reply_s,ok_reply,sizeof(reply_s))!=0)
    {
        printf("ERROR - bad page reply from TX\n");
        return FALSE;
    }
    return TRUE;
}

BOOL readPage(unsigned char page[],char filename[],int offset, int nbytes)
{
    unsigned char frame[nbytes];
    FILE* file = NULL;

    file = fopen(filename, "rb");
    if(file==NULL)
        return FALSE;
    //...
    fseek(file,offset,0);

    int i=0;
    for(i=0;i<nbytes;i++)
    {
        //printf("index : %x\n",(int)ftell(file));
        frame[i]=(unsigned char)fgetc(file);
    }
    memcpy(page,frame,nbytes);
    fclose(file);
    return TRUE;
}


BOOL sumcheck(unsigned char *frame,int nbytes)
{
    int i=0;
    int checksum1= 0xFFFF;
    int checksum2=(frame[nbytes-1]<<8)+frame[nbytes-2];
    for(i=0;i<nbytes-2;i++)
    {
        checksum1=checksum1-frame[i];
    }
    //printf("\n%x ?= %x\n",checksum1,checksum2);

    return  (checksum1==checksum2);
}
