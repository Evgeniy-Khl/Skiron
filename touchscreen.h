// Calibrates value
#define SENSIBILITY 300
#define MINPRESSURE 10
#define MAXPRESSURE 1000

#define NUMSAMPLES  2
#define LOW         0
#define HIGH        1
#define INPUT       0
#define OUTPUT      1

//These are the pins for the shield!
#define XP_PORT		PORTD.6  //#define XP 6
#define XM_PORT		PORTC.2  //#define XM A2
#define YP_PORT		PORTC.1  //#define YP A1
#define YM_PORT		PORTD.7  //#define YM 7

#define XP_DIR		DDRD.6  //#define XP 6
#define XM_DIR		DDRC.2  //#define XM A2
#define YP_DIR		DDRC.1  //#define YP A1
#define YM_DIR		DDRD.7  //#define YM 7 
 
#define YP_PIN		PINC.1  //#define YP A1  
#define XM_PIN		PINC.2  //#define XM A2 