/*##############################################*/
/* WLAN Setup for WiPort/LANTRONIX				*/
/* Christian Schmitz 		September 2004		*/
/* University of Western Australia				*/
/* Fachhochschule Koblenz, Germany				*/
/*##############################################*/
#include "include/eyebot.h"
#include "string.h"
#include <stdlib.h>
void wlanInfo();
void channelInfo();
void serverInfo();
int serverSetUp();
int channelSetUp();
int wlanSetUp();
void setupMenu();
void exitMenu();

char* getInfo(char* buffer, char* string, char* value);
int getBytes(char* recvbuffer, int buflen, char* endstring);
void numberKeys(int number, int row, int column, int* buffer, int offset);

int serial = SERIAL2;  // number of serial port for module


int WiFiSetUp()
{	int pos = 0;
	int error=0;
    int key;
	char* last;
	int i;
	char mac[13];
	char ip[16];
	char netmask[16];
	char speed[7];
	char ifmode[3];
	char flow[8];
	char port[8];
	char connectmode[8];
	char datagramtype[]={'-','-','\0'};
	char wlan[16];
	char ssid[16];
	char wep[8];
	char recvbuffer[2500];
	BYTE c ;
	
	LCDClear();
	LCDPutString("WLAN SETUP\n");
	LCDPutString("----------------");
	LCDPutString("Press Reset and\n");
	LCDPutString("then select\nserial port");
	LCDMenu("Sr1","Sr2","Sr3","END");
	
    switch(KEYGet())
  	{ case KEY1: serial=SERIAL1; break;
  	  case KEY2: serial=SERIAL2; break;
  	  case KEY3: serial=SERIAL3; break;
  	  case KEY4: return 0;
    }
	
	error = OSInitRS232(SER9600, NONE, serial);	//Open RS232 on serial
	if(error)
	{
		LCDPrintf("RS232 error\n");
		return 1;
	}

  	key = KEYGet();
  	switch(key)
  	{
  		case(KEY4):	 			//END - key
			//reset the COM port of EyeBot again
  			OSInitRS232(SER115200, NONE, serial);return 1;break;
  	
		case(KEY1):				//X - key
			i=0;
			LCDPrintf("Connect...\n");
			while( OSCheckInRS232(serial) <= 0 && KEYRead() != KEY4)	//send 'x' until WiPort responds
			{
				c = ((BYTE) 0x78);
				OSSendCharRS232((char) c, serial);
			}
			LCDPrintf("connected\n");
			
			getBytes(recvbuffer, sizeof(recvbuffer), "Press Enter for Setup Mode");//receive mac-address
			getInfo(recvbuffer, "MAC address ", mac);				//get the mac out of the string
			
			c = ((BYTE) 0x0d);										//ENTER
			OSSendCharRS232((char) c, serial);						//enter setup
			break;
	}

	setupMenu();	//display mainmenu
	
	while(1)
	{
		key = KEYGet();
		switch (key)
		{
		case KEY1:							//Info-Button

			LCDClear();
			LCDSetPos(3,0);
			LCDPrintf("please wait...");
			OSSendCharRS232((BYTE)0x0d, serial);
			//read the whole info string from WiPort
			getBytes(recvbuffer, sizeof(recvbuffer), "9 Save and exit");	
			
			if(pos==0)						//Server Info
			{
				last = getInfo(recvbuffer, "IP addr ", ip);
				getInfo(last, "netmask ", netmask);

				LCDClear();
				LCDMenu(" ", " ", " ", "Back");
				LCDPrintf("MAC:%s",mac);
				LCDPrintf("IP:\n%s\n",ip);
				LCDPrintf("Netmask:\n%s",netmask);
				key=KEYGet();
				switch(key)
				{
				case KEY4:
					setupMenu();	
					pos=0;
					OSSleep(10);     //to avoid a double press of KEY4
					continue;
				}
			}
	          
			if(pos==1)						//Channel 1 Info
			{
				last = getInfo(recvbuffer, "Baudrate ", speed);
				last = getInfo(last, "I/F Mode ", ifmode);
				last = getInfo(last, "Flow  ", flow);
				last = getInfo(last, "Port ", port);
				last = getInfo(last, "Connect Mode : ", connectmode);
				getInfo(last, "Datagram Type  ", datagramtype);

				LCDClear();
				LCDMenu(" ", " ", " ", "Back");
				LCDPrintf("Speed:%s\n",speed);
				LCDPrintf("I/F Mode:%s\n",ifmode);
				LCDPrintf("Flow:%s\n", flow);
				LCDPrintf("Port:%s\n",port);
				LCDPrintf("Connect Mode:%s\n",connectmode);
				LCDPrintf("Datagram Type:%s",datagramtype);
				key=KEYGet();
				switch(key)
				{
				case KEY4:
					setupMenu();	
					pos=0;
					OSSleep(10);     //to avoid a double press of KEY4
					continue;
				}
			}
	            
			if(pos==2)						//WLAN Info
			{

				last = getInfo(recvbuffer, "WLAN: ", wlan);
				last = getInfo(last, "Name: ", ssid);
				getInfo(last, "Security: ", wep);

        		LCDClear();
				LCDMenu(" ", " ", " ", "Back");
				LCDPrintf("WLAN:%s\n",wlan);
				LCDPrintf("SSID:%s\n",ssid);
				LCDPrintf("WEP:%s\n", wep);

				key=KEYGet();
				switch(key)
				{
				case KEY4:
					setupMenu();	
					pos=0;
					OSSleep(10);     //to avoid a double press of KEY4
					continue;
				}
				
			}    		
			break;

		case KEY2:          			//Set - Button

			if(pos==0)					//Server setup
			{
				serverSetUp();
				setupMenu();
			}
			if(pos==1)					//Channel 1 setup
			{
				channelSetUp();
				setupMenu();
			}							
			if(pos==2)					//WLAN setup
			{
        		wlanSetUp();
				setupMenu();
      		}
			break;

		case KEY3:						//NXT-Button
			if (pos==0)
			{
				LCDSetChar(2,15,' ');
				LCDSetChar(3,15,'*');
				LCDSetChar(4,15,' ');
				pos=1;
				break;
			}

			if (pos==1)
			{
				LCDSetChar(2,15,' ');
				LCDSetChar(3,15,' ');
				LCDSetChar(4,15,'*');
				LCDMenu("Inf", " ", "Nxt", "END");
				pos=2;
				break;
			}

			if (pos==2)
			{
				LCDSetChar(2,15,'*');
				LCDSetChar(3,15,' ');
				LCDSetChar(4,15,' ');
				LCDMenu("Inf", "Set", "Nxt", "END");
				pos=0;
				break;
			}           
			break;

		case KEY4:      //go to exit menu
			exitMenu();
			return 0;				
		}
	}
}

//Main Menu
void setupMenu()
{	
	LCDClear();
	LCDPutString(" **WLAN SETUP**\n");
	LCDPutString("----------------");
	LCDPutString("Server:\nChannel 1:\nWLAN:\n");
	LCDMenu("Inf","Set","Nxt","END");
	LCDSetChar(2, 15, '*');	
}

//Exit Menu - Choose to save or not to save settings
void exitMenu()
{
	int key;
	char recvbuffer[100];
	LCDClear();
	LCDPutString("WLAN SETUP\n");
	LCDPutString("----------------");
	LCDPutString("\nSave changes?");
	LCDMenu("No"," "," ","Yes");
	
	key=KEYGet();
	switch(key)
	{
	case KEY1:
		OSSendCharRS232((BYTE)0x38, serial);	//8 Save without changes
		OSSendCharRS232((BYTE)0x0d, serial);	//final Enter
		break;

	case KEY4:
		OSSendCharRS232((BYTE)0x39, serial);	//9 Save and exit
		OSSendCharRS232((BYTE)0x0d, serial);	//final Enter 
		LCDClear();
		LCDSetPos(3,0);
		if((getBytes(recvbuffer, sizeof(recvbuffer), "stored")) > 0)
			LCDPrintf("..changes stored");
		else
			LCDPrintf("..error!\nNothing saved!");
		break;
	}
	OSSleep(50);
	OSInitRS232(SER115200, NONE, serial);	//reset EyeBot's COM port
}


//Change settings for server
int serverSetUp()
{
	int adr[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int mask[2]={0,0};
	int number[2] = {0,0};
	int col[2]={0,0};
	int key;
	int pos=0;
	char ip[16];
	char netmask[16];
	char recvbuffer[2500];
	char* start;
	char* end;
	char* startposition;
	int counter=0;
	int octet;
	int maske=0;
	char temp[4];
	int octet_value;
	
	LCDClear();
	LCDSetPos(3,0);
	LCDPrintf("please wait...");
	OSSendCharRS232((BYTE)0x0d, serial);
	getBytes(recvbuffer, sizeof(recvbuffer), "9 Save and exit");	//read the whole info			
	
	getInfo(recvbuffer, "IP addr ", ip);
	getInfo(recvbuffer, "netmask ", netmask);


	// read and evaluate the ip address
	startposition = ip;
	for(octet=0; octet<4; octet++)   //4 octets of ip-address
	{
	    end = strchr(startposition,'.');  //get pointer to first '.'
	    if(end == NULL)
	         end = strchr(startposition,'\0');     //or '\0' if end
	    start = startposition;                                         
	    counter=0;
	    
	    while(start != end  && KEYRead()!=KEY4)     //get the length of the octet (1..3)
	    {
	         start++;
	         counter++;
        }
        
        LCDPrintf("count%d\n",counter);
        switch(counter)                               //change the string to int
        {
        case 3: adr[0+4*octet] = *startposition-0x30;
                adr[1+4*octet] = *(startposition+1)-0x30;
                adr[2+4*octet] = *(startposition+2)-0x30;
                break;
            
        case 2: adr[0+4*octet] = 0;
                adr[1+4*octet] = *startposition-0x30;
                adr[2+4*octet] = *(startposition+1)-0x30;
                break;
                
        case 1: adr[0+4*octet] = 0;
                adr[1+4*octet] = 0;
                adr[2+4*octet] = *startposition-0x30;
                break;
        
        default: break;
        }
        startposition = end+1;                //jump to the next octet
     } 
     
     //read and evaluate the netmask
     startposition = netmask;   LCDClear(); 
     for(octet=0;octet<4;octet++)
     {
     
          end = strchr(startposition,'.');
          if(end == NULL)
                    end = strchr(startposition,'\0');
          start = startposition;
          counter = 0;
          
          while(start != end  && KEYRead()!=KEY4)     //get the length of the octet
          {
	         start++;
	         counter++;
          }
          
          switch(counter)
          {
          case 1:
                    maske += 8;          //then octet = 0
          
          case 3:   
                    memcpy(temp,startposition,3);
                    temp[3]='\0';
                    octet_value = atoi(temp);
                                   
					//only this are valid values for a netmask octet
                    switch(octet_value)
                    {
                    case 128: maske += 7;break;
                    case 192: maske += 6;break;
                    case 224: maske += 5;break;
                    case 240: maske += 4;break;
                    case 248: maske += 3;break;
                    case 252: maske += 2;break;
                    case 254: maske += 1;break;
                    case 255: break;
                    }
          }
          startposition = (end+1);
     }
    
	//netmask is in setup menu given as number of host bits
	//so after we read the netmask in octets we now change it in the
	//appropriate host bit number
    mask[0] = maske/10;
    mask[1] = maske%10;
                
    LCDClear();
	LCDMenu("+", "->", "Nxt", "Bck");
	LCDPrintf("IP Address:\n");
	LCDPrintf("%d%d%d.%d%d%d.%d%d%d.%d%d%d",
              adr[0],adr[1],adr[2],adr[4],adr[5],adr[6],
              adr[8],adr[9],adr[10],adr[12],adr[13],adr[14]);
	LCDSetPos(3,0);
	LCDPrintf("Mask: Number of\nBits for Host:\n");
	LCDPrintf("%d%d",mask[0],mask[1]);
	LCDSetChar(0, 15, '*');	
	
     
	while(key != KEY4)
	{
		key = KEYGet();
		switch (key)
		{
   		case KEY1:          				//+ Button
			if(pos==0)
			{
				number[0]++;
        		if (number[0]>9) 
           			number[0]=0;
        		LCDSetPos(2, col[0]);
				numberKeys(number[0], 1, col[0], adr, 0);
			}
	          
			if(pos==1)
			{
				number[1]++;
          		if (number[1]>9) 
            		number[1]=0;
          		LCDSetPos(4, col[1]);
				numberKeys(number[1], 5, col[1], mask, 0);
			}    
			break;

		case KEY2:          			//-> Button
			if(pos==0)
			{
				number[0]=0;
          		col[0]++;					//go to next address value
          		switch(col[0])				//skip the '.' in the IP 
				{
				case 3: col[0] = 4; break;
				case 7: col[0] = 8; break;
				case 11: col[0] = 12; break;
				case 15: col[0] = 0; break;
				}		
			}

			if(pos==1)
			{
				number[1]=0;
          		col[1]++;					//go to next address value
				if(col[1]>1) col[1]=0;
			}      
			break;

		case KEY3:						//NXT-Button
			if (pos==0)
			{
				LCDSetChar(0,15,' ');
				LCDSetChar(3,15,'*');  
				pos=1;
				break;
			}

			if (pos==1)
			{
				LCDSetChar(0,15,'*');
				LCDSetChar(3,15,' ');
				pos=0;
				break;	
			} 
			break;

		case KEY4:      //send settings to HandyPort

			OSSendCharRS232((BYTE)0x30, serial);	//'0'
			OSSendCharRS232((BYTE)0x0d, serial);	//Enter
			//OSSleep(10);
			getBytes(recvbuffer, sizeof(recvbuffer), ") ");
			OSSendCharRS232(((BYTE)(adr[0] + 0x30)), serial);	//first ip-triple
			OSSendCharRS232(((BYTE)(adr[1] + 0x30)), serial);
			OSSendCharRS232(((BYTE)(adr[2] + 0x30)), serial);
			OSSendCharRS232((BYTE)0x0d, serial);	//Enter
			//OSSleep(10);
			getBytes(recvbuffer, sizeof(recvbuffer), ") ");
			OSSendCharRS232(((BYTE)(adr[4] + 0x30)), serial);	//second ip-triple
			OSSendCharRS232(((BYTE)(adr[5] + 0x30)), serial);
			OSSendCharRS232(((BYTE)(adr[6] + 0x30)), serial);
			OSSendCharRS232((BYTE)0x0d, serial);	//Enter
			//OSSleep(10);
			getBytes(recvbuffer, sizeof(recvbuffer), ") ");
			OSSendCharRS232(((BYTE)(adr[8] + 0x30)), serial);	//third ip-triple
			OSSendCharRS232(((BYTE)(adr[9] + 0x30)), serial);
			OSSendCharRS232(((BYTE)(adr[10] + 0x30)), serial);
			OSSendCharRS232((BYTE)0x0d, serial);	//Enter
			//OSSleep(10);
			getBytes(recvbuffer, sizeof(recvbuffer), ") ");
			OSSendCharRS232(((BYTE)(adr[12] + 0x30)), serial);	//fourth ip-triple
			OSSendCharRS232(((BYTE)(adr[13] + 0x30)), serial);
			OSSendCharRS232(((BYTE)(adr[14] + 0x30)), serial);
			OSSendCharRS232((BYTE)0x0d, serial);	//Enter after last IP triple
			//OSSleep(10);
			getBytes(recvbuffer, sizeof(recvbuffer), "Gateway");
			OSSendCharRS232((BYTE)0x0d, serial);	//Enter after gateway
			//OSSleep(10);
			getBytes(recvbuffer, sizeof(recvbuffer), "Host Part");
			OSSendCharRS232(((BYTE)(mask[0] + 0x30)), serial);	//netmask
			OSSendCharRS232(((BYTE)(mask[1] + 0x30)), serial);
			OSSendCharRS232((BYTE)0x0d, serial);	//Enter after netmask
			//OSSleep(10);
			getBytes(recvbuffer, sizeof(recvbuffer), "password");
			OSSendCharRS232((BYTE)0x0d, serial);	//Enter after telnet pwd
			//OSSleep(10);
			getBytes(recvbuffer, sizeof(recvbuffer), "without save");
			OSSleep(10);
			
		}
	}
	return 0;
}


//Changes COM port settings
int channelSetUp()
{
	int serspeeds[]={9600,19200,38400,57600,115200,921600};
	int serifmodes[]={76,120};
	int serflows[]={0,1,2,5};
	int sercomodes[]={204,192,196};
	int sertypes[]={0,1};
	int baudrate=0;
	int ifmode=0;
	int flow=0;
	int port[5]={0,0,0,0,0};
	int connectmode=0;
	int datagramtype=0;
	int number=0;
	int col[6]= {0,0,0,7,12,0};	//offset where to print
	int key;
	int pos=0;
	int i,j=4;
	char portvalue[6];
	char recvbuffer[2500];
	char* last;
	

	LCDClear();
	LCDSetPos(3,0);
	LCDPrintf("please wait...");
	OSSendCharRS232((BYTE)0x0d, serial);
	getBytes(recvbuffer, sizeof(recvbuffer), "9 Save and exit");	//read the whole info			
	
	last = strstr(recvbuffer,"Channel");	//jump to the ***Channel section of the menu
	getInfo(last, "Port", portvalue);
	//fill the port array with the portvalue
	for(i=strlen(portvalue); i>0; i--)
		port[j--] = portvalue[i-1] - 0x30;	//convert char in int

	LCDClear();
	LCDMenu("+", "-", "Nxt", "Bck");
	LCDPrintf("Speed:\n");
	LCDPrintf("I/F Mode:\n");
	LCDPrintf("Flow:\n");
	LCDPrintf("PortNo:00000");
	LCDSetString(3,(12-strlen(portvalue)),portvalue);
	LCDPrintf("\nConnectMode:00\n");
	LCDPrintf("DatagramType:\n");
	LCDSetChar(0, 15, '*');

	while(key != KEY4)
	{
		LCDSetPos(0,6);
     	LCDPutIntS(serspeeds[baudrate],6);
		LCDSetPos(1,9);
		LCDPutHex1(serifmodes[ifmode]);
		LCDSetPos(2,6);
		LCDPutHex1(serflows[flow]);
		LCDSetPos(4,12);
		LCDPutHex1(sercomodes[connectmode]);
		LCDSetPos(5,13);
		LCDPutIntS(sertypes[datagramtype],1);

		key = KEYGet();
		switch (key)
		{
   		case KEY1:          	//+ Button
			switch(pos)
			{
			case 0:		//baudrate
				baudrate++;
            	if (baudrate>4) baudrate=0;
				break;

			case 1:		//ifmode
				ifmode++;
				if (ifmode>1) ifmode=0;
				break;

			case 2:		//flow
				flow++;
       			if (flow>3) flow=0;
				break;

			case 3:		//port number
				number++;
       			if (number>9) 
           			number=0;
       			LCDSetPos(3, col[3]);
           		numberKeys(number, 3, col[3], port, 7);
				break;

			case 4:		//connect mode
				connectmode++;
				if (connectmode>2) connectmode=0;
				break;

			case 5:		//datagram type
				datagramtype++;
       			if (datagramtype>1) datagramtype=0;
				break;
			}
			break;

		
		case KEY2:          	//- Button
			switch(pos)
			{
			case 0:		//baudrate
				baudrate--;
				if (baudrate<0) baudrate=4;
				break;
			
			case 1:		//ifmode
				ifmode--;
				if (ifmode<0) ifmode=1;
				break;

			case 2:		//flow
				flow--;
          		if (flow<0) flow=3;
				break;

			case 3:		//port number
				number=0;
          		col[3]++;					//go to next value
          		if(col[3]>11) col[3]=7;	
				break;

			case 4:		//connect mode
				connectmode--;
				if (connectmode<0) connectmode=2;
				break;

			case 5:		//datagram type
				datagramtype--;
          		if (datagramtype<0) datagramtype=1;	
				break;
			}
			break;

		case KEY3:						//NXT-Button
			switch(pos)
			{
			case 0:
				LCDSetChar(0,15,' ');
				LCDSetChar(1,15,'*');
				LCDSetChar(2,15,' ');
				LCDSetChar(3,15,' ');
				LCDSetChar(4,15,' ');
				LCDSetChar(5,15,' ');
				pos=1;
				break;

			case 1:
				LCDSetChar(0,15,' ');
				LCDSetChar(1,15,' ');
				LCDSetChar(2,15,'*');
				LCDSetChar(3,15,' ');
				LCDSetChar(4,15,' ');
				LCDSetChar(5,15,' ');              
				pos=2;
				break;

			case 2:
				LCDSetChar(0,15,' ');
				LCDSetChar(1,15,' ');
				LCDSetChar(2,15,' ');
				LCDSetChar(3,15,'*');
				LCDSetChar(4,15,' ');
				LCDSetChar(5,15,' ');
				LCDMenu("+","->","Nxt","Bck");	//change menu for port number & connectmode
				pos=3;
				break;

			case 3:
				LCDSetChar(0,15,' ');
				LCDSetChar(1,15,' ');
				LCDSetChar(2,15,' ');
				LCDSetChar(3,15,' ');
				LCDSetChar(4,15,'*');
				LCDSetChar(5,15,' ');
				LCDMenu("+","-","Nxt","Bck");	//change it back
				pos=4;
				break;

			case 4:
				LCDSetChar(0,15,' ');
				LCDSetChar(1,15,' ');
				LCDSetChar(2,15,' ');
				LCDSetChar(3,15,' ');
				LCDSetChar(4,15,' ');
				LCDSetChar(5,15,'*');
				pos=5;
				break;
	            
			case 5:
				LCDSetChar(0,15,'*');
				LCDSetChar(1,15,' ');
				LCDSetChar(2,15,' ');
				LCDSetChar(3,15,' ');
				LCDSetChar(4,15,' ');
				LCDSetChar(5,15,' ');      
				pos=0;
				break;
			}
			break;


		case KEY4:      //send settings to HandyPort
			
			OSSendCharRS232((BYTE)0x31, serial);	//'1'
			OSSendCharRS232((BYTE)0x0d, serial);	//Enter
			//OSSleep(10);
			getBytes(recvbuffer, sizeof(recvbuffer), ") ?");
			switch(baudrate)						//send baudrate
			{
			case 0:	
				OSSendCharRS232((BYTE)0x39, serial);
				OSSendCharRS232((BYTE)0x36, serial);break;
			case 1:
				OSSendCharRS232((BYTE)0x31, serial);
				OSSendCharRS232((BYTE)0x39, serial);
				OSSendCharRS232((BYTE)0x32, serial);break;
			case 2:
				OSSendCharRS232((BYTE)0x33, serial);
				OSSendCharRS232((BYTE)0x38, serial);
				OSSendCharRS232((BYTE)0x34, serial);break;
			case 3:
				OSSendCharRS232((BYTE)0x35, serial);
				OSSendCharRS232((BYTE)0x37, serial);
				OSSendCharRS232((BYTE)0x36, serial);break;
			case 4:
				OSSendCharRS232((BYTE)0x31, serial);
				OSSendCharRS232((BYTE)0x31, serial);
				OSSendCharRS232((BYTE)0x35, serial);
				OSSendCharRS232((BYTE)0x32, serial);break;
			}
			
			OSSendCharRS232((BYTE)0x30, serial);
			OSSendCharRS232((BYTE)0x30, serial);
			OSSendCharRS232((BYTE)0x0d, serial);	//Enter	
			//OSSleep(10);	
			getBytes(recvbuffer, sizeof(recvbuffer), ") ?");
			switch(ifmode)						//send ifmode
			{
			case 0:	
				OSSendCharRS232((BYTE)0x34, serial);		//4
				OSSendCharRS232((BYTE)0x43, serial);break;	//C
			case 1:
				OSSendCharRS232((BYTE)0x37, serial);		//7
				OSSendCharRS232((BYTE)0x38, serial);break;	//8
			}
			OSSendCharRS232((BYTE)0x0d, serial);	//Enter
			
			//OSSleep(10);
			getBytes(recvbuffer, sizeof(recvbuffer), ") ?");
			switch(flow)						//send flow
			{
			case 0:	
				OSSendCharRS232((BYTE)0x30, serial);break;
			case 1:
				OSSendCharRS232((BYTE)0x31, serial);break;
			case 2:
				OSSendCharRS232((BYTE)0x32, serial);break;
			case 3:
				OSSendCharRS232((BYTE)0x35, serial);break;
			}
			OSSendCharRS232((BYTE)0x0d, serial);	//Enter
			//OSSleep(10);
			getBytes(recvbuffer, sizeof(recvbuffer), ") ?");
			OSSendCharRS232(((BYTE)(port[0] + 0x30)), serial);	//port
			OSSendCharRS232(((BYTE)(port[1] + 0x30)), serial);
			OSSendCharRS232(((BYTE)(port[2] + 0x30)), serial);
			OSSendCharRS232(((BYTE)(port[3] + 0x30)), serial);
			OSSendCharRS232(((BYTE)(port[4] + 0x30)), serial);
			OSSendCharRS232((BYTE)0x0d, serial);	//Enter 
			//OSSleep(10);
			getBytes(recvbuffer, sizeof(recvbuffer), ") ?");
			OSSendCharRS232((BYTE)0x43, serial);	// send 'C'  //send connectmode
			switch(connectmode)
			{
			case 0:
				OSSendCharRS232((BYTE)0x43, serial);break;
			case 1:
				OSSendCharRS232((BYTE)0x30, serial);break;
			case 2:
				OSSendCharRS232((BYTE)0x34, serial);break;
			}
			OSSendCharRS232((BYTE)0x0d, serial);	//Enter 
			//OSSleep(10);
			if(connectmode==0)
			{
				getBytes(recvbuffer, sizeof(recvbuffer), ") ?");
				OSSendCharRS232(((BYTE)datagramtype + 0x30), serial);	//send datagramtype
				OSSendCharRS232((BYTE)0x0d, serial);	//Enter
			}
			else
			{
				for(i=0; i<12; i++)
				{
					getBytes(recvbuffer, sizeof(recvbuffer), ") ");
					//OSSleep(10);
					OSSendCharRS232((BYTE)0x0d, serial);	//Enter
					LCDPrintf("%d ",i);
				}					
			}
			//OSSleep(10);
			getBytes(recvbuffer, sizeof(recvbuffer), "choice");
			OSSleep(10);     //to avoid a double press of KEY4
			return(0);
			
		}
	}
	return 0;
}


int wlanSetUp()
{
	LCDClear();
	LCDMenu(" "," "," ","Back");
	LCDPrintf("Nothing at all!\n");
	/*LCDSetChar(2,7,'*');
	LCDSetChar(2,9,'*');
	LCDSetChar(3,8,'|');
	LCDSetChar(4,7,'\\');
	LCDSetChar(4,8,'-');
	LCDSetChar(4,9,'/');
	if(KEYGet())*/
	return 0;
}


//##################################################################
// this function is used for displaying the hexadecimal numbers
// in the specified row and column. The int value will be stored in
// the buffer @ 'column-offset'. 'offset' makes sure, that the buffer starts
// at index=0 if the displayed number does not start at position 0.
void numberKeys(int number, int row, int column, int* buffer, int offset)
{
	switch(number)
    {
		case 0: LCDSetChar(row,column,'0');buffer[column-offset]=0;break;
		case 1: LCDSetChar(row,column,'1');buffer[column-offset]=1;break;
		case 2: LCDSetChar(row,column,'2');buffer[column-offset]=2;break;
		case 3: LCDSetChar(row,column,'3');buffer[column-offset]=3;break;
		case 4: LCDSetChar(row,column,'4');buffer[column-offset]=4;break;
		case 5: LCDSetChar(row,column,'5');buffer[column-offset]=5;break;
		case 6: LCDSetChar(row,column,'6');buffer[column-offset]=6;break;
		case 7: LCDSetChar(row,column,'7');buffer[column-offset]=7;break;
		case 8: LCDSetChar(row,column,'8');buffer[column-offset]=8;break;
		case 9: LCDSetChar(row,column,'9');buffer[column-offset]=9;break;
		case 10: LCDSetChar(row,column,'A');buffer[column-offset]=10;break;
		case 11: LCDSetChar(row,column,'B');buffer[column-offset]=11;break;
		case 12: LCDSetChar(row,column,'C');buffer[column-offset]=12;break;
		case 13: LCDSetChar(row,column,'D');buffer[column-offset]=13;break;
		case 14: LCDSetChar(row,column,'E');buffer[column-offset]=14;break;
		case 15: LCDSetChar(row,column,'F');buffer[column-offset]=15;break;
	}
}




//################################################################
// getInfo() looks in buffer for the 'string' and returns the pointer
// to where it was found. The searched info string will be stored 
// in 'value'

char* getInfo(char* buffer, char* string, char* value)
{
	char* pointer;
	char* cp;
	int blankcounter=0;
	int start=0;
	int counter=0;

	pointer = strstr(buffer, string);
	if(pointer==NULL)
	{
		strcpy(value, "-");
		return pointer;
	}
	else
	{
		cp = pointer + strlen(string);
		while( *cp != 0x0D &&  *cp != 0x0A &&  *cp != ',')
		{	
			if(*cp == ' ' && !start)
			{
				blankcounter++;
				cp++;
			}
			else if(*cp == ' ' && start)
				break;
			else
			{
				start = 1;
				cp++;
				counter++;
			}
		}

		memcpy(value, pointer+strlen(string)+blankcounter, counter);
		value[counter]='\0';
		return pointer;
	}
}



//################################################################
// getBytes reads bytes through RS232 at least until the end_string 
// has been read. Function returns number of read bytes or 0 if
// Key4 was pressed.
int getBytes(char* recvbuffer, int buflen, char* end_string)
{
	//recv all info from the WiPort
	int end = 0;
	int x=0;

	memset(recvbuffer,0,buflen);
	while(!end && KEYRead() != KEY4)
	{
		if(OSCheckInRS232(serial)>0)
		{
			OSRecvRS232(&recvbuffer[x],serial);
			x++;
		}
		else
		{
			recvbuffer[x]='\0';
			if((strstr(recvbuffer,end_string)) != NULL)
			{
				end = 1;
			}
		}
	}
	if(end == 1)
		return x;
	else
		return 0;
}


