//! GENINFO :: platform:OS/2, version:20.45, target:ApiTest.generate
/*****************************************************************************\
* ApiTestWin.hpp :: This is the Native Win32 variant of ApiTest               *
* --------------------------------------------------------------------------- *
* This variant is not 'Odin Aware' and is built as a Native Win32 application.*
* So it is in PE-format and reuires the special pe/pec loaders to run, or the *
* win32k.sys driver. These loaders convert the PE into an LX on-the-fly which *
* then accesses the Odin32 subsystem. This process is called 'Odinization',   *
* because the application is converted to adapt to Odin32.                    *
* Besides a Win32 application being built with non-OS/2 tools and headers,    *
* there is also a difference between the code-path taken by Odin32 internally *
* between 'Odin Based' and 'Odinized' applications. So this variant provides  *
* yet another angle on testing the Odin32-API.                                *
\*****************************************************************************/


#ifndef     __APITEST_HPP__
#define     __APITEST_HPP__
#ifdef      __cplusplus
    extern "C" {
#endif



#ifdef      __cplusplus
    }
#endif
#endif // __APITEST_HPP__
