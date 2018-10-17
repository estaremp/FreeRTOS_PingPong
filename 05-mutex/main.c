//*****************************************************************************
//
// Copyright (C) 2015 - 2016 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//  Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//
//  Redistributions in binary form must reproduce the above copyright
//  notice, this list of conditions and the following disclaimer in the
//  documentation and/or other materials provided with the
//  distribution.
//
//  Neither the name of Texas Instruments Incorporated nor the names of
//  its contributors may be used to endorse or promote products derived
//  from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

// Includes standard
#include <stdio.h>
#include <stdint.h>

// Includes FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

// Definicion de prioridades de tareas
#define prvSENDER_TASK_PRIORITY    3
#define prvRECEIVER_TASK_PRIORITY  1

// Definicion de color de LED
#define RED     0
#define GREEN   1

// Prototipos de funciones privadas
static void prvSetupHardware(void);
static void prvSenderTask(void *pvParameters);
static void prvReceiverTask(void *pvParameters);

// Declaracion de un semaforo binario
// para sincronizar SenderTask con la ISR
SemaphoreHandle_t xBinarySemaphore;

// Declaracion de un mutex
SemaphoreHandle_t xMutex;

// Tipo comando
typedef struct {
    // Led a activar en tarea ReceiverTask
    uint8_t led;
    // Tiempo de activacion del LED
    uint16_t blinkOnMs;
    // Flag indicador de comando nuevo
    bool nuevo;
} command_t;

// Variable comando compartida entre las
// tareas SenderTask y ReceiverTask
command_t commandVar;

int main(void)
{
    // Inicializacion del semaforo binario
    xBinarySemaphore = xSemaphoreCreateBinary();
    // Inicializacion del mutex
    xMutex = xSemaphoreCreateMutex();
    // Inicialización de la variable de comando
    commandVar.led = RED;
    commandVar.blinkOnMs = 100;
    commandVar.nuevo = true;

    // Comprueba si semaforo y mutex se han creado bien
    if ((xBinarySemaphore != NULL) && (xMutex != NULL))
    {
        // Inicializacion del hardware (clocks, GPIOs, IRQs)
        prvSetupHardware();
        // Creacion de tarea SenderTask
        xTaskCreate( prvSenderTask,
                    "SenderTask",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    prvSENDER_TASK_PRIORITY,
                    NULL );
        // Creacion de tarea ReceiverTask
        xTaskCreate( prvReceiverTask,
                    "ReceiverTask",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    prvRECEIVER_TASK_PRIORITY,
                    NULL );
        // Puesta en marcha de las tareas creadas
        vTaskStartScheduler();
    }

    // Solo llega aqui si no hay suficiente memoria
    // para iniciar el scheduler
    return 0;
}

// Inicializacion del hardware del sistema
static void prvSetupHardware(void)
{
    extern void FPU_enableModule(void);

    // Inicializacion de pins sobrantes para reducir consumo
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, PIN_ALL8);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_PB, PIN_ALL16);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_PC, PIN_ALL16);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_PD, PIN_ALL16);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_PE, PIN_ALL16);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, PIN_ALL8);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_PB, PIN_ALL16);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_PC, PIN_ALL16);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_PD, PIN_ALL16);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_PE, PIN_ALL16);

    // Habilita la FPU
    MAP_FPU_enableModule();
    // Cambia el numero de "wait states" del controlador de Flash
    MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
    MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);

    // Selecciona la frecuencia central de un rango de frecuencias del DCO
    MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_6);
    // Configura la frecuencia del DCO
    CS_setDCOFrequency(8000000);

    // Inicializa los clocks HSMCLK, SMCLK, MCLK y ACLK
    MAP_CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    // Selecciona el nivel de tension del core
    MAP_PCM_setCoreVoltageLevel(PCM_VCORE0);

    // Configura el pin P1.0 como salida (LED rojo)
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

    // Configura el pin P2.1 como salida (LED verde)
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);

    // Configuracion del pin P1.1 como entrada con R de pull-up
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);

    // Reset del flag de interrupcion del pin P1.1
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    // Habilita la interrupcion del pin P1.1
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
    // Configura la prioridad de la interrupcion del PORT1
    MAP_Interrupt_setPriority(INT_PORT1, 0xA0);
    // Habilita la interrupcion del PORT1
    MAP_Interrupt_enableInterrupt(INT_PORT1);
    // Habilita que el procesador responda a las interrupciones
    MAP_Interrupt_enableMaster();
}

// Tarea SenderTask
static void prvSenderTask (void *pvParameters)
{
    // Tiempo maximo de espera entre dos interrupciones del pulsador
    const TickType_t xMaxExpectedBlockTime = pdMS_TO_TICKS( 500 );

    // La tarea se repite en un bucle infinito
    for(;;) {
        // El semaforo debe ser entregado por la ISR PORT1_IRQHandler
        // Espera un numero maximo de xMaxExpectedBlockTime ticks
        if( xSemaphoreTake( xBinarySemaphore, xMaxExpectedBlockTime ) == pdPASS )
        {
            // Intenta coger el mutex, bloqueandose si no esta disponible
            xSemaphoreTake( xMutex, portMAX_DELAY );
            {
                // Escribe el nuevo comando en la variable commandVar
                if ( commandVar.led == RED )
                {
                    commandVar.blinkOnMs = 500;
                    commandVar.led = GREEN;
                }
                else
                {
                    commandVar.blinkOnMs = 100;
                    commandVar.led = RED;
                }
                // Activa el flag para indicar hay comando nuevo
                commandVar.nuevo = true;
            }
            // Libera el mutex
            xSemaphoreGive( xMutex );
        }
    }
}

// Tarea ReceiverTask
static void prvReceiverTask (void *pvParameters)
{
    // Tiempo de activacion del LED (en ticks)
    static TickType_t xBlinkOn;

    // La tarea se repite en un bucle infinito
    for(;;) {
        // Intenta coger el mutex, bloqueandose si no esta disponible
        xSemaphoreTake( xMutex, portMAX_DELAY );
        {
            // Comprueba si hay comando nuevo
            if ( commandVar.nuevo == true)
            {
                // Calcula el tiempo de activacion del LED (en ticks)
                xBlinkOn  = pdMS_TO_TICKS( commandVar.blinkOnMs );
                // Desactiva el flag para indicar no hay comando nuevo
                commandVar.nuevo = false;
                // Ejecuta el comando
                switch (commandVar.led)
                {
                case RED:
                    // Libera el mutex
                    xSemaphoreGive( xMutex );
                    // Fija a 1 la salida digital en pin 1.0 (LED rojo on)
                    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
                    // Bloquea la tarea durante el tiempo de on del LED
                    vTaskDelay(xBlinkOn);
                    // Fija a 0 la salida digital en pin 1.0 (LED rojo off)
                    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
                    break;
                case GREEN:
                default:
                    // Libera el mutex
                    xSemaphoreGive( xMutex );
                    // Fija a 1 la salida digital en pin 2.1 (LED on)
                    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
                    // Bloquea la tarea durante el tiempo de on del LED
                    vTaskDelay(xBlinkOn);
                    // Fija a 0 la salida digital en pin 2.1 (LED off)
                    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
                    break;
                }
            }
            else // no hay comando nuevo
            {
                // Libera el mutex
                xSemaphoreGive( xMutex );
            }
        }
    }
}

// Rutina de Servicio a Interrupcion (ISR) del PORT1
void PORT1_IRQHandler(void)
{
    uint32_t status;
    static BaseType_t xHigherPriorityTaskWoken;

    // Lee el estado de la interrupcion generada por GPIO_PORT_P1
    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    // Reset del flag de interrupcion del pin que la genera
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    // Chequea si la interrupcion la genero el pin P1.1
    if(status & GPIO_PIN1)
    {
        // El parametro xHigherPriorityTaskWoken debe inicializarse
        // en pdFALSE, ya que se establecera en pdTRUE dentro de la
        // funcion API de interrupcion segura si se requiere un cambio
        // de contexto
        xHigherPriorityTaskWoken = pdFALSE;
        // Entrega el semaforo para desbloquear la tarea SenderTask
        xSemaphoreGiveFromISR(xBinarySemaphore,
                &xHigherPriorityTaskWoken);

        // Pasa xHigherPriorityTaskWoken en portYIELD_FROM_ISR().
        // Si xHigherPriorityTaskWoken valia pdTRUE dentro de
        // xSemaphoreGiveFromISR(), entonces al llamar a
        // portYIELD_FROM_ISR() solicitara un cambio de contexto.
        // Si xHigherPriorityTaskWoken sigue siendo pdFALSE, la llamada
        // a portYIELD_FROM_ISR() no tendra ningun efecto
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
}
