nmake RELEASE=1 clean
nmake dep
nmake RELEASE=1 /a /i

nmake DEBUG=1 clean
nmake DEBUG=1 /a /i

@rem nmake PROFILE=1 DEBUG=1 clean
@rem nmake PROFILE=1 DEBUG=1 /a /i
