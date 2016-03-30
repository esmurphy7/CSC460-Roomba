#include "LED_Test.h"
#include <avr/io.h>
#include "os.h"

#include "roomba.h"

void Task_P1()
{
    Roomba_Drive(100, 100);

    for(;;){};
}

void Idle() {
    for(;;) {}
}

void a_main()
{
    Roomba_Init();

    Task_Create(Task_P1, 1, 0);

    Task_Create(Idle, 10, 0);

    disable_LED(PORTL0);
    disable_LED(PORTL2);
    disable_LED(PORTL5);
    disable_LED(PORTL6);

    enable_LED(PORTL0);
    disable_LED(PORTL0);

    Task_Terminate();
}
