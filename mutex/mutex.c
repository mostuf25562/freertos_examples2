#include <stdio.h>
#include <stdlib.h>
#include <driver/gpio.h>
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define LED_PIN GPIO_NUM_21

#define TASK_1_NAME       "task 1"
#define TASK_1_STACK_SIZE (configMINIMAL_STACK_SIZE * 4)
#define TASK_1_PRIORITY   (tskIDLE_PRIORITY + 1)

#define TASK_2_NAME       "task 2"
#define TASK_2_STACK_SIZE (configMINIMAL_STACK_SIZE * 4)
#define TASK_2_PRIORITY   (tskIDLE_PRIORITY + 2)
#define MAX_LENGTH 6

// Declaração das funções
static void task_1(void *arg);
static void task_2(void *arg);

// Declaração dos handles
static TaskHandle_t xtask_handle_1 = NULL;
static TaskHandle_t xtask_handle_2 = NULL;

//TODOs:
//buffer size
SemaphoreHandle_t serialPrintSemaphore;




void serialPrintService(  char *format, ...)
{
  static uint8_t count = 0;


  if (xSemaphoreTake(serialPrintSemaphore, portMAX_DELAY) == pdTRUE)
  {
    if (count == MAX_LENGTH) {
      char finalMessage[] = "access to printing service is now locked";

      //infor client that printing service is blocked
      gpio_set_level(LED_PIN, 1);
      printf("%s\n\r", finalMessage);

      xSemaphoreGive(serialPrintSemaphore);
      count++;

    } else if (count < MAX_LENGTH) {
      va_list args;
      va_start(args, format);
      vprintf(format, args);
      va_end(args);
      xSemaphoreGive(serialPrintSemaphore);
      count++;

    }
  }
}

void setupPrintService() {
  if ( serialPrintSemaphore == NULL )  // Check to confirm that the Serial Semaphore has not already been created.
  {
    serialPrintSemaphore = xSemaphoreCreateMutex();  // Create a mutex semaphore we will use to manage the Serial Port
    if ( ( serialPrintSemaphore ) != NULL )
      xSemaphoreGive( ( serialPrintSemaphore ) );  // Make the Serial Port available for use, by "Giving" the Semaphore.
  }
  serialPrintService("protect access to printing service using mutex\n");

}

void app_main(void)
{
  setupPrintService();

  gpio_pad_select_gpio(LED_PIN);
  gpio_set_direction (LED_PIN, GPIO_MODE_OUTPUT);

  BaseType_t xReturn = pdPASS;



  xReturn = xTaskCreate(&task_1,
                        TASK_1_NAME,
                        TASK_1_STACK_SIZE,
                        NULL,
                        TASK_1_PRIORITY,
                        &xtask_handle_1);

  if (xReturn != pdPASS || xtask_handle_1 == NULL) {
    serialPrintService("Error creating task 1\n");
  }

  xReturn = xTaskCreate(&task_2,
                        TASK_2_NAME,
                        TASK_2_STACK_SIZE,
                        NULL,
                        TASK_2_PRIORITY,
                        &xtask_handle_2);

  if (xReturn != pdPASS || xtask_handle_2 == NULL) {
    serialPrintService("Error creating task 2\n");
  }

  while (1) {
    vTaskDelay(pdMS_TO_TICKS(10000));
  }
}

static void task_1(void *arg)
{
  uint8_t count = 0;

  while (true) {
    serialPrintService("%s tick : %d count: %d\n\r", __func__, xTaskGetTickCount(), count);
    vTaskDelay(pdMS_TO_TICKS(500));
    count++;
  }
}

static void task_2(void *arg)
{
  uint8_t count = 0;

  while (true) {
    serialPrintService("%s tick : %d count: %d\n\r", __func__, xTaskGetTickCount(), count);
    vTaskDelay(500 / portTICK_RATE_MS);
    count++;
  }

}
