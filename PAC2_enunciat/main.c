/*
 * Copyright (C) 2017 Universitat Oberta de Catalunya - http://www.uoc.edu/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Universitat Oberta de Catalunya nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*----------------------------------------------------------------------------*/

/* Standard includes */
#include <stdlib.h>
#include <stdio.h>



/* Free-RTOS includes */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "portmacro.h"


/* MSP432, Wi-Fi and UART includes */
#include "msp432_launchpad_board.h"
#include "cc3100_boosterpack.h"
#include "cli_uart.h"


/*----------------------------------------------------------------------------*/

#define SPAWN_TASK_PRIORITY         ( tskIDLE_PRIORITY + 6 )
#define MAIN_TASK_PRIORITY          ( tskIDLE_PRIORITY + 3 )
#define SND_TASK_PRIORITY           ( tskIDLE_PRIORITY + 2 )
#define RCV_TASK_PRIORITY           ( tskIDLE_PRIORITY + 1 )
#define BLINK_TASK_PRIORITY         ( tskIDLE_PRIORITY + 1 )

#define MAIN_STACK_SIZE             ( 1024 )
#define SND_STACK_SIZE               ( 1024 )
#define RCV_STACK_SIZE               ( 1024 )
#define BLINK_STACK_SIZE            ( 128 )

#define SERVER_ADDRESS              ( "192.168.2.101")
#define SERVER_PORT                 ( 5005 )
#define BUFFER_SIZE                 ( 16 )
#define PING_NUMBER                 ( 10  )

/*----------------------------------------------------------------------------*/

static void BlinkTask(void *pvParameters);
static void MainTask(void *pvParameters);
static void SNDTask(void *pvParameters);
static void RCVTask(void *pvParameters);

/*----------------------------------------------------------------------------*/

// Declaracion de un mutex
SemaphoreHandle_t mutSOCKET;
SemaphoreHandle_t semaphoreEND;
SemaphoreHandle_t semaphoreRCV;
QueueHandle_t q;

int16_t socket_id = -1;

static void BlinkTask(void *pvParameters) {
    while(true)
    {
        /* Turn red LED on */
        led_red_on();

        /* Sleep for 10 ms */
        vTaskDelay(pdMS_TO_TICKS(10));

        /* Turn red LED on */
        led_red_off();

        /* Sleep for 990 ms */
        vTaskDelay(pdMS_TO_TICKS(990));
    }
}

static void MainTask(void *pvParameters) {
    int16_t retVal = -1;
    SlSockAddrIn_t Addr;
    int ip_address;
    char message[50];

    // Intenta coger el mutex, bloqueandose si no esta disponible
    xSemaphoreTake( mutSOCKET, portMAX_DELAY );{

    /* Initialize Wi-Fi */
    retVal = wifi_init();
    if (retVal < 0) {
        led_red_on();
        CLI_Write(" Failed to init wifi module. \n\r");
        while(1);
    }

    /* Set socket address */
    retVal = stringToAddress(SERVER_ADDRESS, &ip_address);
    if (!retVal) {
        CLI_Write(" Malformed IP address. \n\r");
    }
    Addr.sin_family = AF_INET;
    Addr.sin_addr.s_addr = sl_Htonl( (uint32_t) ip_address);
    Addr.sin_port = sl_Htons((uint16_t) SERVER_PORT);

    /* Create TCP socket */
    while (socket_id < 0){

        socket_id =  wifi_tcp_client_open(&Addr);

        if (socket_id < 0) {
            led_red_on();
            CLI_Write(" Failed to create TCP socket. \n\r");
        }
        xSemaphoreGive( mutSOCKET );
    }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // Intenta coger el mutex, bloqueandose si no esta disponible
    xSemaphoreTake( semaphoreEND, portMAX_DELAY );{

    retVal = wifi_client_close(socket_id);
    if (retVal <0){
        led_red_on();
        CLI_Write(" Failed to close TCP socket. \n\r");
    }
    CLI_Write ("PING PONG finished!");

    }

}

static void SNDTask(void *pvParameters) {

    int16_t retVal = -1;

    uint8_t txBuffer[BUFFER_SIZE];

    uint32_t ping_counter = 0;

    BaseType_t status;

    char message[50];

    // Intenta coger el mutex, bloqueandose si no esta disponible
    xSemaphoreTake( mutSOCKET, portMAX_DELAY );
    {

    for(;;)
    {

    /* Turn green LED on */
    led_green_on();

    /* Send TCP packet*/
    sprintf(message, "PING %d", ping_counter);
    strcpy((char*) txBuffer, message);
    retVal =  wifi_tcp_client_send(socket_id, txBuffer, BUFFER_SIZE);
    if (retVal <0){
        led_red_on();
        CLI_Write(" Failed to send data through TCP socket. \n\r");
    }
    sprintf(message, "Sent PING %d \n\r", ping_counter);
    CLI_Write((unsigned char*) message);

    /*Allow to receive */
    xSemaphoreGiveFromISR( semaphoreRCV, pdFALSE );

    /* Block until PONG has been received in order to keep sending */
    status = xQueueReceive( q, &ping_counter, portMAX_DELAY );


    }
    }
}

static void RCVTask(void *pvParameters) {

    int16_t retVal = -1;

    uint8_t rxBuffer[BUFFER_SIZE];

    uint32_t ping_counter=0;

    BaseType_t status;

    /* Take semaphore to recive if sender has granted it*/
    xSemaphoreTake( semaphoreRCV, portMAX_DELAY );{

    char message[50];

    for(;;){

    /* Receive TCP packet */
    retVal = -1;
    while (retVal<1){
        //CLI_Write(" TASK2-waiting for Pong. \n\r");
        retVal = wifi_tcp_client_receive(socket_id, rxBuffer,  BUFFER_SIZE);
    }

    rxBuffer[retVal] = '\0';
    CLI_Write(rxBuffer); CLI_Write("\n\r");

    /* Increase counter */
    ping_counter++;
;
    if (ping_counter==PING_NUMBER) {

        /* Release the sempahore to close connection */
        xSemaphoreGiveFromISR( semaphoreEND, pdFALSE );

    } else {

        /* Return counter to Task 2 */
        status =  xQueueSendToBack( q, &ping_counter, 0);

        if( status != pdPASS ) {
            CLI_Write(" Failed to send data to the queue. \n\r");
        }
    }
    }
    }

}

/*----------------------------------------------------------------------------*/

int main(int argc, char** argv){

    mutSOCKET = xSemaphoreCreateMutex();
    semaphoreEND = xSemaphoreCreateBinary();
    semaphoreRCV = xSemaphoreCreateBinary();
    q = xQueueCreate(5, sizeof(int32_t));

    // Comprueba si semaforo y mutex se han creado bien
    if (mutSOCKET!=NULL)
    {
    int32_t retVal = -1;
    /* Initialize the board */
    board_init();

    /* Set up Command Line Interface (UART) */
    CLI_Configure();

    /* Start the SimpleLink task to manage CC3100 events*/
    retVal = VStartSimpleLinkSpawnTask(SPAWN_TASK_PRIORITY);
    if(retVal < 0)
    {
        led_red_on();
        while(1);
    }

    /* Create blink task */
    //retVal = xTaskCreate(BlinkTask,
     //                    "BlinkTask",
     //                    BLINK_STACK_SIZE,
     //                    NULL,
     //                    BLINK_TASK_PRIORITY,
     //                    NULL );
    //if(retVal < 0)
    //{
    //    led_red_on();
    //    while(1);
    //}

    /* Create main task */
    retVal = xTaskCreate(MainTask,
                         "MainTask",
                         MAIN_STACK_SIZE,
                         NULL,
                         MAIN_TASK_PRIORITY,
                         NULL );
    if(retVal < 0)
    {
        led_red_on();
        while(1);
    }

    /* Create send task */
    retVal = xTaskCreate(SNDTask,
                         "SNDTask",
                         SND_STACK_SIZE,
                         NULL,
                         SND_TASK_PRIORITY,
                         NULL );

    if(retVal < 0)
    {
        led_red_on();
        while(1);
    }

    /* Create receive task */
    retVal = xTaskCreate(RCVTask,
                         "RCVTask",
                         RCV_STACK_SIZE,
                         NULL,
                         RCV_TASK_PRIORITY,
                         NULL );

    if(retVal < 0)
    {
        led_red_on();
        while(1);
    }

    /* Start the task scheduler */
    vTaskStartScheduler();
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

