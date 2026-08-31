/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the PSOC C1 MCU: FreeRTOS Blinky 
*              Example for ModusToolbox.
*
* Related Document: See README.md
*
*******************************************************************************
* (c) 2026, Infineon Technologies AG, or an affiliate of Infineon
* Technologies AG. All rights reserved.
* This software, associated documentation and materials ("Software") is
* owned by Infineon Technologies AG or one of its affiliates ("Infineon")
* and is protected by and subject to worldwide patent protection, worldwide
* copyright laws, and international treaty provisions. Therefore, you may use
* this Software only as provided in the license agreement accompanying the
* software package from which you obtained this Software. If no license
* agreement applies, then any use, reproduction, modification, translation, or
* compilation of this Software is prohibited without the express written
* permission of Infineon.
*
* Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
* IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
* THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
* SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
* Infineon reserves the right to make changes to the Software without notice.
* You are responsible for properly designing, programming, and testing the
* functionality and safety of your intended application of the Software, as
* well as complying with any legal requirements related to its use. Infineon
* does not guarantee that the Software will be free from intrusion, data theft
* or loss, or other breaches ("Security Breaches"), and Infineon shall have
* no liability arising out of any Security Breaches. Unless otherwise
* explicitly approved by Infineon, the Software may not be used in any
* application where a failure of the Product or any consequences of the use
* thereof can reasonably be expected to result in personal injury.
*******************************************************************************/


#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "cybsp.h"
#include "cy_utils.h"
#include "cy_gpio.h"

/*******************************************************************************
* Macros
*******************************************************************************/

#define BLINKY_TASK_NAME           ("Blinky")
#define BLINKY_TASK_STACK_SIZE     (configMINIMAL_STACK_SIZE)
#define BLINKY_TASK_PRIORITY       (tskIDLE_PRIORITY + 1)
#define MAIN_TASK_NAME             ("Main")
#define MAIN_TASK_STACK_SIZE       (configMINIMAL_STACK_SIZE)
#define MAIN_TASK_PRIORITY         (tskIDLE_PRIORITY + 1)

/* USER LED toggle period in milliseconds */
#define USER_LED_TOGGLE_PERIOD_MS 500

/*******************************************************************************
* Global Variables
*******************************************************************************/

/* RTOS semaphore */
static xSemaphoreHandle xSemaphore;

/*******************************************************************************
* Function Name: blinky_task
********************************************************************************
* Summary:
*  This RTOS task toggles the User LED each time the semaphore is obtained.
*
* Parameters:
*  void *pvParameters : Task parameter defined during task creation (unused)
*
* Return:
*  The RTOS task never returns.
*
*******************************************************************************/
__NO_RETURN static void blinky_task(void *pvParameters)
{
    CY_UNUSED_ARG(pvParameters);

    for(;;)
    {
        /* Block until the semaphore is given */
        xSemaphoreTake(xSemaphore, portMAX_DELAY);

        /* Toggle the USER LED state */
        Cy_GPIO_ToggleOutput(CYBSP_USER_LED_PORT, CYBSP_USER_LED_PIN);
    }
}

/*******************************************************************************
* Function Name: main_task
********************************************************************************
* Summary:
*  This RTOS task releases the semaphore every USER_LED_TOGGLE_PERIOD_MS.
*
* Parameters:
*  void *pvParameters : Task parameter defined during task creation (unused)
*
* Return:
*  The RTOS task never returns.
*
*******************************************************************************/
__NO_RETURN static void main_task(void *pvParameters)
{
    CY_UNUSED_ARG(pvParameters);

    for(;;)
    {
        /* Block task for USER_LED_TOGGLE_PERIOD_MS. */
        vTaskDelay(USER_LED_TOGGLE_PERIOD_MS);

        /* Release semaphore */
        xSemaphoreGive(xSemaphore);
    }
}

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
*  This is the main function. It creates two tasks, initializes the semaphore
*  for synchronization between tasks, and starts the FreeRTOS scheduler.
*
* Parameters:
*  none
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;
    BaseType_t retval;

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Create the Semaphore for synchronization between Blinky and Main task */
    xSemaphore = xSemaphoreCreateBinary();
    if( xSemaphore == NULL )
    {
        CY_ASSERT(0);
    }

    /* Create the RTOS tasks */
    retval = xTaskCreate(blinky_task, BLINKY_TASK_NAME, BLINKY_TASK_STACK_SIZE, NULL, BLINKY_TASK_PRIORITY, NULL );
    if (retval != pdPASS)
    {
        CY_ASSERT(0);
    }

    retval = xTaskCreate(main_task, MAIN_TASK_NAME, MAIN_TASK_STACK_SIZE, NULL, MAIN_TASK_PRIORITY, NULL );
    if (retval != pdPASS)
    {
        CY_ASSERT(0);
    }

    /* Start the scheduler */
    vTaskStartScheduler();

    for(;;)
    {
        /* vTaskStartScheduler never returns */
    }
}

/* [] END OF FILE */
