/* $Id: cout.h,v 1.3 1999-11-10 01:45:32 bird Exp $
 *
 * cout - cout replacement.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#error("Not used! Use kprintf/dprintf!")

#ifndef _cout_h_
#define _cout_h_
    #ifndef __cplusplus
        void coutInit(USHORT usCom);
    #else
        extern "C" {void coutInit(USHORT usCom);}

        #ifndef OUTPUT_COM1
            #define OUTPUT_COM1     0x3f8
        #endif

        #ifndef OUTPUT_COM2
            #define OUTPUT_COM2     0x2f8
        #endif

        /**
         * @prop    Provide easy Ring0 logging thru a COM port.
         * @desc    Provide easy Ring0 logging thru a COM port for C++ code.
         *          WARNING: It Yields CPU! using Yield();
         * @purpose Ring0 replacement for cout.
         * @author  knut st. osmundsen
         */
        class kLog
        {
            private:
                /** @cat Datamembers */
                /** */
                /** flags - hex or decimal */
                int  fHex;

                /** buffer to make sprintfs into */
                char szBuffer[32];

                /** port-address */
                unsigned long ulPort;

                /** @cat Methods */
                void putch(char ch);
                void putstr(const char *pszStr);

            public:
                /** @cat Constructors/Destructor */
                kLog();
                ~kLog();

                /* @cat Methods */
                void constructor(ULONG ulPort = OUTPUT_COM2);
                void setf(int, int);

                /** @cat Operators */
                kLog& operator<<(char);
                kLog& operator<<(const char*);
                kLog& operator<<(const signed char*);
                kLog& operator<<(const unsigned char*);
                kLog& operator<<(int);
                kLog& operator<<(long);             /**/
                kLog& operator<<(unsigned int);
                kLog& operator<<(unsigned long);    /**/
                kLog& operator<<(const void*);      /**/
                kLog& operator<<(short);
                kLog& operator<<(unsigned short);

                /** @cat Enums */
                enum {dec=0, hex=1, basefield=2};
        };


        /*******************/
        /* debug / release */
        /*******************/
        #ifndef RELEASE
            extern  kLog  _log_;
            #define cout  _log_
            #define ios   kLog
            #define endl  "\n"
        #else
            #define cout Yield();//
            #define ios  {}//
            #define endl {}//
        #endif

    #endif
#endif


