#include <string.h>
#include <avr/interrupt.h>
#include "os.h"

/**
 * \file active.c
 * \brief A Skeleton Implementation of an RTOS
 * 
 * \mainpage A Skeleton Implementation of a "Full-Served" RTOS Model
 * This is an example of how to implement context-switching based on a 
 * full-served model. That is, the RTOS is implemented by an independent
 * "kernel" task, which has its own stack and calls the appropriate kernel 
 * function on behalf of the user task.
 *
 * \author Dr. Mantis Cheng
 * \date 29 September 2006
 *
 * ChangeLog: Modified by Alexander M. Hoole, October 2006.
 *			  -Rectified errors and enabled context switching.
 *			  -LED Testing code added for development (remove later).
 *
 * \section Implementation Note
 * This example uses the ATMEL AT90USB1287 instruction set as an example
 * for implementing the context switching mechanism. 
 * This code is ready to be loaded onto an AT90USBKey.  Once loaded the 
 * RTOS scheduling code will alternate lighting of the GREEN LED light on
 * LED D2 and D5 whenever the correspoing PING and PONG tasks are running.
 * (See the file "cswitch.S" for details.)
 */

//Comment out the following line to remove debugging code from compiled version.
#define DEBUG

typedef void (*voidfuncptr) (void);      /* pointer to void f(void) */

#define WORKSPACE     256

/*===========
  * RTOS Internal
  *===========
  */

/**
  * This internal kernel function is the context switching mechanism.
  * It is done in a "funny" way in that it consists two halves: the top half
  * is called "Exit_Kernel()", and the bottom half is called "Enter_Kernel()".
  * When kernel calls this function, it starts the top half (i.e., exit). Right in
  * the middle, "Cp" is activated; as a result, Cp is running and the kernel is
  * suspended in the middle of this function. When Cp makes a system call,
  * it enters the kernel via the Enter_Kernel() software interrupt into
  * the middle of this function, where the kernel was suspended.
  * After executing the bottom half, the context of Cp is saved and the context
  * of the kernel is restore. Hence, when this function returns, kernel is active
  * again, but Cp is not running any more. 
  * (See file "switch.S" for details.)
  */
extern void CSwitch();
extern void Exit_Kernel();    /* this is the same as CSwitch() */

/* Prototype */
void Task_Terminate(void);

/** 
  * This external function could be implemented in two ways:
  *  1) as an external function call, which is called by Kernel API call stubs;
  *  2) as an inline macro which maps the call into a "software interrupt";
  *       as for the AVR processor, we could use the external interrupt feature,
  *       i.e., INT0 pin.
  *  Note: Interrupts are assumed to be disabled upon calling Enter_Kernel().
  *     This is the case if it is implemented by software interrupt. However,
  *     as an external function call, it must be done explicitly. When Enter_Kernel()
  *     returns, then interrupts will be re-enabled by Enter_Kernel().
  */
extern void Enter_Kernel();

#define Disable_Interrupt()		asm volatile ("cli"::)
#define Enable_Interrupt()		asm volatile ("sei"::)


/**
  *  This is the set of states that a task can be in at any given time.
  */
typedef enum process_states
{
    DEAD = 0,
    READY,
    RUNNING
} PROCESS_STATES;

/**
  * This is the set of kernel requests, i.e., a request code for each system call.
  */
typedef enum kernel_request_type
{
    NONE = 0,
    CREATE,
    NEXT,
    TERMINATE
} KERNEL_REQUEST_TYPE;

/**
  * Each task is represented by a process descriptor, which contains all
  * relevant information about this task. For convenience, we also store
  * the task's stack, i.e., its workspace, in here.
  */
typedef struct ProcessDescriptor
{
    volatile unsigned char *sp;   /* stack pointer into the "workSpace" */
    unsigned char workSpace[WORKSPACE];
    PROCESS_STATES state;
    voidfuncptr  code;   /* function to be executed as a task */
    KERNEL_REQUEST_TYPE request;

    int name;
    int arg;
    int level;
    struct ProcessDescriptor* next;
} PD;

/**
 * @brief Contains pointers to head and tail of a linked list.
 */
typedef struct
{
    /** The first item in the queue. NULL if the queue is empty. */
    volatile PD*  head;
    /** The last item in the queue. Undefined if the queue is empty. */
    volatile PD*  tail;
} Q;

EVENT * Event_Init(void);

/**
  * This table contains ALL process descriptors. It doesn't matter what
  * state a task is in.
  */
static PD Process[MAXPROCESS + 1];

static PD* idle_task = &Process[MAXPROCESS];

/**
  * The process descriptor of the currently RUNNING task.
  */
volatile static PD* Cp;

/** 
  * Since this is a "full-served" model, the kernel is executing using its own
  * stack. We can allocate a new workspace for this kernel stack, or we can
  * use the stack of the "main()" function, i.e., the initial C runtime stack.
  * (Note: This and the following stack pointers are used primarily by the
  *   context switching code, i.e., CSwitch(), which is written in assembly
  *   language.)
  */
volatile unsigned char *KernelSp;

/**
  * This is a "shadow" copy of the stack pointer of "Cp", the currently
  * running task. During context switching, we need to save and restore
  * it into the appropriate process descriptor.
  */
volatile unsigned char *CurrentSp;

/** index to next task to run */
volatile static unsigned int NextP;

/** 1 if kernel has been started; 0 otherwise. */
volatile static unsigned int KernelActive;

/** number of tasks created so far */
volatile static unsigned int Tasks;

static int kernel_request_retval;

static uint8_t current_periodic_finished = 0;

/** The ready queue for RR tasks. Their scheduling is round-robin. */
static Q rr_queue;

/** The ready queue for SYSTEM tasks. Their scheduling is first come, first served. */
static Q system_queue;

/** An array of queues for tasks waiting on events. */
static Q event_queue[MAXEVENT];

/**
 * When creating a new task, it is important to initialize its stack just like
 * it has called "Enter_Kernel()"; so that when we switch to it later, we
 * can just restore its execution context on its stack.
 * (See file "cswitch.S" for details.)
 */
void Kernel_Create_Task_At(PD *p, voidfuncptr f, int arg, unsigned int level, unsigned int name)
{
    unsigned char *sp;

#ifdef DEBUG
    int counter = 0;
#endif

    //Changed -2 to -1 to fix off by one error.
    sp = (unsigned char *) &(p->workSpace[WORKSPACE-1]);



    /*----BEGIN of NEW CODE----*/
    //Initialize the workspace (i.e., stack) and PD here!

    //Clear the contents of the workspace
    memset(&(p->workSpace),0,WORKSPACE);

    //Notice that we are placing the address (16-bit) of the functions
    //onto the stack in reverse byte order (least significant first, followed
    //by most significant).  This is because the "return" assembly instructions
    //(rtn and rti) pop addresses off in BIG ENDIAN (most sig. first, least sig.
    //second), even though the AT90 is LITTLE ENDIAN machine.

    //Store terminate at the bottom of stack to protect against stack underrun.
    *(unsigned char *)sp-- = ((unsigned int)Task_Terminate) & 0xff;
    *(unsigned char *)sp-- = (((unsigned int)Task_Terminate) >> 8) & 0xff;
    *(unsigned char *)sp-- = 0x00;

    //Place return address of function at bottom of stack
    *(unsigned char *)sp-- = ((unsigned int)f) & 0xff;
    *(unsigned char *)sp-- = (((unsigned int)f) >> 8) & 0xff;
    *(unsigned char *)sp-- = 0x00;

#ifdef DEBUG
    //Fill stack with initial values for development debugging
    //Registers 0 -> 31 and the status register
    for (counter = 0; counter < 34; counter++)
    {
        *(unsigned char *)sp-- = counter;
    }
#else
    //Place stack pointer at top of stack
   sp = sp - 34;
#endif

    p->sp = sp;
    p->code = f;
    p->arg = arg;
    p->level = level;
    p->name = name;
    p->request = NONE;
    p->state = READY;
}

static void enqueue(Q* queue_ptr, PD* task_to_add)
{
    task_to_add->next = NULL;

    if(queue_ptr->head == NULL)
    {
        /* empty queue */
        queue_ptr->head = task_to_add;
        queue_ptr->tail = task_to_add;
    }
    else
    {
        /* put task at the back of the queue */
        queue_ptr->tail->next = task_to_add;
        queue_ptr->tail = task_to_add;
    }
}

static PD* dequeue(Q* queue_ptr)
{
    volatile PD* task_ptr = queue_ptr->head;

    if(queue_ptr->head != NULL)
    {
        queue_ptr->head = queue_ptr->head->next;
        task_ptr->next = NULL;
    }

    return task_ptr;
}

/**
  *  Create a new task
  */
static void Kernel_Create_Task(voidfuncptr f, int arg, unsigned int level, unsigned int name)
{
    int x;

    if (Tasks == MAXPROCESS) return;  /* Too many task! */

    /* find a DEAD PD that we can use  */
    for (x = 0; x < MAXPROCESS; x++) {
        if (Process[x].state == DEAD) break;
    }

    ++Tasks;
    Kernel_Create_Task_At(&(Process[x]), f, arg, level, name);
}


/**
  * This internal kernel function is a part of the "scheduler". It chooses the 
  * next task to run, i.e., Cp.
  */
static void Dispatch()
{
    if(system_queue.head != NULL)
    {
        Cp = dequeue(&system_queue);
    }
    else if(rr_queue.head != NULL)
    {
        Cp = dequeue(&rr_queue);
    }
    else
    {
        Cp = idle_task;
    }

    Cp->state = RUNNING;
}

/**
  * This internal kernel function is the "main" driving loop of this full-served
  * model architecture. Basically, on OS_Start(), the kernel repeatedly
  * requests the next user task's next system call and then invokes the
  * corresponding kernel function on its behalf.
  *
  * This is the main loop of our kernel, called by OS_Start().
  */
static void Next_Kernel_Request()
{
    Dispatch();  /* select a new task to run */

    while(1) {
        Cp->request = NONE; /* clear its request */

        /* activate this newly selected task */
        CurrentSp = Cp->sp;
        Exit_Kernel();    /* or CSwitch() */

        /* if this task makes a system call, it will return to here! */

        /* save the Cp's stack pointer */
        Cp->sp = CurrentSp;

        switch(Cp->request){
            case CREATE:
                Kernel_Create_Task(Cp->code, Cp->arg, Cp->level, Cp->name);

                /* enqueue READY RR tasks. */
                if(Cp->level == RR && Cp->state == READY)
                {
                    enqueue(&rr_queue, Cp);
                }

                break;
            case NONE:
            case NEXT:
                Cp->state = READY;
                switch(Cp->level){
                    case SYSTEM:
                        enqueue(&system_queue, Cp);
                        break;
                    case PERIODIC:
                        current_periodic_finished = 1;
                        break;
                    case RR:
                        enqueue(&rr_queue, Cp);
                        break;
                    default:
                        break;
                }
                Dispatch();
                break;
            case TERMINATE:
                /* deallocate all resources used by this task */
                Cp->state = DEAD;
                Dispatch();
                break;
            default:
                /* Houston! we have a problem here! */
                break;
        }
    }
}


/*================
  * RTOS  API  and Stubs
  *================
  */

/**
  * This function initializes the RTOS and must be called before any other
  * system calls.
  */
void OS_Init()
{
    int x;

    Tasks = 0;
    KernelActive = 0;
    NextP = 0;
    //Reminder: Clear the memory for the task on creation.
    for (x = 0; x < MAXPROCESS; x++) {
        memset(&(Process[x]),0,sizeof(PD));
        Process[x].state = DEAD;
    }
}


/**
  * This function starts the RTOS after creating a few tasks.
  */
void OS_Start()
{
    if ( (! KernelActive) && (Tasks > 0)) {
        Disable_Interrupt();
        /* we may have to initialize the interrupt vector for Enter_Kernel() here. */

        /* here we go...  */
        KernelActive = 1;
        Next_Kernel_Request();
        /* NEVER RETURNS!!! */
    }
}


/**
  * For this example, we only support cooperatively multitasking, i.e.,
  * each task gives up its share of the processor voluntarily by calling
  * Task_Next().
  */
int Task_Create(void (*f)(void), int arg, unsigned int level, unsigned int name)
{
    if (KernelActive) {
        Disable_Interrupt();
        Cp->request = CREATE;
        Cp->code = f;
        Cp->arg = arg;
        Cp->level = level;
        Cp->name = name;
        Enter_Kernel();
    } else {
        Kernel_Create_Task(f, arg, level, name);
    }

    return 0;
}

/**
  * The calling task gives up its share of the processor voluntarily.
  */
void Task_Next()
{
    if (KernelActive) {
        Disable_Interrupt();
        Cp->request = NEXT;
        Enter_Kernel();
    }
}


/**
  * The calling task terminates itself.
  */
void Task_Terminate()
{
    if (KernelActive) {
        Disable_Interrupt();
        Cp -> request = TERMINATE;
        Enter_Kernel();
        /* never returns here! */
    }
}

void setupTimer()
{
    TCCR0A = (1<<WGM01); //Set CTC bit
    OCR0A = 156;//Output compare register
    TIMSK0 = (1<<OCIE0A);
    TCCR0B = (1<<CS02) | (1<<CS00);

    sei();
    // enable interrupts
}

ISR(TIMER0_COMPA_vect)
{
    PORTL = 0x00;
    asm("jmp Task_Next"::);
}

/**
  * This function creates two cooperative tasks, "Ping" and "Pong". Both
  * will run forever.
  */
int main()
{

    OS_Init();

    a_main(0);

    setupTimer();

    OS_Start();

    return 0;
}
