
class OS2IDirectDraw
{
  public:
    //this one has to go first!
    IDirectDraw4Vtbl *lpVtbl;
    IDirectDraw2Vtbl  Vtbl;
    // MS did it again with direct/X 6 they changed the behavior of the directdraw component
    // So we now need 2 Virt. Tables one for the old and one for the new so we return the
    // right interface if one creates a DX6 directdraw object but wants a DX2,3,5 interface
    IDirectDraw4Vtbl  Vtbl4;
    IDirect3DVtbl     Vtbl3D;

    OS2IDirectDraw( GUID *lpGUID);
    ~OS2IDirectDraw();

    // Simple management for Surfaces should be good enougth for now
    inline  BOOL          HasPrimarySurface() { return PrimaryExists;}
            void          SetPrimarySurface(BOOL NewStatus){PrimaryExists = NewStatus;}
            int           Referenced;
    inline  HRESULT       GetLastError()    { return lastError;    }
    inline  char *        GetFrameBuffer()  { return pFrameBuffer; }
    inline  int           GetScreenWidth()  { return screenwidth;  }
    inline  int           GetScreenHeight() { return screenheight; }
    inline  int           GetScreenBpp()    { return screenbpp;}
    inline  FOURCC        GetScreenFourCC() { return SupportedFourCCs[screenbpp>>3];}

  protected:
    HRESULT lastError;
    FOURCC  SupportedFourCCs[] = {FOURCC_LUT8,FOURCC_R565,FOURCC_RGB3,FOURCC_RGB4};

    char   *pFrameBuffer;
    HWND    hwndClient;
    int     screenwidth, screenheight, screenbpp;
    BOOL    PrimaryExists;

   // Linked list management
              OS2IDirectDraw* next;                   // Next OS2IDirectDraw
    static    OS2IDirectDraw* ddraw;                  // List of OS2IDirectDraw
}
