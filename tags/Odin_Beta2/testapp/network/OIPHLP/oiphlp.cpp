/* OIPHLP.cpp : Defines the entry point for the console application.
 */

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <iphlpapi.h>


void doPrintIPAddressString(LPSTR lpstrTitle,
                            PIP_ADDR_STRING pipAddr)
{
	printf("%s\n"
		   "      next = %08xh\n"
		   "      addr = [%s]\n"
		   "      mask = [%s]\n"
		   "      ctx  = %08xh\n",
		lpstrTitle,
		pipAddr->Next,
		pipAddr->IpAddress,
		pipAddr->IpMask,
		pipAddr->Context);

	if (pipAddr->Next)
	{
		doPrintIPAddressString("(next address string",
			pipAddr->Next);
	}
}


void doPrintAdapterInfo(PIP_ADAPTER_INFO p)
{
	printf("Adapter [%08xh]\n",
		p);

	printf("  next Adapter [%08xh]\n"
		   "  index=%d\n"
		   "  name='%s'\n"
		   "  description='%s'\n"
		   "  address length=%d\n",
		   p->Next,
		   p->ComboIndex,
		   p->AdapterName,
		   p->Description,
		   p->AddressLength);
		   
	// print address
	int i;
	PBYTE pb;
	printf("  address '");
	for (i = 0, pb=p->Address;
		i < p->AddressLength;
		i++, pb++)
	{
		printf("%02xh ",
			*pb);
	}
	printf("\n");

	printf("  index=%d\n"
		   "  type=%d\n"
		   "  dhcp enabled=%d\n",
			p->Index,
			p->Type,
			p->DhcpEnabled);

	doPrintIPAddressString("  current ip address = ",
		p->CurrentIpAddress);

	doPrintIPAddressString("  IP address list = ",
		&p->IpAddressList);

	doPrintIPAddressString("  gateway list = ",
		&p->GatewayList);

	doPrintIPAddressString("  dhcp server = ",
		&p->DhcpServer);

	printf("  have wins = %d",
		&p->HaveWins);

	doPrintIPAddressString("  primary wins server = ",
		&p->PrimaryWinsServer);

	doPrintIPAddressString("  secondary wins server = ",
		&p->SecondaryWinsServer);

	printf("  lease obtained = %08xh"
		   "  lease expires  = %08xh",
		p->LeaseObtained,
		p->LeaseExpires);

	if (p->Next)
	{
		printf("\nNext Adapter ...");
		doPrintAdapterInfo(p->Next);
	}
}


void doGetAdaptersInfo(void)
{
	CHAR szBuf[4096];
	DWORD ulBufLen = sizeof(szBuf);
	DWORD rc;
	PIP_ADAPTER_INFO pAdapters;

	rc = GetAdaptersInfo ( (PIP_ADAPTER_INFO) szBuf,
						   (PULONG) &ulBufLen);
	printf("GetAdaptersInfo(%08xh, %08xh (len=%d)) returned %d\n",
		&szBuf,
		&ulBufLen,
		ulBufLen,
		rc);
	if (rc != ERROR_SUCCESS)
	{
		printf("Aborting\n");
		return;
	}

	// OK, display formatted results
	pAdapters = (PIP_ADAPTER_INFO)szBuf;


}


int main(int argc, char* argv[])
{
	printf("ODIN GetAdaptersInfo Testcase!\n");

	doGetAdaptersInfo();

	return 0;
}

