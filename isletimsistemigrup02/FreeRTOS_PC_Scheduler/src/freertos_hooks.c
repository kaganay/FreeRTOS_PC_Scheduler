/* FreeRTOS hooks - FreeRTOS hook fonksiyonları */
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <stdlib.h>

/* Assert hatası durumunda çağrılan hook fonksiyonu
 *  - FreeRTOS configASSERT makrosu tetiklendiğinde buraya düşer
 *  - Hangi dosya/satırda hata olduğunu ekrana yazar ve programı durdurur
 */
void vAssertCalled( const char * const pcFileName, unsigned long ulLine )
{
    printf("ASSERT: %s:%lu\n", pcFileName, ulLine);
    fflush(stdout);
    abort();
}

/* Idle hook - sistemde çalışacak başka görev yokken (boşta) çağrılır */
void vApplicationIdleHook( void )
{
    /* Boş - gerekirse buraya kod eklenebilir */
}

/* Tick hook - her tick kesmesinde çağrılır, periyodik işler için kullanılabilir */
void vApplicationTickHook( void )
{
    /* Boş - gerekirse buraya kod eklenebilir */
}

/* Malloc failed hook - pvPortMalloc/vPortMalloc başarısız olduğunda çağrılır */
void vApplicationMallocFailedHook( void )
{
    printf("Malloc failed!\n");
    fflush(stdout);
    abort(); /* istersen sadece abort() yapmayabilirsin, ama debug için iyi */
}

/* ----------------------------------------------------------
   STATIC ALLOCATION HOOKS (configSUPPORT_STATIC_ALLOCATION=1)
   Linker hatası veren fonksiyonlar bunlardı.
   ---------------------------------------------------------- */

/* Idle task için statik bellek sağlayan hook */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize )
{
    /* Statik alanlar: program boyunca yaşar */
    static StaticTask_t xIdleTCB;
    static StackType_t  xIdleStack[ configMINIMAL_STACK_SIZE ];

    *ppxIdleTaskTCBBuffer   = &xIdleTCB;
    *ppxIdleTaskStackBuffer = xIdleStack;
    *pulIdleTaskStackSize   = ( uint32_t ) configMINIMAL_STACK_SIZE;
}

/* Timer task için statik bellek sağlayan hook (configUSE_TIMERS=1 ise gerekir) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize )
{
    static StaticTask_t xTimerTCB;
    static StackType_t  xTimerStack[ configTIMER_TASK_STACK_DEPTH ];

    *ppxTimerTaskTCBBuffer   = &xTimerTCB;
    *ppxTimerTaskStackBuffer = xTimerStack;
    *pulTimerTaskStackSize   = ( uint32_t ) configTIMER_TASK_STACK_DEPTH;
}

/* Timer daemon task başlarken çağrılır (configUSE_DAEMON_TASK_STARTUP_HOOK=1 ise gerekir) */
void vApplicationDaemonTaskStartupHook( void )
{
    /* Boş - istersen burada bir log atabilirsin */
}
