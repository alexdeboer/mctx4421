/** @name radiodemo.c
  * Tests sending and receiving via wireless transmission
  * VERSION FOR EYEBOT
  * @author Thomas Braunl, 1999, 2005
  */


#include "include/eyebot.h"

#define SEND(port) __asm("BSET.B #1,%0" : "=g" (port) : )
#define RECV(port) __asm("BCLR.B #1,%0" : "=g" (port) : )
BYTE *SER2PORT = (BYTE*)Ser1Base + 4;

#define MAXLEN  5
#define CODENO  6
char mesmnemo[CODENO][4] = {"Tok","OS ","Usr","Syn","Wld","Add"};
char legend[] =  "cod to fr nx len";

#define SYNCCHAR  ((char) 0x55)   /* sync      */
#define UARTCHAR  ((char) 0xFF)   /* lock UART */
#define STARTCHAR ((char) 0x01)   /* mes start */
#define MSTART    ((char) 0xc6)   /* mes. start char   */
#define MES       ((char) 0x00)
#define TOID      ((char) 1)
#define FROMID    ((char) 2)
#define NEXTID    ((char) 0)
#define LEN       ((char) 0)
#define CHECK     ((char) 3)

#define SENDLEN 14
char sendstr[SENDLEN] = {SYNCCHAR,SYNCCHAR,SYNCCHAR,UARTCHAR,UARTCHAR,
                         STARTCHAR,STARTCHAR,
                         MSTART,MES,TOID,FROMID,NEXTID,LEN,CHECK};


void radiodemo()
{
  int  i,j, meslen, init;
  char buf, key, c;

  LCDClear();
  LCDPutString("Wireless Network");
  LCDMenu("Tst","Snd","Rec","END");

  while ((key=KEYGet()) != KEY4)
    { switch(key)
      { case KEY1:
          OSInitRS232(SER9600, NONE, SERIAL2);
          c = 'A';
          for (i=0; i<3; i++)
            {
              LCDPutString("Send - ");
              AUTone(200, 4000); // sound longer than necessary

              SEND(*SER2PORT);
              for (j=0; j<1000; j++)
                OSSendCharRS232(c,SERIAL2);
              RECV(*SER2PORT);

              LCDPutString("Receive\n");
              AUTone(200, 1); // stop sound
              OSWait(100);
            }
          break;

        case KEY2:
          OSInitRS232(SER9600, NONE, SERIAL2);
          for (j=0; j<4; j++)
            {
              SEND(*SER2PORT);  // switch transmitter on
              AUBeep();
              LCDPrintf("send from%d to%d\n", sendstr[SENDLEN-4], sendstr[SENDLEN-5]);
              OSWait(10);
              for (i=0; i<SENDLEN; i++)
                OSSendCharRS232(sendstr[i], SERIAL2);
              OSWait(10);  // wait for transmission to finish
              RECV(*SER2PORT);
              if (sendstr[SENDLEN-5] < 15) sendstr[SENDLEN-5]++;
                else sendstr[SENDLEN-5] = 0;
            }
          break;

        case KEY3:
          LCDPutString(legend);
          RECV(*SER2PORT); // switch off transmitter

          OSInitRS232(SER9600, 0, SERIAL2);  // init port
          init = 0;
          meslen = 0;
          LCDMenu(" "," "," ","END");

          while ((key=KEYRead()) != KEY4)
            { if (OSCheckInRS232 (SERIAL2))
              { OSRecvRS232(&buf, SERIAL2);
                meslen++;
                if (buf == (char) 0xC6)
                { init   = 1;  // seen start byte
                  meslen = 0;  // start of new message
                }
                if (init && meslen >= 2 && meslen <= MAXLEN)
                { LCDPutHex1(buf);
                  if (meslen<MAXLEN) LCDPutChar(' ');
                    else             LCDPutChar(NL);
                }
                // print code for 2nd byte
                if (init && meslen == 1)
                { if (0 <= (int)buf && (int) buf < CODENO)
                    LCDPutString(mesmnemo[(int)buf]);
                  else LCDPutString("XXX");
                  LCDPutChar(' ');
                }
              }
            }
          break;

        case KEY4: break;
      }
      LCDMenu("Tst","Snd","Rec","END");
  }
}

