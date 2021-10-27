#include "o3.h"
#include "gpio.h"
#include "systick.h"

int s;
int m;
int h;

// states
#define SEC_STATE 0
#define MIN_STATE 1
#define HOUR_STATE 2
#define COUNTDOWN_STATE 3
#define ALARM_STATE 4

int state = SEC_STATE;

// addresses
#define LED_PORT GPIO_PORT_E
#define LED_PIN 2
#define BUTTON_PORT GPIO_PORT_B
#define PB0_PIN 9
#define PB1_PIN 10


/**************************************************************************//**
 * @brief Konverterer nummer til string 
 * Konverterer et nummer mellom 0 og 99 til string
 *****************************************************************************/
void int_to_string(char *timestamp, unsigned int offset, int i) {
    if (i > 99) {
        timestamp[offset]   = '9';
        timestamp[offset+1] = '9';
        return;
    }

    while (i > 0) {
	    if (i >= 10) {
		    i -= 10;
		    timestamp[offset]++;
		
	    } else {
		    timestamp[offset+1] = '0' + i;
		    i=0;
	    }
    }
}

/**************************************************************************//**
 * @brief Konverterer 3 tall til en timestamp-string
 * timestamp-argumentet må være et array med plass til (minst) 7 elementer.
 * Det kan deklareres i funksjonen som kaller som "char timestamp[7];"
 * Kallet blir dermed:
 * char timestamp[7];
 * time_to_string(timestamp, h, m, s);
 *****************************************************************************/
void time_to_string(char *timestamp, int h, int m, int s) {
    timestamp[0] = '0';
    timestamp[1] = '0';
    timestamp[2] = '0';
    timestamp[3] = '0';
    timestamp[4] = '0';
    timestamp[5] = '0';
    timestamp[6] = '\0';

    int_to_string(timestamp, 0, h);
    int_to_string(timestamp, 2, m);
    int_to_string(timestamp, 4, s);
}

typedef struct {
	volatile word CTRL;
	volatile word MODEL;
	volatile word MODEH;
	volatile word DOUT;
	volatile word DOUTSET;
	volatile word DOUTCLR;
	volatile word DOUTTGL;
	volatile word DIN;
	volatile word PINLOCKN;
} gpio_port_map_t;

typedef struct {
	volatile gpio_port_map_t ports[6];
	volatile word unused_space[10];
	volatile word EXTIPSELL;
	volatile word EXTIPSELH;
	volatile word EXTIRISE;
	volatile word EXTIFALL;
	volatile word IEN;
	volatile word IF;
	volatile word IFS;
	volatile word IFC;
	volatile word ROUTE;
	volatile word INSENSE;
	volatile word LOCK;
	volatile word CTRL;
	volatile word CMD;
	volatile word EM4WUEN;
	volatile word EM4WUPOL;
	volatile word EM4WUCAUSE;
} gpio_map_t;

typedef struct {
	volatile word CTRL;
	volatile word LOAD;
	volatile word VAL;
	volatile word CALIB;
} gpio_systick_map_t;


volatile gpio_systick_map_t* systick = (gpio_systick_map_t*) SYSTICK_BASE;
volatile gpio_map_t* gpio = (gpio_map_t*) GPIO_BASE;


void update_display() {
	char timestamp[7];
	time_to_string(timestamp, h, m, s);
	lcd_write(timestamp);
}

void set_led(int value) {
	if (value) {
		// on
		gpio->ports[LED_PORT].DOUTSET |= 1 << LED_PIN;

	}
	else {
		// off
		gpio->ports[LED_PORT].DOUTCLR |= 1 << LED_PIN;
	}
}

void set_clock (int value) {
	if (value) {
		// start clock
		systick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	}
	else {
		// stop clock
		systick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	}
}


//PB0: increment current unit
void GPIO_ODD_IRQHandler(void) {

	switch (state) {
		case SEC_STATE:
			s++;
			if (s>60) {s = 60;}
			update_display();
			break;

		case MIN_STATE:
			m++;
			if (m>60) {m = 60;}
			update_display();
			break;

		case HOUR_STATE:
			h++;
			if (h>60) {h = 60;}
			update_display();
			break;
	};


	gpio->IFC |= 1 << PB0_PIN; // clear interrupt flag

}

//PB1: change state
void GPIO_EVEN_IRQHandler(void) {

	switch (state) {
		case SEC_STATE:
			state = MIN_STATE;
			break;

		case MIN_STATE:
			state = HOUR_STATE;
			break;

		case HOUR_STATE:
			state = COUNTDOWN_STATE;
			// start clock
			set_clock(1);
			break;

		case COUNTDOWN_STATE: break;

		case ALARM_STATE:
			state = SEC_STATE;
			//clear pin
			set_led(0);
			break;
	};

	gpio->IFC |= 1 << PB1_PIN; // clear interrupt flag
}


int countdown() {

	s--;
	if (!(h | m | s)) {
		update_display();
		return 1;
	}
	if (s < 0) {
		m--;
		if (m < 0) {
			h--;
			if (h < 0){
				h = m = s = 0;
			}
			else {
				m = 59;
				s = 59;
			}
		}
		else {
			s = 59;
		}
	}

	update_display();
	return 0;
}

void SysTick_Handler() {
	if (state == COUNTDOWN_STATE) {
		if (countdown()) {
			state = ALARM_STATE;
			// stop clock, assert light high
			set_clock(0);
			set_led(1);
		}
	}
}


int main(void) {
    init();
    // Skriv din kode her...


    // LED
    gpio->ports[LED_PORT].MODEH &= ~(0b1111 << 8); // reset
	gpio->ports[LED_PORT].MODEH |= LED_PORT << 8;

    // SysTick interrupts. LOAD, VAL, CTRL
	systick->LOAD = FREQUENCY;
	systick->VAL = 0;
	systick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk; // SysTick_CTRL_ENABLE_Msk


    // Button interrupts
	gpio->ports[BUTTON_PORT].DOUT = 0;


	// PB0 - port B, pin 9
	// MODEH
	gpio->ports[BUTTON_PORT].MODEH &= ~(0b1111 << 4); // reset
	gpio->ports[BUTTON_PORT].MODEH |= BUTTON_PORT << 4;
	// EXTIPSELH
	gpio->EXTIPSELH &= ~(0b1111 << 4); // reset
	gpio->EXTIPSELH |= BUTTON_PORT << 4;
	// EXTIFALL
	gpio->EXTIFALL |= 1 << PB0_PIN;
	// IF
	gpio->IFC |= 1 << PB0_PIN; // clear interrupt flag
	// IE
	gpio->IEN |= 1 << PB0_PIN;


	// PB1 - port B, pin 10
	// MODEH
	gpio->ports[BUTTON_PORT].MODEH &= ~(0b1111 << 8); // reset
	gpio->ports[BUTTON_PORT].MODEH |= BUTTON_PORT << 8;
	// EXTIPSELH
	gpio->EXTIPSELH &= ~(0b1111 << 8); // reset
	gpio->EXTIPSELH |= BUTTON_PORT << 8;
	// EXTIFALL
	gpio->EXTIFALL |= 1 << PB1_PIN;
	// IF
	gpio->IFC |= 1 << PB1_PIN; // clear interrupt flag
	// IE
	gpio->IEN |= 1 << PB1_PIN;




    update_display();

    while (1) {
    	// wait for interrupt
    }
    return 0;
}

