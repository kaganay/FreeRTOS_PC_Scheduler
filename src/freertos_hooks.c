#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include <stdio.h>
#include <stdlib.h>

/* Basit assert: mesaj yaz ve dur */
void vAssertCalled( const char * const pcFileName, unsigned long ulLine )
{
    printf("ASSERT: %s:%lu\n", pcFileName, ulLine);
    fflush(stdout);
    abort();
}

/* Statik idle task bellekleri */
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE_STATIC ];

void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
                                    StackType_t ** ppxIdleTaskStackBuffer,
                                    uint32_t * pulIdleTaskStackSize )
{
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE_STATIC;
}

/* Statik timer task bellekleri */
static StaticTask_t xTimerTaskTCB;
static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH_STATIC ];

void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer,
                                     StackType_t ** ppxTimerTaskStackBuffer,
                                     uint32_t * pulTimerTaskStackSize )
{
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH_STATIC;
}

/* Opsiyonel hook'lar: burada boş bırakıldı */
void vApplicationTickHook( void ) {}
void vApplicationIdleHook( void ) {}
void vApplicationDaemonTaskStartupHook( void ) {}

