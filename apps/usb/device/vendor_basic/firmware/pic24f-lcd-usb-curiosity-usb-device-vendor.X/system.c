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

#include <xc.h>
#include "system.h"
#include "usb.h"
#include "led1.h"
#include "led2.h"
#include "buttons.h"


//------------------------------------------------------------------------------
//Example USB compatible oscillator settings.  Uncomment only one of the below.
//------------------------------------------------------------------------------
//#define USB_RUN_FROM_FRCPLL_WITH_ACTIVE_CLOCK_TUNING
#define USB_RUN_FROM_EXPLORER_16_8MHZ_CRYSTAL



/** CONFIGURATION Bits **********************************************/
// FSEC
#pragma config BWRP = OFF               // Boot Segment Write-Protect bit (Boot Segment may be written)
#pragma config BSS = DISABLED           // Boot Segment Code-Protect Level bits (No Protection (other than BWRP))
#pragma config BSEN = OFF               // Boot Segment Control bit (No Boot Segment)
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GSS = DISABLED           // General Segment Code-Protect Level bits (No Protection (other than GWRP))
#pragma config CWRP = OFF               // Configuration Segment Write-Protect bit (Configuration Segment may be written)
#pragma config CSS = DISABLED           // Configuration Segment Code-Protect Level bits (No Protection (other than CWRP))
#pragma config AIVTDIS = OFF            // Alternate Interrupt Vector Table bit (Disabled AIVT)

// FBSLIM
#pragma config BSLIM = 0x1FFF           // Boot Segment Flash Page Address Limit bits (Enter Hexadecimal value)

// FOSCSEL
#pragma config FNOSC = PRIPLL           // Oscillator Source Selection (Primary Oscillator with PLL module (XT + PLL, HS + PLL, EC + PLL))
#pragma config PLLMODE = PLL96DIV2      // PLL Mode Selection (96 MHz PLL. Oscillator input is divided by 2 (8 MHz input))
#pragma config IESO = OFF               // Two-speed Oscillator Start-up Enable bit (Start up with user-selected oscillator source)

// FOSC
#pragma config POSCMD = HS              // Primary Oscillator Mode Select bits (HS Crystal Oscillator mode)
#pragma config OSCIOFCN = ON            // OSC2 Pin Function bit (RC14 is general purpose digital I/O pin)
#pragma config SOSCEN = OFF             // SOSC Power Selection Configuration bits (SOSC crystal driver is disabled (RC13/RC14 mode))
#pragma config PLLSS = PRI              // PLL Secondary Selection Configuration bit (PLL is fed by the Primary oscillator)
#pragma config IOL1WAY = OFF            // Peripheral pin select configuration bit (Allow multiple reconfigurations)
#pragma config FCKSM = CSECMD           // Clock Switching Mode bits (Clock switching is enabled and Fail-safe Clock Monitor is disabled)

// FWDT
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler bits (1:32,768)
#pragma config FWPSA = PR128            // Watchdog Timer Prescaler bit (1:128)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bits (WDT and SWDTEN are disabled)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (WDT in non-windowed mode)
#pragma config WDTWIN = WIN25           // Watchdog Timer Window Select bits (WDT window is 25% of WDT period)
#pragma config WDTCMX = WDTCLK          // WDT MUX Source Select bits (WDT clock source is determined by the WDTCLK Configuration bits)
#pragma config WDTCLK = LPRC            // WDT Clock Source Select bits (WDT uses LPRC)

// FPOR
#pragma config BOREN = ON               // Brown Out Enable bit (Brown Out is enabled)
#pragma config LPREGEN = OFF            // Low power regulator control (Low power regulator is disabled)
#pragma config LPBOREN = ON             // Downside Voltage Protection Enable bit (Low power BOR is enabled when BOR is inactive)

// FICD
#pragma config ICS = PGD2               // ICD Communication Channel Select bits (Communicate on PGEC2 and PGED2)
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)
#pragma config BTSWP = OFF              // BOOTSWP Instruction Enable bit (BOOTSWP instruction is disabled)

// FDMTIVTL
#pragma config DMTIVTL = 0xFFFF         //  (Enter Hexadecimal value)

// FDMTIVTH
#pragma config DMTIVTH = 0xFFFF         //  (Enter Hexadecimal value)

// FDMTCNTL
#pragma config DMTCNTL = 0xFFFF         //  (Enter Hexadecimal value)

// FDMTCNTH
#pragma config DMTCNTH = 0xFFFF         //  (Enter Hexadecimal value)

// FDMT
#pragma config DMTDIS = OFF             //  (Dead Man Timer is disabled and can be enabled by software)

// FDEVOPT1
#pragma config ALTCMPI = OFF            // Alternate Comparator Input Enable bit (C2INC and C3INC are on RG9 and RD11)
#pragma config TMPRPIN = OFF            // Tamper Pin Enable bit (TMPRN pin function is disabled)
#pragma config SOSCHP = ON              // SOSC High Power Enable bit (valid only when SOSCSEL = 1 (SOSC high power mode (default))
#pragma config ALTI2C1 = OFF            // Alternate I2C pin Location (SDA1 and SCL1 on RA15 and RA14)
#pragma config SMB3EN = ON              // SMBus 3 Enable bit (SMBus 3.0 input levels)

/*********************************************************************
* Function: void SYSTEM_Initialize( SYSTEM_STATE state )
*
* Overview: Initializes the system.
*
* PreCondition: None
*
* Input:  SYSTEM_STATE - the state to initialize the system into
*
* Output: None
*
********************************************************************/
void SYSTEM_Initialize( SYSTEM_STATE state )
{
    switch(state)
    {
        case SYSTEM_STATE_USB_START:
            //Make sure that the general purpose output driver multiplexed with
            //the VBUS pin is always consistently configured to be tri-stated in
            //USB applications, so as to avoid any possible contention with the host.
            //(ex: maintain TRISFbits.TRISF7 = 1 at all times).
            TRISFbits.TRISF7 = 1;

            //Configure clock settings to be compatible with USB operation.  
            //This could involve doing nothing (ex: if the config bit settings
            //already result in a USB compatible clock frequency at bootup), or,
            //could involve clock switching and/or turning on the PLL and/or turning 
            //on the active clock tuning feature.
 
            #if defined(USB_RUN_FROM_EXPLORER_16_8MHZ_CRYSTAL)
                //Don't need to do anything, with the default config bit settings for this 
                //oscillator setting, since the settings will already be USB compatible.
            #else
                //Assume USB_RUN_FROM_FRCPLL_WITH_ACTIVE_CLOCK_TUNING is selected instead.
                CLKDIV = 0x3120;    //Set FRC to 4MHz output mode, and CPUDIV to 1:1 (which is a USB compatible setting)
               __builtin_write_OSCCONH(0x01);  //0x01 = FRCPLL with postscalar
               __builtin_write_OSCCONL(OSCCON | 0x0001);   //Set the OSWEN bit, to initiate clock switching operation.
               //Wait for PLL lock
               while(OSCCONbits.LOCK == 0);
               //Turn on the active clock tuning for USB operation
               OSCTUN = 0x9000;        //Enable active clock tuning from USB host reference
               //The oscillator settings should now be USB compatible.           
            #endif
            
            //Configure other USB or application related resources.
            BUTTON_Enable(BUTTON_DEVICE_VENDOR_BASIC_DEMO);
            break;
            
        case SYSTEM_STATE_USB_SUSPEND:
            //If developing a bus powered USB device that needs to be USB compliant,
            //insert code here to reduce the I/O pin and microcontroller power consumption,
            //so that the total current is <2.5mA from the USB host's VBUS supply.
            //If developing a self powered application (or a bus powered device where
            //official USB compliance isn't critical), nothing strictly needs
            //to be done during USB suspend.
            
            USBSleepOnSuspend();
            break;

        case SYSTEM_STATE_USB_RESUME:
            //If you didn't change any I/O pin states prior to entry into suspend,
            //then nothing explicitly needs to be done here.  However, by the time
            //the firmware returns from this function, the full application should
            //be restored into effectively exactly the same state as the application
            //was in, prior to entering USB suspend.
            
            //Additionally, before returning from this function, make sure the microcontroller's
            //currently active clock settings are compatible with USB operation, including
            //taking into consideration all possible microcontroller features/effects
            //that can affect the oscillator settings (such as internal/external 
            //switchover (two speed start up), fail-safe clock monitor, PLL lock time,
            //external crystal/resonator startup time (if using a crystal/resonator),
            //etc.

            //Additionally, the official USB specifications dictate that USB devices
            //must become fully operational and ready for new host requests/normal 
            //USB communication after a 10ms "resume recovery interval" has elapsed.
            //In order to meet this timing requirement and avoid possible issues,
            //make sure that all necessary oscillator startup is complete and this
            //function has returned within less than this 10ms interval.

            break;

        default:
            break;
    }
}

#if defined(USB_INTERRUPT)
void __attribute__((interrupt,auto_psv)) _USB1Interrupt()
{
    USBDeviceTasks();
}
#endif


        
