/** @name radiodemo.c
  * Tests sending and receiving via wireless transmission
  * VERSION FOR EYEBOT UNDER BLUETOOTH
  * @author Thomas Braunl, 2005
  */

#include "include/eyebot.h"
#include "string.h"

void sendstr(char s[])
{ int i=0;
  while (s[i] != 0)
  { OSSendCharRS232(s[i], SERIAL1);
    i++;
  }
}


void radiodemo()
{ BYTE myId, fromId;
  char mes[1000];
  int i, err, key, cable, len;
  char* hello = "Hello EyeBot!\n";
  int hlen = strlen(hello);

  LCDClear();
  LCDPutString("Wireless Network\nCABLE test(Ser1) or\nWIRELESS");
  LCDMenu("Cab","Wle"," "," ");
  do {key = KEYGet();} while (key!=KEY1 && key!=KEY2);
  cable = (key==KEY1);
  
  LCDClear();
  LCDPutString("Wireless Eye2PC\n");
  LCDPutString("---------------\n");
  LCDMenu("Snd","SnA","Rec","END");
  myId = OSMachineID();
  LCDPrintf("I am robot %d\n", myId);
 
  if (cable) OSInitRS232(SER115200, NONE, SERIAL1);
  else { LCDPutString("Radio-Init\n");
         err = RADIOInit();
         if (err) { LCDPutString("Error Radio Init\n"); 
                    KEYGet(); return;
                  }
       }

  do
  { key = KEYGet();
    if (key == KEY1) 
    { LCDPutString(hello);
      if (cable) sendstr(hello);
      else RADIOSend(0, hlen, (BYTE*) hello); /* send to Host PC */
    }
    
    if (key == KEY2) 
    for (i=0; i<1000; i++)
    { LCDPutString("A");
      if (cable) OSSendCharRS232('A', SERIAL1);
        else RADIOSend(0, 1, "A"); /* send to Host PC */
    }
    
    if (key == KEY3) //RECEIVE DATA
    { LCDPutString("\nReceive Mode\nPress any key\nto exit\n");
      do
      { len=0;
        if (cable)
        { while (OSCheckInRS232(SERIAL1) && !KEYRead())
          { OSRecvRS232(mes, SERIAL1);  // Receive single char
            LCDPutChar(mes[0]);;
          }
        }
        else while (RADIOCheck() && !KEYRead())  //check whether message is waiting 
          { RADIORecv(&fromId, &len, mes);  // wait for next message
            LCDPrintf("Recv %d char\from ID %d\n", len, fromId);
            if (len>100) len=100;
            for (i=0; i<len; i++) LCDPutChar(mes[i]);
            LCDPutChar('\n');;
          }
      } while (!KEYRead());
      LCDPutString("\n--terminated--\n");
    }
       
  } while (key != KEY4);
  LCDPutString("Term..");
  RADIOTerm();
}

