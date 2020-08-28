/*******************************************************************************
Copyright 2016 Microchip Technology Inc. (www.microchip.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

To request to license the code under the MLA license (www.microchip.com/mla_license), 
please contact mla_licensing@microchip.com
*******************************************************************************/

/** INCLUDES *******************************************************/
#include "usb.h"

#include "usb_device_generic.h"

#include "system.h"

#include "app_led_usb_status.h"

#include <stdint.h>

/** DECLARATIONS ***************************************************/

/** TYPE DEFINITIONS ************************************************/

/** VARIABLES ******************************************************/
/* Some processors have a limited range of RAM addresses where the USB module
 * is able to access.  The following section is for those devices.  This section
 * assigns the buffers that need to be used by the USB module into those
 * specific areas.
 */
#if defined(FIXED_ADDRESS_MEMORY)
    #if defined(__18CXX)
        #pragma udata VendorBasicDemo_IN_DATA=VENDOR_BASIC_DEMO_IN_DATA_BUFFER_ADDRESS
            //User application buffer for sending IN packets to the host
            unsigned char INPacket[USBGEN_EP_SIZE];

        #pragma udata VendorBasicDemo_OUT_DATA=VENDOR_BASIC_DEMO_OUT_DATA_BUFFER_ADDRESS
            //User application buffer for receiving and holding OUT packets sent from the host
            unsigned char OUTPacket[USBGEN_EP_SIZE];
        #pragma udata
    #elif defined(__XC8)
        //User application buffer for sending IN packets to the host
        unsigned char INPacket[USBGEN_EP_SIZE] VENDOR_BASIC_DEMO_IN_DATA_BUFFER_ADDRESS;

        //User application buffer for receiving and holding OUT packets sent from the host
        unsigned char OUTPacket[USBGEN_EP_SIZE] VENDOR_BASIC_DEMO_OUT_DATA_BUFFER_ADDRESS;
    #endif
#else
    //User application buffer for sending IN packets to the host
    unsigned char INPacket[USBGEN_EP_SIZE];

    //User application buffer for receiving and holding OUT packets sent from the host
    unsigned char OUTPacket[USBGEN_EP_SIZE];
#endif


static USB_HANDLE USBGenericOutHandle;  //USB handle.  Must be initialized to 0 at startup.
static USB_HANDLE USBGenericInHandle;   //USB handle.  Must be initialized to 0 at startup.

/*********************************************************************
* Function: void APP_DeviceVendorBasicDemoInitialize(void);
*
* Overview: Initializes the demo code
*
* PreCondition: None
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_DeviceVendorBasicDemoInitialize(void)
{  
    //initialize the variable holding the handle for the last
    // transmission
    USBGenericOutHandle = 0;
    USBGenericInHandle = 0;

    //Enable the application endpoints
    USBEnableEndpoint(USBGEN_EP_NUM,USB_OUT_ENABLED|USB_IN_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);

    //Arm the application OUT endpoint, so it can receive a packet from the host
    USBGenericOutHandle = USBGenRead(USBGEN_EP_NUM,(uint8_t*)&OUTPacket,USBGEN_EP_SIZE);
}//end UserInit

/*********************************************************************
* Function: void APP_DeviceVendorBasicDemoTasks(void);
*
* Overview: Keeps the demo running.
*
* PreCondition: The demo should have been initialized and started via
*   the APP_DeviceVendorBasicDemoInitialize() and APP_DeviceVendorBasicDemoStart() demos
*   respectively.
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_DeviceVendorBasicDemoTasks(void)
{  
    /* If the USB device isn't configured yet, we can't really do anything
     * else since we don't have a host to talk to.  So jump back to the
     * top of the while loop. */
    if( USBGetDeviceState() < CONFIGURED_STATE )
    {
        return;
    }

    /* If we are currently suspended, then we need to see if we need to
     * issue a remote wakeup.  In either case, we shouldn't process any
     * keyboard commands since we aren't currently communicating to the host
     * thus just continue back to the start of the while loop. */
    if( USBIsDeviceSuspended()== true )
    {
        return;
    }

    //As the device completes the enumeration process, the USBCBInitEP() function will
    //get called.  In this function, we initialize the user application endpoints (in this
    //example code, the user application makes use of endpoint 1 IN and endpoint 1 OUT).
    //The USBGenRead() function call in the USBCBInitEP() function initializes endpoint 1 OUT
    //and "arms" it so that it can receive a packet of data from the host.  Once the endpoint
    //has been armed, the host can then send data to it (assuming some kind of application software
    //is running on the host, and the application software tries to send data to the USB device).

    //If the host sends a packet of data to the endpoint 1 OUT buffer, the hardware of the SIE will
    //automatically receive it and store the data at the memory location pointed to when we called
    //USBGenRead().  Additionally, the endpoint handle (in this case USBGenericOutHandle) will indicate
    //that the endpoint is no longer busy.  At this point, it is safe for this firmware to begin reading
    //from the endpoint buffer, and processing the data.  In this example, we have implemented a few very
    //simple commands.  For example, if the host sends a packet of data to the endpoint 1 OUT buffer, with the
    //first byte = 0x80, this is being used as a command to indicate that the firmware should "Toggle LED(s)".
    if(!USBHandleBusy(USBGenericOutHandle))		//Check if the endpoint has received any data from the host.
    {
        switch(OUTPacket[0])					//Data arrived, check what kind of command might be in the packet of data.
        {
            case 0x80:  //Toggle LED(s) command from PC application.
                LED2_Toggle();
                break;
            case 0x81:  //Get push button state command from PC application.
				//Now check to make sure no previous attempts to send data to the host are still pending.  If any attempts are still
				//pending, we do not want to write to the endpoint 1 IN buffer again, until the previous transaction is complete.
				//Otherwise the unsent data waiting in the buffer will get overwritten and will result in unexpected behavior.
                if(!USBHandleBusy(USBGenericInHandle))
                {
                    //The endpoint was not "busy", therefore it is safe to write to the buffer and arm the endpoint.
                    INPacket[0] = 0x81;				//Echo back to the host PC the command we are fulfilling in the first byte.  In this case, the Get Pushbutton State command.
                    if(BUTTON_IsPressed(BUTTON_DEVICE_VENDOR_BASIC_DEMO) == false)					//pushbutton not pressed, pull up resistor on circuit board is pulling the PORT pin high
                    {
                        INPacket[1] = 0x01;
                    }
                    else
                    {
                        INPacket[1] = 0x00;
                    }
                    
                    //The USBGenWrite() function call "arms" the endpoint (and makes the handle indicate the endpoint is busy).
                    //Once armed, the data will be automatically sent to the host (in hardware by the SIE) the next time the
                    //host polls the endpoint.  Once the data is successfully sent, the handle (in this case USBGenericInHandle)
                    //will indicate the the endpoint is no longer busy.
                    USBGenericInHandle = USBGenWrite(USBGEN_EP_NUM,(uint8_t*)&INPacket,USBGEN_EP_SIZE);
                }
                break;
        }

        //Re-arm the OUT endpoint for the next packet:
        //The USBGenRead() function call "arms" the endpoint (and makes it "busy").  If the endpoint is armed, the SIE will
        //automatically accept data from the host, if the host tries to send a packet of data to the endpoint.  Once a data
        //packet addressed to this endpoint is received from the host, the endpoint will no longer be busy, and the application
        //can read the data which will be sitting in the buffer.
        USBGenericOutHandle = USBGenRead(USBGEN_EP_NUM,(uint8_t*)&OUTPacket,USBGEN_EP_SIZE);
    }

}//end ProcessIO


