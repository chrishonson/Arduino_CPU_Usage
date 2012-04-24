#include <avr/io.h>
#include <avr/interrupt.h>
// #include <stdint.h>
// #include <inttypes.h>
    // #include <stdint.h>

#define LEDPIN 13
// typedef unsigned long int uint32_t;
// typedef unsigned int U16;
// typedef unsigned char  uint8_t;
// #define unsigned long int uint32_t
// #define unsigned int U16
// #define unsigned char  uint8_t
    typedef uint8_t   U8;
    // typedef int8_t    S8;
    // typedef char      C8;   //lint !e971 : "char" used directly in this context 

    // typedef uint16_t  U16;
    // typedef int16_t   S16;

    // typedef uint32_t  uint32_t;
    // typedef int32_t   S32;
uint32_t Idle_Counter;
U8 CPU_Utilization_Info_Read_To_Compute;
uint32_t Percent_CPU_Utilization;
uint32_t Prev_Idle_Counter;
uint32_t Idle_Counts;
void initTimer2(void)
{
  //8 bit timer. we want a 1ms compare match interrupt
  //16Mhz main clock. 16Mhz / 1024 = 15625hz = 64us
  //target interval = tick time * (timer counts + 1)
  //1ms = 64us * (x + 1)
  //(1ms / 64us) - 1 = x = 14.625
  //
  //16Mhz/256 = 62500hz = 0.000016s
  //(1ms / 16us) - 1 = x = 61.5
  //
  //16Mhz/128 = 125000hz = 0.000008s
  //(1ms/8us) - 1 = x = 124
  // set compare match register to desired timer count:
  OCR2A = 124;

  // turn on CTC(clear timer on compare match) mode:
  TCCR2B |= (1 << WGM22);

  // Set CS20 and CS22 bits for 128 prescaler:
  TCCR2B |= (1 << CS20);
  TCCR2B |= (1 << CS22);

  // enable timer compare match interrupt:
  TIMSK2 |= (1 << OCIE2A);
}
void setup()
{
  pinMode(LEDPIN, OUTPUT);

  // initialize the serial communication:
  Serial.begin(9600);

  noInterrupts();

  initTimer2();

  // enable global interrupts:
  interrupts();
}
void Signal_Idle(void)
{
  Idle_Counter++; 
}
uint32_t Read_Idle_Counts(void)
{
  uint32_t rv;
  noInterrupts();//reading 4bytes is not atomic on an 8bit arch
  rv = Idle_Counts;
  interrupts();
  return rv;
}
U8 One_MS_Task_Ready;
U8 Ten_MS_Task_Ready;
U8 Twenty_MS_Task_Ready;
U8 One_Hundred_MS_Task_Ready;
U8 One_S_Task_Ready;

void One_MS_Task(void)
{

}
void Ten_MS_Task(void)
{

}
void Twenty_MS_Task(void)
{

}
void One_Hundred_MS_Task(void)
{

}
void One_S_Task(void)
{
  // digitalWrite(LEDPIN, !digitalRead(LEDPIN));
  // Serial.print("Last Idle_Counts");
  // Serial.println(Calculate_Idle_Counts());

}
void Run_Tasks(void)
{
  if(One_MS_Task_Ready)
  {
    One_MS_Task();
  }
  if(Ten_MS_Task_Ready)
  {
    Ten_MS_Task();
  }
  if(Twenty_MS_Task_Ready)
  {
    Twenty_MS_Task();
  }
  if(One_Hundred_MS_Task_Ready)
  {
    One_Hundred_MS_Task();
  }
  if(One_S_Task_Ready)
  {
    One_S_Task();
  }
}
#define UNLOADED_IDLE_COUNTS 2
#define INTERVAL 100
uint16_t previousMillis;
void loop()
{
  Signal_Idle();
  Run_Tasks();
}
uint32_t Calculate_Idle_Counts (void)
{
  uint32_t temp_counts;
  Idle_Counts = Idle_Counter - Prev_Idle_Counter;
  Prev_Idle_Counter = Idle_Counter;
  // CPU_Utilization_Info_Read_To_Compute = 1;
  
  
    // CPU_Utilization_Info_Read_To_Compute = 0;
    Percent_CPU_Utilization = (100 * temp_counts) / UNLOADED_IDLE_COUNTS;
  return Idle_Counts;
}
/* WARNING this function called from ISR */
void Update_Task_Ready_Flags(void)
{
  static uint16_t counter;
  One_MS_Task_Ready=1;
  counter++;
  if(counter == 10)
  {
    Ten_MS_Task_Ready;
  }
  if(counter == 20)
  {
    Twenty_MS_Task_Ready;
  }
  if(counter == 100)
  {
    One_Hundred_MS_Task_Ready;
  }
  if(counter == 1000)
  {
    One_S_Task_Ready;
    counter=0;
  }  
}
ISR(TIMER2_COMPA_vect)//1ms isr
{
  Update_Task_Ready_Flags();//keep this isr short and sweet
}    
