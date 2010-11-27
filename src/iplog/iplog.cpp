#define INCL_DOSPROCESS       /* DOS process values (for DosSleep) */
#define INCL_DOSEXCEPTIONS    /* DOS exception values */
#define INCL_ERRORS           /* DOS error values     */
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>

 ULONG _System MyTermHandler( PEXCEPTIONREPORTRECORD       p1,
                              PEXCEPTIONREGISTRATIONRECORD p2,
                              PCONTEXTRECORD               p3,
                              PVOID                        pv );


#define WIN32_IP_LOG_PORT	5001

int s = -1;
FILE *logfile = NULL;

void main(void)
{
   int sockint, namelen, client_address_size;
   struct sockaddr_in client, server;
   char buf[4096];

   EXCEPTIONREGISTRATIONRECORD RegRec  = {0};     /* Exception Registration Record */
   APIRET      rc      = NO_ERROR;   /* Return code                   */
 
   /* Add MyTermHandler to this thread's chain of exception handlers */
 
   RegRec.ExceptionHandler = (ERR)MyTermHandler;
   rc = DosSetExceptionHandler( &RegRec );
   if (rc != NO_ERROR) {
      printf("DosSetExceptionHandler error: return code = %u\n",rc);
      return;
   }

   /*
    * Initialize with sockets.
    */
   if ((sockint = sock_init()) != 0)
   {
      printf(" INET.SYS probably is not running");
      exit(1);
   }

   /*
    * Create a datagram socket in the internet domain and use the
    * default protocol (UDP).
    */
   if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
   {
       psock_errno("socket()");
       exit(1);
   }

   /*
    *
    * Bind my name to this socket so that clients on the network can
    * send me messages. (This allows the operating system to demultiplex
    * messages and get them to the correct server)
    *
    * Set up the server name. The internet address is specified as the
    * wildcard INADDR_ANY so that the server can get messages from any
    * of the physical internet connections on this host. (Otherwise we
    * would limit the server to messages from only one network interface)
    */
   server.sin_family      = AF_INET;   /* Server is in Internet Domain */
   server.sin_port        = WIN32_IP_LOG_PORT;
   server.sin_addr.s_addr = INADDR_ANY;/* Server's Internet Address    */

   if (bind(s, (struct sockaddr *)&server, sizeof(server)) < 0)
   {
       psock_errno("bind()");
       exit(2);
   }

   logfile = fopen("odin32.log", "wb");

   while(TRUE) {
       /*
        * Receive a message on socket s in buf  of maximum size 32
        * from a client. Because the last two paramters
        * are not null, the name of the client will be placed into the
        * client data structure and the size of the client address will
        * be placed into client_address_size.
        */
       client_address_size = sizeof(client);
    
       if(recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *) &client,
                &client_address_size) <0)
       {
           psock_errno("recvfrom()");
           exit(4);
       }
       /*
        * Print the message and the name of the client.
        * The domain should be the internet domain (AF_INET).
        * The port is received in network byte order, so we translate it to
        * host byte order before printing it.
        * The internet address is received as 32 bits in network byte order
        * so we use a utility that converts it to a string printed in
        * dotted decimal format for readability.
        */
       buf[sizeof(buf)-1] = 0;
       int len = strlen(buf);
       fwrite(buf, 1, len, logfile);
       if(buf[len-1] != '\n') {
           fwrite("\n", 1, 1, logfile);
       }

       if(ftell(logfile) > 250*1024*1024) {
           fclose(logfile);
           logfile = fopen("odin32.log", "wb");
       }
   }
    
   /*
    * Deallocate the socket.
    */
   soclose(s);
}

 
 /***************************************************************************/
 ULONG _System MyTermHandler( PEXCEPTIONREPORTRECORD       p1,
                              PEXCEPTIONREGISTRATIONRECORD p2,
                              PCONTEXTRECORD               p3,
                              PVOID                        pv )
 {
   switch( p1->ExceptionNum) {
    case XCPT_PROCESS_TERMINATE:
    case XCPT_ASYNC_PROCESS_TERMINATE:
        if(logfile) {
            fflush(logfile);
            fprintf(logfile, "*****************************************************\n");
            fprintf(logfile, "End of logfile\n");
            fclose(logfile);
        }
        if(s != -1) soclose(s);
        break;
    default:;
   }
 
   return XCPT_CONTINUE_SEARCH;          /* Exception not resolved... */
 }
 

