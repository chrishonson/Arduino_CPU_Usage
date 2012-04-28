#include <avr/io.h>
#include <avr/interrupt.h>

#define LEDPIN 13

uint32_t Idle_Counter;
uint8_t CPU_Utilization_Info_Read_To_Compute;
uint32_t Prev_Idle_Counter;
uint32_t Idle_Counts;
uint32_t Calculate_Idle_Counts (void);
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
  Serial.println("started");

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
#define UNLOADED_IDLE_COUNTS 348945
uint32_t Calculate_CPU_Utilization (uint32_t temp_counts)
{
  return 100 - ((100 * temp_counts) / UNLOADED_IDLE_COUNTS);
}
uint32_t Calculate_Idle_Counts (void)
{
  uint32_t temp_counts;
  Idle_Counts = Idle_Counter - Prev_Idle_Counter;
  Prev_Idle_Counter = Idle_Counter;
  return Idle_Counts;
}
uint8_t One_MS_Task_Ready;
uint8_t Ten_MS_Task_Ready;
uint8_t Twenty_MS_Task_Ready;
uint8_t One_Hundred_MS_Task_Ready;
uint8_t One_S_Task_Ready;
void One_MS_Task(void)
{

}
void Ten_MS_Task(void)
{

}
void One_Hundred_MS_Task(void)
{
  //10 * 50ms = 500ms delay
  Serial.println("delay(50);");
  delay(50);

}
void One_S_Task(void)
{
  uint32_t idleCounts = Calculate_Idle_Counts();
  digitalWrite(LEDPIN, !digitalRead(LEDPIN));
  Serial.print("Last Idle_Counts  ");
  Serial.println(idleCounts);
  Serial.print("CPU Utilization  ");
  Serial.print(Calculate_CPU_Utilization(idleCounts));
  Serial.println("%");

}
void Run_Tasks(void)
{
  if(One_MS_Task_Ready)
  {
    One_MS_Task_Ready=0;
    One_MS_Task();
  }
  if(Ten_MS_Task_Ready)
  {
    Ten_MS_Task_Ready=0;
    Ten_MS_Task();
  }
  if(One_Hundred_MS_Task_Ready)
  {
    One_Hundred_MS_Task_Ready=0;
    One_Hundred_MS_Task();
  }
  if(One_S_Task_Ready)
  {
    One_S_Task_Ready=0;
    One_S_Task();
  }
}
// #define INTERVAL 100
// uint16_t previousMillis;
void loop()
{
  Signal_Idle();
  Run_Tasks();
}
/* WARNING this function called from ISR */
void Update_Task_Ready_Flags(void)
{
  static uint16_t counter;
  One_MS_Task_Ready=1;
  counter++;
  if((counter%10)==0)
  {
    Ten_MS_Task_Ready=1;
  }
  if((counter%100)==0)
  {
    One_Hundred_MS_Task_Ready=1;;
  }
  if(counter == 1000)
  {
    One_S_Task_Ready=1;
    counter=0;
  }  
}
ISR(TIMER2_COMPA_vect)//1ms isr
{
  Update_Task_Ready_Flags();//keep this isr short and sweet
}    
