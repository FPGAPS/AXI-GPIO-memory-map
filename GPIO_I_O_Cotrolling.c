// include header files
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "sleep.h"
#include "xil_io.h"

//-------------------------------------------------
// AXI GPIOs addresses here: You can also find them
#define AXI_GPIO_0 XPAR_AXI_GPIO_0_BASEADDR //0xA0000000 LED control
#define AXI_GPIO_1 XPAR_AXI_GPIO_1_BASEADDR //0xA0010000 DIP switch
#define AXI_GPIO_2 XPAR_AXI_GPIO_2_BASEADDR //0xA0020000 First  input for Adder
#define AXI_GPIO_3 XPAR_AXI_GPIO_3_BASEADDR //0xA0030000 Second input for Adder
#define AXI_GPIO_4 XPAR_AXI_GPIO_4_BASEADDR //0xA0040000 Adder's output

//------------------ main ---------------------
int main() {
	init_platform();

	xil_printf("\r\n**********************************\r\n");
	xil_printf("*              Adder in PL        *\r\n");
	xil_printf("**********************************\r\n");

//------------------ First practice the arithmetic in PL side!----------------
	volatile u32* GPIO_2_address = (u32*) XPAR_AXI_GPIO_2_BASEADDR;
	volatile u32* GPIO_3_address = (u32*) XPAR_AXI_GPIO_3_BASEADDR;
	volatile u32* GPIO_4_address = (u32*) XPAR_AXI_GPIO_4_BASEADDR;
	u32 a, b, c;
	for (a = 1, b = 2; a < 6; a = a + 2, b = b + 3) {
		*GPIO_2_address = a;
		*GPIO_3_address = b;
		c = *GPIO_4_address;
		xil_printf("Arithmetic Calculation in PL: %d + %d = %d \r\n", a, b, c);
		xil_printf("**********************************\r\n");

	}
	xil_printf("Please press Enter to continue\r\n");
	getchar();
	xil_printf("\r\n**********************************\r\n");
	xil_printf("*          I/O Controlling       *\r\n");
	xil_printf("**********************************\r\n");

//---------------------- I/O controlling ------------
	u8 init_shift_value = 0b0001;
	u8 LED_Shiffer_Value = (u8) (init_shift_value & 0b1111);

	u8 shift_direction = 1; // 1 for right, 0 for left
	u32 usecond_sleep = (u32) 125000; // delay in the loop in Microseconds
	u8 stop_shift = 0; // will stop the shift if this is 1
	u8 off_leds = 0; // turn off all LEDs if this value is 1
	while (1) {
		// write the value to the user LEDs
		if (off_leds == 0) {
			Xil_Out32(AXI_GPIO_0, LED_Shiffer_Value);
		} else {
			Xil_Out32(AXI_GPIO_0, 0);
		}
		// delay in the loop Microseconds
		usleep(usecond_sleep);

		//---------- read the user dip switch in poled mode
		u8 dip_switch_value = (u8) (Xil_In32(AXI_GPIO_1) & 0b1111);
		shift_direction = (u8) (dip_switch_value & 0b0001); // first bit to control the direction
		usecond_sleep = (u32) 125000
				* ((dip_switch_value & 0b0010) == 0 ? 1 : 2); // second bit to control the speed
		stop_shift = (u8) (dip_switch_value & 0b0100); // third bit to stop or run
		off_leds = (u8) (dip_switch_value & 0b1000); // forth bit to turn off or on

		//-------------Shift the LEDs based on the selected direction-----------------------
		if (stop_shift == 0) {
			if (shift_direction == 0) { //left shift
				LED_Shiffer_Value = (u8) (LED_Shiffer_Value << 1 & 0b1111);
				init_shift_value = 0b0001;
			} else { // right shift
				LED_Shiffer_Value = (u8) (LED_Shiffer_Value >> 1 & 0b1111);
				init_shift_value = 0b1000;
			}
		}
		if (LED_Shiffer_Value == 0) {
			LED_Shiffer_Value = (u8) (init_shift_value & 0b1111);
		}

	}
//----------------- I/O controlling---------------------------
	print("Successfully finish the test!");
	cleanup_platform();
	return 0;
}
