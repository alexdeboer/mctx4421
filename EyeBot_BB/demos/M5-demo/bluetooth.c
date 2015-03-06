/*##############################################*/
/* Bluetooth Setup for HandyPort				*/
/* Christian Schmitz 		September 2004		*/
/* Fachhochschule Koblenz, Germany				*/
/* Adapted by T. Braunl, May 2005               */
/*##############################################*/
#include "include/eyebot.h"
#define ENTER ((char) 0x0d)  // enter character

int bluetoothsetup (void)
{	int pos = 0, error=0, speed=0, hske=0, modus=0, comport=1, number=0, col=0;
    int key, i, serial;  // port number of module
	char adr[]={'0','0','0','0','0','0','0','0','0','0','0','0'};
	int serspeeds[] = {9600,19200,38400,57600,115200,4800};
    char *string_hsk[2] ={"   NONE", "RTS/CTS"};
    char *string_mod[3] ={"    1:1", "   WAIT", "REG_CON"};
	LCDClear();		
	LCDPutString("Bluetooth SETUP\n----------------");
	LCDPutString("Press RST on BT.Wait until LNK \nflashes...\n");
    LCDPutString("...Then select\n   serial port");
	LCDMenu("Sr1","Sr2","Sr3","END");
	
    switch(KEYGet())
  	{ case KEY1: serial=SERIAL1; break;
  	  case KEY2: serial=SERIAL2; break;
  	  case KEY3: serial=SERIAL3; break;
  	  case KEY4: return 0;
    }
	error = OSInitRS232(SER9600, NONE, serial);	//Open RS232 on SERIAL1 with HandyPort default values
	if(error)
	{ LCDPrintf("RS232 error\n");
      KEYGet(); return 1;
	}
  
	error = OSSendCharRS232(ENTER, serial);	
	if (error)
    { LCDPrintf("RS232 send error");
      KEYGet(); return 1;
    }
	LCDClear();
	LCDPutString("Bluetooth SETUP\n");
	//LCDPutString("----------------");
	LCDPutString("Speed:\nHandske:\nMode:\nRemAdr:\n");
	LCDMenu("+","-","Nxt","SET");
	LCDSetChar(1, 15, '*');	
	LCDSetPos(5,0);
	LCDPutString("000000000000\nPort Num");
     
    while (1)
    {   LCDSetPrintf(1,9, "%6d", serspeeds[speed]); 
        LCDSetString(2,8, string_hsk[hske]);
   		LCDSetString(3,8, string_mod[modus]);
		LCDSetPrintf(6,12,"%d  ", comport);
  
    	key = KEYGet();
    	switch (key)
      	{ case KEY1: //+ Button
    	  if (pos==0) speed = (speed+1) %5;          
      	  if (pos==1) hske  = (hske+1)  %2;
          if (pos==2) modus = (modus+1) %3;  
      	  if (pos==3)
          	{ number = (number+1) %16;
              if (number<10) adr[col]='0'+number;
                else         adr[col]='A'+number-10;
              LCDSetChar(5,col, adr[col]);
      		}
		  if (pos==4) comport = comport%7 +1; // 1..7
		  break;

          case KEY2:       	//- Button
          if (pos==0) speed = (speed-1+5) %5;
          if (pos==1) hske  = (hske -1+2) %2;
          if (pos==2) modus = (modus-1+3) %3;
          if (pos==3) col   = (col+1)     %12;
		  if (pos==4) comport = (comport-1-1+7)%7 +1; //1..7
		  break;
		  
          case KEY3:		//NXT-Button
          if (pos==0)
            { LCDSetChar(1,15,' ');
              LCDSetChar(2,15,'*');
            }
          else if (pos==1)
            { LCDSetChar(2,15,' ');
              LCDSetChar(3,15,'*');
            }
          else if (pos==2)
            { LCDSetChar(3,15,' ');
              LCDSetChar(4,15,'*');
			  LCDMenu("","-->","","");	//change menu for address
            }
          else if (pos==3)
          	{ LCDSetChar(4,15,' ');
			  LCDSetChar(6,15,'*');
              col = 0;
              LCDMenu("+","-","Nxt","SET");
            }
		  else // if (pos==4)
          	{ LCDSetChar(1,15,'*');
			  LCDSetChar(6,15,' ');
            }
          pos = (pos+1) %5;            
          break;

        case KEY4:  //send settings to HandyPort
          OSSendCharRS232('B', serial);  //send speed
          OSSendCharRS232('3'+speed, serial);	
          OSSendCharRS232('F', serial);  //'F' Flow control
          OSSendCharRS232('0'+hske, serial);	//send handshake
          OSSendCharRS232('M', serial);
          OSSendCharRS232('0'+modus, serial);	//send mode
          OSSendCharRS232('C', serial);
		  OSSendCharRS232('1'+comport,serial); //send comport
          OSSendCharRS232('A', serial);
          for(i=0;i<12;i++) OSSendCharRS232(adr[i], serial);	//send remote address
          OSSendCharRS232(ENTER, serial);	//send confirmation      
          OSSendCharRS232('X', serial);	//save and reboot of HandyPort

          LCDClear();
          LCDPutString("WAIT\nReboot of\nwireless module\nin progress ...");
		  OSWait(300);

		  OSInitRS232(SER115200, NONE, serial);//set RS232 back to default values
          return 0;
      }
    }
}

