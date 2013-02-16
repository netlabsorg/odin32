// test.cpp : Defines the entry point for the console application.
//

#define UNICODE
#include <windows.h>
#include <mmsystem.h>

#include <stdio.h>

int mixertypes[] = {
 MIXERCONTROL_CONTROLTYPE_CUSTOM         ,
 MIXERCONTROL_CONTROLTYPE_BOOLEANMETER   ,
 MIXERCONTROL_CONTROLTYPE_SIGNEDMETER    ,
 MIXERCONTROL_CONTROLTYPE_PEAKMETER      ,
 MIXERCONTROL_CONTROLTYPE_UNSIGNEDMETER  ,
 MIXERCONTROL_CONTROLTYPE_BOOLEAN        ,
 MIXERCONTROL_CONTROLTYPE_ONOFF          ,
 MIXERCONTROL_CONTROLTYPE_MUTE           ,
 MIXERCONTROL_CONTROLTYPE_MONO           ,
 MIXERCONTROL_CONTROLTYPE_LOUDNESS       ,
 MIXERCONTROL_CONTROLTYPE_STEREOENH      ,
 MIXERCONTROL_CONTROLTYPE_BUTTON         ,
 MIXERCONTROL_CONTROLTYPE_DECIBELS       ,
 MIXERCONTROL_CONTROLTYPE_SIGNED         ,
 MIXERCONTROL_CONTROLTYPE_UNSIGNED       ,
 MIXERCONTROL_CONTROLTYPE_PERCENT        ,
 MIXERCONTROL_CONTROLTYPE_SLIDER         ,
 MIXERCONTROL_CONTROLTYPE_PAN            ,
 MIXERCONTROL_CONTROLTYPE_QSOUNDPAN      ,
 MIXERCONTROL_CONTROLTYPE_FADER          ,
 MIXERCONTROL_CONTROLTYPE_VOLUME         ,
 MIXERCONTROL_CONTROLTYPE_BASS           ,
 MIXERCONTROL_CONTROLTYPE_TREBLE         ,
 MIXERCONTROL_CONTROLTYPE_EQUALIZER      ,
 MIXERCONTROL_CONTROLTYPE_SINGLESELECT   ,
 MIXERCONTROL_CONTROLTYPE_MUX            ,
 MIXERCONTROL_CONTROLTYPE_MULTIPLESELECT ,
 MIXERCONTROL_CONTROLTYPE_MIXER          ,
 MIXERCONTROL_CONTROLTYPE_MICROTIME      ,
 MIXERCONTROL_CONTROLTYPE_MILLITIME      
};


int MixGetControls(MIXERLINE &line);

int main(int argc, char* argv[])
{
	UINT mixid;
	MMRESULT ret;
	int i;

	ret = mixerGetID(0, &mixid, 0x10000000);

	MIXERCAPS caps;
	
	ret = mixerGetDevCaps(0, &caps, sizeof(caps));
	printf("Nr of destination %d\n", caps.cDestinations);

	for(i=0;i<caps.cDestinations;i++) {
		MIXERLINE line = {0};
		line.cbStruct = sizeof(MIXERLINE);
		line.dwDestination = i;
		ret = mixerGetLineInfo(0, &line, MIXER_GETLINEINFOF_DESTINATION);
		if(ret != MMSYSERR_NOERROR) {
			printf("mixerGetLineInfo returned error %d\n", ret);
			return 1;
		}

		printf("\nDestination %d (%ls) (source %d) connections %d controls %d\n", i, line.szName, line.dwSource, line.cConnections, line.cControls);

		MixGetControls(line);
		for(int j=0;j<line.cConnections;j++) {
			MIXERLINE src = {0};
			src.cbStruct = sizeof(MIXERLINE);
			src.dwSource = j;
			src.dwDestination = line.dwDestination;
			ret = mixerGetLineInfo(0, &src, MIXER_GETLINEINFOF_SOURCE);
			if(ret != MMSYSERR_NOERROR) {
				printf("mixerGetLineInfo returned error %d\n", ret);
				return 1;
			}

			printf("Source %d (%ls) connections %d controls %d\n", j, src.szName, src.cConnections, src.cControls);
			MixGetControls(src);
		}
	}

	return 0;
}

int MixGetControls(MIXERLINE &line)
{
	MMRESULT ret;

		for(int j=0;j<sizeof(mixertypes)/sizeof(int);j++) {
			MIXERLINECONTROLS mxlc;
			MIXERCONTROL mctrl;

			mxlc.cbStruct = sizeof(mxlc);
			mxlc.pamxctrl = &mctrl;
			mxlc.dwControlType = mixertypes[j];
			mxlc.dwLineID      = line.dwLineID;
			mxlc.cControls     = 1;
			mxlc.cbmxctrl      = sizeof(MIXERCONTROL);

			mctrl.cbStruct = sizeof(MIXERCONTROL);

			ret = mixerGetLineControls(0, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE);
			if(ret == MMSYSERR_NOERROR) {
				printf("Control %d (%ls) type %x mitems %d min/max (%d,%d) step %d\n", mctrl.dwControlID, mctrl.szName, mctrl.dwControlType, mctrl.cMultipleItems, mctrl.Bounds.dwMinimum, mctrl.Bounds.dwMaximum, mctrl.Metrics.cSteps);
 				MIXERLINECONTROLS mxlc1;
				MIXERCONTROL mctrl1;

				mxlc1.cbStruct = sizeof(mxlc);
				mxlc1.pamxctrl = &mctrl1;
				mxlc1.dwControlID   = mctrl.dwControlID	;
				mxlc1.dwLineID      = line.dwLineID;
				mxlc1.cControls     = 1;
				mxlc1.cbmxctrl      = sizeof(MIXERCONTROL);

				mctrl1.cbStruct = sizeof(MIXERCONTROL);

				ret = mixerGetLineControls(0, &mxlc1, MIXER_GETLINECONTROLSF_ONEBYID);
				if(ret != MMSYSERR_NOERROR) {
					printf("mixerGetLineControls returned error %d\n", ret);
					return 1;
				}

				switch(mctrl.dwControlType) {
				case MIXERCONTROL_CONTROLTYPE_VOLUME:
				{
					MIXERCONTROLDETAILS mxcd = {0};
					MIXERCONTROLDETAILS_UNSIGNED val[2] = {0};

					mxcd.cbStruct = sizeof(mxcd);
					mxcd.cChannels = line.cChannels;
					mxcd.dwControlID = mctrl.dwControlID;
					mxcd.paDetails = &val[0];
					mxcd.cbDetails = sizeof(val);
					ret = mixerGetControlDetails(0, &mxcd, MIXER_GETCONTROLDETAILSF_VALUE);
					if(ret != MMSYSERR_NOERROR) {
						printf("mixerGetControlDetails returned error %d\n", ret);
						return 1;
					}
					if(line.cChannels == 2) {
						printf("Volume (%d,%d)\n", val[0].dwValue, val[1].dwValue);
					}
					else printf("Volume %d\n", val[0].dwValue);

					break;
				}
				case MIXERCONTROL_CONTROLTYPE_MUTE:
				{
					MIXERCONTROLDETAILS mxcd = {0};
					MIXERCONTROLDETAILS_BOOLEAN val;

					mxcd.cbStruct = sizeof(mxcd);
					mxcd.cChannels = 1;
					mxcd.dwControlID = mctrl.dwControlID;
					mxcd.paDetails = &val;
					mxcd.cbDetails = sizeof(val);
					ret = mixerGetControlDetails(0, &mxcd, MIXER_GETCONTROLDETAILSF_VALUE);
					if(ret != MMSYSERR_NOERROR) {
						printf("mixerGetControlDetails returned error %d\n", ret);
						return 1;
					}
					printf("Mute %d\n", val.fValue);
					break;
				}
				case MIXERCONTROL_CONTROLTYPE_MUX:
				{
					MIXERCONTROLDETAILS mxcd = {0};
					MIXERCONTROLDETAILS_BOOLEAN val[16];

					mxcd.cbStruct = sizeof(mxcd);
					mxcd.cChannels = 1;
					mxcd.dwControlID = mctrl.dwControlID;
					mxcd.cMultipleItems = line.cConnections;
					mxcd.paDetails = &val[0];
					mxcd.cbDetails = sizeof(val[0]);
					ret = mixerGetControlDetails(0, &mxcd, MIXER_GETCONTROLDETAILSF_VALUE);
					if(ret != MMSYSERR_NOERROR) {
						printf("mixerGetControlDetails returned error %d\n", ret);
						return 1;
					}
					MIXERCONTROLDETAILS_LISTTEXT tval[16] = {0};
					mxcd.cbDetails = sizeof(tval[0]);
					mxcd.paDetails = &tval[0];
					mxcd.cChannels = 1;
					
					ret = mixerGetControlDetails(0, &mxcd, MIXER_GETCONTROLDETAILSF_LISTTEXT);

					for(int k=0;k<line.cConnections;k++) {
						printf("Mux %d %d (%ls)\n", k, val[k].fValue, tval[k].szName);
					}
					break;
				}
				case MIXERCONTROL_CONTROLTYPE_BASS:
				{
					MIXERCONTROLDETAILS mxcd = {0};
					MIXERCONTROLDETAILS_UNSIGNED val;

					mxcd.cbStruct = sizeof(mxcd);
					mxcd.cChannels = line.cChannels;
					mxcd.dwControlID = mctrl.dwControlID;
					mxcd.paDetails = &val;
					mxcd.cbDetails = sizeof(val);
					ret = mixerGetControlDetails(0, &mxcd, MIXER_GETCONTROLDETAILSF_VALUE);
					if(ret != MMSYSERR_NOERROR) {
						printf("mixerGetControlDetails returned error %d\n", ret);
						return 1;
					}
					printf("Bass %d\n", val.dwValue);
					break;
				}
				case MIXERCONTROL_CONTROLTYPE_LOUDNESS:
				{
					MIXERCONTROLDETAILS mxcd = {0};
					MIXERCONTROLDETAILS_UNSIGNED val;

					mxcd.cbStruct = sizeof(mxcd);
					mxcd.cChannels = line.cChannels;
					mxcd.dwControlID = mctrl.dwControlID;
					mxcd.paDetails = &val;
					mxcd.cbDetails = sizeof(val);
					ret = mixerGetControlDetails(0, &mxcd, MIXER_GETCONTROLDETAILSF_VALUE);
					if(ret != MMSYSERR_NOERROR) {
						printf("mixerGetControlDetails returned error %d\n", ret);
						return 1;
					}
					printf("Loudness %d\n", val.dwValue);
					break;
				}
				case MIXERCONTROL_CONTROLTYPE_TREBLE:
				{
					MIXERCONTROLDETAILS mxcd = {0};
					MIXERCONTROLDETAILS_UNSIGNED val;

					mxcd.cbStruct = sizeof(mxcd);
					mxcd.cChannels = line.cChannels;
					mxcd.dwControlID = mctrl.dwControlID;
					mxcd.paDetails = &val;
					mxcd.cbDetails = sizeof(val);
					ret = mixerGetControlDetails(0, &mxcd, MIXER_GETCONTROLDETAILSF_VALUE);
					if(ret != MMSYSERR_NOERROR) {
						printf("mixerGetControlDetails returned error %d\n", ret);
						return 1;
					}
					printf("Treble %d\n", val.dwValue);
					break;
				}
				case MIXERCONTROL_CONTROLTYPE_ONOFF:
				{
					MIXERCONTROLDETAILS mxcd = {0};
					MIXERCONTROLDETAILS_BOOLEAN val;

					mxcd.cbStruct = sizeof(mxcd);
					mxcd.cChannels = line.cChannels;
					mxcd.dwControlID = mctrl.dwControlID;
					mxcd.paDetails = &val;
					mxcd.cbDetails = sizeof(val);
					ret = mixerGetControlDetails(0, &mxcd, MIXER_GETCONTROLDETAILSF_VALUE);
					if(ret != MMSYSERR_NOERROR) {
						printf("mixerGetControlDetails returned error %d\n", ret);
						return 1;
					}
					printf("OnOff %d\n", val.fValue);
					break;
				}
				case MIXERCONTROL_CONTROLTYPE_FADER:
				{
					MIXERCONTROLDETAILS mxcd = {0};
					MIXERCONTROLDETAILS_UNSIGNED val;

					mxcd.cbStruct = sizeof(mxcd);
					mxcd.cChannels = line.cChannels;
					mxcd.dwControlID = mctrl.dwControlID;
					mxcd.paDetails = &val;
					mxcd.cbDetails = sizeof(val);
					ret = mixerGetControlDetails(0, &mxcd, MIXER_GETCONTROLDETAILSF_VALUE);
					if(ret != MMSYSERR_NOERROR) {
						printf("mixerGetControlDetails returned error %d\n", ret);
						return 1;
					}
					printf("Fader %d\n", val.dwValue);
					break;
				}

				}
			}
		}
		return 0;
}
