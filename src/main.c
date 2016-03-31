#include "LED_Test.h"
#include <avr/io.h>
#include <util/delay.h>
#include "os.h"

#include "roomba.h"

void Task_P1()
{
    for(;;){
        Roomba_Drive(-150, 32768);
        _delay_ms(10000);
    }

    for(;;){};
}


void Task_P2()
{
//    Roomba_Drive(100, 100);

    for(;;){};
}

void Idle() {
    for(;;) {}
}

void a_main()
{
    enable_LED(PORTL0);
    disable_LED(PORTL0);


    Roomba_Init();

    Task_Create(Task_P1, 1, 0);
    Task_Create(Task_P2, 2, 0);

    Task_Create(Idle, 10, 0);

    disable_LED(PORTL0);
    disable_LED(PORTL2);
    disable_LED(PORTL5);
    disable_LED(PORTL6);

    Task_Terminate();
}
