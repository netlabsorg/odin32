# $Id: setup.os2libs.mk,v 1.1 2002-08-30 19:45:30 bird Exp $

#
# The fixed library dependencies.
#
#

# Defaults
$(LIB_OS) \
$(LIB_OS) \
$(LIB_C_OBJ) \
$(LIB_C_DLL) \
$(LIB_C_RTDLL) \
$(LIB_C_NRE) \
$(LIB_C_DMNGL) \
$(OBJ_PROFILE) \
\
# (the toolkit452 lib directory listing.)
crypto.lib    \
dapwsock.lib  \
dmiapi.lib    \
encap.lib     \
ffst.lib      \
fss.lib       \
ftpapi.lib    \
glut.lib      \
lfapi.lib     \
libaux.lib    \
libcmi.lib    \
libcmsi.lib   \
libcni.lib    \
libcnsi.lib   \
libconv.lib   \
libcsi.lib    \
libcssi.lib   \
librsvp.lib   \
libtk.lib     \
libuls.lib    \
mailslot.lib  \
mmpm2.lib     \
netapi.lib    \
netapi32.lib  \
netapinc.lib  \
netoem.lib    \
opengl.lib    \
os2im.lib     \
os2286.lib    \
os2386.lib    \
pmbidi.lib    \
pmnls286.lib  \
pmnls386.lib  \
pmwinx.lib    \
pmwsock.lib   \
r0lib.lib     \
r0lib32.lib   \
rexx.lib      \
rpc32dll.lib  \
rtmidi.lib    \
rxstring.lib  \
so32dll.lib   \
tcp32dll.lib  \
tcpip32.lib   \
tcpipdll.lib  \
trace.lib     \
tunnel.lib    \
unikbd.lib    \
upm.lib       \
upm32.lib     \
vdh.lib       \
somcd.lib     \
somdbg.lib    \
somtk.lib     \
\
# misc ddk
addcalls.lib  \
dbcs32.lib    \
dbcslib.lib   \
dhcalls.lib   \
doscalls.lib  \
libh.lib      \
mmpm2.lib     \
os2286p.lib   \
os2386p.lib   \
pmwp.lib      \
rexx.lib      \
rmcalls.lib   \
thunkrt.lib   \
vdh.lib       \
                    :
