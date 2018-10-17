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

// Definicion de prioridades de tareas
#define prvRED_LED_TASK_PRIORITY     (tskIDLE_PRIORITY + 1)

// Prototipos de funciones privadas
static void prvSetupHardware(void);
static void prvRedLedTask(void *pvParameters);

int main(void)
{
    // Inicializacion del hardware del sistema
    prvSetupHardware();

    // Creacion de tarea RedLedTask
    xTaskCreate( prvRedLedTask,            // Puntero a la funcion que implementa la tarea
                "RedLedTask",              // Nombre descriptivo de la tarea
                configMINIMAL_STACK_SIZE,  // Tamaño del stack de la tarea
                NULL,                      // Argumentos de la tarea
                prvRED_LED_TASK_PRIORITY,  // Prioridad de la tarea
                NULL );

    // Puesta en marcha de las tareas creadas
    vTaskStartScheduler();

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

    // Configura el pin P1.0 como salida (LED)
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
}

// Tarea RedLedTask
static void prvRedLedTask (void *pvParameters)
{
    // Calcula el tiempo de activacion del LED (en ticks)
    // a partir del tiempo en milisegundos
    static const TickType_t xBlinkOn  = pdMS_TO_TICKS(10);

    // Calcula el tiempo de desactivacion del LED (en ticks)
    // a partir del tiempo en milisegundos
    static const TickType_t xBlinkOff = pdMS_TO_TICKS(900);

    // La tarea se repite en un bucle infinito
    for(;;) {
        // Fija a 1 la salida digital en pin 1.0 (LED on)
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
        // Bloquea la tarea durante el tiempo de on del LED
        vTaskDelay(xBlinkOn);
        // Fija a 0 la salida digital en pin 1.0 (LED off)
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
        // Bloquea la tarea durante el tiempo de off del LED
        vTaskDelay(xBlinkOff);
    }
}
