 #include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <termios.h>
  #include <stdio.h>

  /* Baudrate 설정은 <asm/termbits.h>에 정의되어 있다.*/
  /* <asm/termbits.h>는 <termios.h>에서 include된다. */
  #define BAUDRATE B230400
  /* 여기의 포트 장치 파일을 바꾼다. COM1="/dev/ttyS1, COM2="/dev/ttyS2 */
  #define MODEMDEVICE "/dev/ttyUSB0"
  //#define MODEMDEVICE "/dev/ttyMFD1"

  #define FALSE 0
  #define TRUE 1

  volatile int STOP=FALSE;

  int main()
  {
    int fd,c, res;
    struct termios oldtio,newtio;

    int tx_size,rx_size,col,raw,i;

    char Tx_Cmd[]="S";

    char Rx_Data[600];

    for (i=0;i<600;i++) Rx_Data[i]=10;

    printf("\r\n Smart Fabric Interface System");
    printf("\r\n Ver Beta V0.1");


   //fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY  );
   fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY |O_NONBLOCK );

   if (fd <0) {
	   printf("\r\n UART failed to setup\n");
	   exit(-1);

   }
   else
       {
       	printf("\r\n UART Port Initialization Done!");
       }


   tcgetattr(fd,&oldtio); /* save current serial port settings */
   bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */
   newtio.c_cflag = BAUDRATE | CS8 ;
   newtio.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,nable reading
   newtio.c_cflag &= ~(PARENB | PARODD);      // shut off parity
   newtio.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
   newtio.c_iflag = IGNPAR; //Ignore parity errors
   newtio.c_oflag = 0;
   newtio.c_lflag = 0;
   newtio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
   newtio.c_cc[VMIN]     = 0;     /* blocking read until 500 character arrives */

   tcflush(fd, TCIFLUSH);
   tcsetattr(fd,TCSANOW,&newtio);

  /*
    터미널 세팅이 끝났고, 이제는 입력을 처리한다.
    이 예제에서는 한 줄의 맨 첫 문자를 'z'로 했을 때 프로그램을
    종료한다.
  */

   printf("\r\n Tx Start -----------");

   for(i=0;i<10;i++)
   {

	   tx_size=write(fd,"S",1);
	   sleep(1);
	   rx_size = read(fd,&Rx_Data[0],576);


	   printf("\r\n TX [%d] : %c",tx_size,Tx_Cmd[0] );
	   printf("\r\n Rx Data Size : %d\n",rx_size);


		for (col=0; col<24; col++)
		{
			printf("[ ");
			for (raw=0; raw<24; raw++)
			{

				printf("%d ",Rx_Data[col*24+raw]);

			}
			printf("]\n");
		}


   }


   /* restore the old port settings */
   tcsetattr(fd,TCSANOW,&oldtio);
   close(fd);
  }
