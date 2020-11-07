/*
 *  Author: Zdenìk Brhel
 *  Login: xbrhel04
 *  Last change: 30.12.2018
 *  Original
 */

/* Header file with all the essential definitions for a given type of MCU */
#include "MK60D10.h"

/* Pin defines */
#define SPEAKER 25 // P1(40)
#define SPEAKER_MASK (1<<SPEAKER)

#define KEYBOARD_ROW_1 8 // P1(23)
#define KEYBOARD_ROW_1_MASK (1 << KEYBOARD_ROW_1)
#define KEYBOARD_ROW_2 10 // P1(24)
#define KEYBOARD_ROW_2_MASK (1 << KEYBOARD_ROW_2)
#define KEYBOARD_ROW_3 6 // P1(25)
#define KEYBOARD_ROW_3_MASK (1 << KEYBOARD_ROW_3)
#define KEYBOARD_ROW_4 11 // P1(26)
#define KEYBOARD_ROW_4_MASK (1 << KEYBOARD_ROW_4)
#define KEYBOARD_COL_1 7 // P1(27)
#define KEYBOARD_COL_1_MASK (1 << KEYBOARD_COL_1)
#define KEYBOARD_COL_2 9 // P1(28)
#define KEYBOARD_COL_2_MASK (1 << KEYBOARD_COL_2)
#define KEYBOARD_COL_3 27 // P1(35)
#define KEYBOARD_COL_3_MASK (1 << KEYBOARD_COL_3)
#define KEYBOARD_COL_4 29 // P1(36)
#define KEYBOARD_COL_4_MASK (1 << KEYBOARD_COL_4)
#define KEYBOARD_ROW_MASK (KEYBOARD_ROW_1_MASK | KEYBOARD_ROW_2_MASK | KEYBOARD_ROW_3_MASK | KEYBOARD_ROW_4_MASK)
#define KEYBOARD_COL_MASK (KEYBOARD_COL_1_MASK | KEYBOARD_COL_2_MASK | KEYBOARD_COL_3_MASK | KEYBOARD_COL_4_MASK)

//Output for PORT A
#define PORT_A_OUTPUT (KEYBOARD_ROW_MASK | SPEAKER_MASK)

#define ROW_COUNT 4
#define COL_COUNT 4

/* Tones */
enum tone_e {
	c1 = 261,
	c1S = 277,
	d1 = 294,
	d1S = 311,
	e1 = 329,
	f1 = 349,
	f1S = 370,
	g1 = 391,
	g1S = 415,
	a1 = 440,
	a1S = 455,
	b1 = 466,
	h1 = 493,
	c2 = 523,
	c2S = 554,
	d2 = 587,
	d2S = 622,
	e2 = 659,
	f2 = 698,
	f2S = 740,
	g2 = 784,
	g2S = 830,
	a2 = 880
} Tone;
/* Global variables */
enum state_e {
	STATE_IDLE,
	STATE_PUSHED,
	STATE_SONG
} State;


/* A delay function */
void delay(unsigned long long bound) {
  for(unsigned long long i=0;i<bound;i++);
}

/* Initialize the MCU - basic clock settings, turning the watchdog off */
void MCUInit(void)  {
    MCG_C4 |= ( MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x01) );
    SIM_CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(0x00);
    WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;
}
/* Initialize ports - sets activity, enabling clock for PORT A, setting output ports */
void PortsInit(void)
{
    // Enable clock for PORT-A
    SIM->SCGC5 = SIM_SCGC5_PORTA_MASK;

    // Setting ports - speaker
    PORTA->PCR[SPEAKER] = PORT_PCR_MUX( 0x01 );

    // Setting ports - keyboard
    PORTA->PCR[KEYBOARD_ROW_1] = PORT_PCR_MUX( 0x01 );
    PORTA->PCR[KEYBOARD_ROW_2] = PORT_PCR_MUX( 0x01 );
    PORTA->PCR[KEYBOARD_ROW_3] = PORT_PCR_MUX( 0x01 );
    PORTA->PCR[KEYBOARD_ROW_4] = PORT_PCR_MUX( 0x01 );
    PORTA->PCR[KEYBOARD_COL_1] = PORT_PCR_MUX( 0x01 );
    PORTA->PCR[KEYBOARD_COL_2] = PORT_PCR_MUX( 0x01 );
    PORTA->PCR[KEYBOARD_COL_3] = PORT_PCR_MUX( 0x01 );
    PORTA->PCR[KEYBOARD_COL_4] = PORT_PCR_MUX( 0x01 );

    // Output settings
    PTA->PDDR = GPIO_PDDR_PDD(PORT_A_OUTPUT);
}

void LPTMR0Init(int count)
{
    SIM_SCGC5 |= SIM_SCGC5_LPTIMER_MASK;

    LPTMR0_CSR &= ~LPTMR_CSR_TEN_MASK;     // Turn OFF LPTMR to perform setup

    LPTMR0_PSR = ( LPTMR_PSR_PRESCALE(0) // 0000 is div 2
                 | LPTMR_PSR_PBYP_MASK  // LPO feeds directly to LPT
                 | LPTMR_PSR_PCS(1)) ; // use the choice of clock

    LPTMR0_CMR = count;  // Set compare value

    LPTMR0_CSR =(  LPTMR_CSR_TCF_MASK   // Clear any pending interrupt (now)
                 | LPTMR_CSR_TIE_MASK   // LPT interrupt enabled
                );

    NVIC_EnableIRQ(LPTMR0_IRQn);

    LPTMR0_CSR |= LPTMR_CSR_TEN_MASK;   // Turn ON LPTMR0 and start counting
}

int scanKeyboard(void)
{
	static int rows[] = { KEYBOARD_ROW_1_MASK, KEYBOARD_ROW_2_MASK, KEYBOARD_ROW_3_MASK, KEYBOARD_ROW_4_MASK };
	static int columns[] = { KEYBOARD_COL_1_MASK, KEYBOARD_COL_2_MASK, KEYBOARD_COL_3_MASK, KEYBOARD_COL_4_MASK };

	GPIOA_PDOR |= KEYBOARD_ROW_MASK; //Set all rows to 1

	for(int row = 0; row < ROW_COUNT; row++)
	{
		GPIOA_PDOR &= ~rows[row]; // take one and set it to 0
		delay(100);
		for(int col = 0; col < COL_COUNT; col++)
		{
			//int res = (GPIOA_PDIR & columns[col]);
			//!(res >=1)
			if(!(GPIOA_PDIR & columns[col]))
			{
    			int val = ((row+1)*10)+(col+1); //get number of button (11 - 44), where first number is number of row and second number of col
    			GPIOA_PDOR |= KEYBOARD_ROW_MASK; //set row back to 1
    			delay(1000);
    			return val;
			}
		}
		GPIOA_PDOR |= KEYBOARD_ROW_MASK; //set row back to 1
	}
	return 0;
}

void PlayNote(enum tone_e note, int duration)
{
	long dl = (long)(10000/note);  // Semiperiod
	long time = (long)((duration*100)/(dl*2));  // Lenght on note
	for (int i=0;i<time;i++)
	{
    	GPIOA_PDOR ^= SPEAKER_MASK;
		delay(dl*55);   //
	}
	delay(800); //Add a little delay to separate the single notes
}

void PlaySong1() //Imperial march
{
    PlayNote(a1, 500);
    PlayNote(a1, 500);
    PlayNote(a1, 500);
    PlayNote(f1, 350);
    PlayNote(c2, 150);
    PlayNote(a1, 500);
    PlayNote(f1, 350);
    PlayNote(c2, 150);
    PlayNote(a1, 650);

    delay(3000);

    PlayNote(e2, 500);
    PlayNote(e2, 500);
    PlayNote(e2, 500);
    PlayNote(f2, 350);
    PlayNote(c2, 150);
    PlayNote(g1S, 500);
    PlayNote(f1, 350);
    PlayNote(c2, 150);
    PlayNote(a1, 650);

    delay(3000);

    PlayNote(a1, 500);
    PlayNote(a1, 300);
    PlayNote(a1, 150);
    PlayNote(a2, 400);
    PlayNote(g2S, 200);
    PlayNote(g2, 200);
    PlayNote(f2S, 125);
    PlayNote(f2, 125);
    PlayNote(f2S, 250);

    delay(3000);

    PlayNote(a1S, 250);
    PlayNote(d2S, 400);
    PlayNote(d2, 200);
    PlayNote(c2S, 200);
    PlayNote(c2, 125);
    PlayNote(h1, 125);
    PlayNote(c2, 250);

    delay(3000);

    PlayNote(f1, 125);
    PlayNote(g1S, 500);
    PlayNote(f1, 375);
    PlayNote(a1, 125);
    PlayNote(c2, 500);
    PlayNote(a1, 375);
    PlayNote(c2, 125);
    PlayNote(e2, 650);

    PlayNote(a2, 500);
    PlayNote(a1, 300);
    PlayNote(a1, 150);
    PlayNote(a2, 400);
    PlayNote(g2S, 200);
    PlayNote(g2, 200);
    PlayNote(f2S, 125);
    PlayNote(f2, 125);
    PlayNote(f2S, 250);

    delay(3000);

    PlayNote(a1S, 250);
    PlayNote(d2S, 400);
    PlayNote(d2, 200);
    PlayNote(c2S, 200);
    PlayNote(c2, 125);
    PlayNote(h1, 125);
    PlayNote(c2, 250);

    delay(3000);

    PlayNote(f1, 250);
    PlayNote(g1S, 500);
    PlayNote(f1, 375);
    PlayNote(c2, 125);
    PlayNote(a1, 500);
    PlayNote(f1, 375);
    PlayNote(c2, 125);
    PlayNote(a1, 650);
}

void PlaySong2() //Concering hobbits
{
	PlayNote(d1, 250);
	PlayNote(e1, 250);

	PlayNote(f1S,500);
	PlayNote(a1, 500);
	PlayNote(f1S,500);
	PlayNote(e1, 250);

	PlayNote(d1, 875);
	delay(25);
	PlayNote(d1, 250);
	PlayNote(f1S, 250);
	PlayNote(a1, 250);

	PlayNote(h1, 750);
	PlayNote(c2S,125);
	PlayNote(d2, 125);
	PlayNote(c2S, 500);
	PlayNote(a1, 500);

	PlayNote(f1S, 750);
	PlayNote(g1, 125);
	PlayNote(f1S, 125);
	PlayNote(e1, 750);
	PlayNote(d1, 125);
	PlayNote(e1, 125);

	PlayNote(f1S, 500);
	PlayNote(a1, 500);
	PlayNote(f1S, 500);
	PlayNote(e1, 500);

	PlayNote(d1, 875);
	delay(25);
	PlayNote(d1, 250);
	PlayNote(f1S, 250);
	PlayNote(a1, 250);

	PlayNote(h1, 950);
	PlayNote(c2S, 63);
	PlayNote(h1, 63);
	PlayNote(a1, 500);
	PlayNote(f1S, 500);

	PlayNote(f1S, 750);
	PlayNote(g1, 125);
	PlayNote(f1S, 125);

	PlayNote(e1, 750);
	PlayNote(f1S, 125);
	PlayNote(e1, 125);

	PlayNote(d1, 200);
	delay(1000000);
	PlayNote(d1, 200);
}

void setTone(int key)
{
	if(key != 0 && State != STATE_SONG)
		State = STATE_PUSHED;
	switch(key)
	{
	case 11: // 1
		Tone = c1;
		break;
	case 12: // 2
		Tone = c1S;
		break;
	case 13: // 3
		Tone = d1;
		break;
	case 14: // A
		Tone = d1S;
		break;
	case 21: // 4
		Tone = e1;
		break;
	case 22: // 5
		Tone = f1;
		break;
	case 23: // 6
		Tone = f1S;
		break;
	case 24: // B
		Tone = g1;
		break;
	case 31: // 7
		Tone = g1S;
		break;
	case 32: // 8
		Tone = a1;
		break;
	case 33: // 9
		Tone = a1S;
		break;
	case 34: // C
		Tone = h1;
		break;
	case 41: // *
		Tone = c2;
		break;
	case 42: // 0
		//?? Not enough notes in scale :(
		break;
	case 43: // #
		State = STATE_SONG;
		PlaySong2();
		State = STATE_IDLE;
		break;
		break;
	case 44: // D
		State = STATE_SONG;
		PlaySong1();
		State = STATE_IDLE;
		break;
	default:
		State = STATE_IDLE;
		break;
	}
}

void LPTMR0_IRQHandler(void)
{
    //LPTMR0_CMR = Tone;  // Set compare value
	if(State != STATE_IDLE && State != STATE_SONG)
		PlayNote(Tone, 100);
	LPTMR0_CSR |=  LPTMR_CSR_TCF_MASK; // writing 1 to TCF tclear the flag
}

int main(void)
{
    // Default values
    Tone = a1;
    State = STATE_IDLE;

	// Init modules
    MCUInit();
    PortsInit();
    LPTMR0Init(1);

    int key;

    while (1) { // Repeative scanning of keyboard
    	key = scanKeyboard();
    	setTone(key);
    }

    return 0;
}
