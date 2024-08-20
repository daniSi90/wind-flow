/**
 ********************************************************************************
 * @file    wind_flow.c
 * @author  Danijel Sipos
 * @date    06.08.2024
 * @brief   Implementation of wind_flow
 *
 *
 * Copyright (c) 2024 Danijel SIPOS
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 ********************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "wind_flow.h"

#ifdef CONFIG_IDF_TARGET
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#if 1
#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#else
#define LOG_LOCAL_LEVEL ESP_LOG_NONE
#endif
#include "esp_log.h"
#endif
/* Private define ------------------------------------------------------------*/
static const char *TAG = "wf";

/* Private macro -------------------------------------------------------------*/
#ifdef CONFIG_IDF_TARGET
#define WF_LOGD(format, ...) ESP_LOGD(TAG, format, ##__VA_ARGS__)
#define WF_LOGI(format, ...) ESP_LOGI(TAG, format, ##__VA_ARGS__)
#define WF_LOGE(format, ...) ESP_LOGE(TAG, format, ##__VA_ARGS__)
#define WF_LOGW(format, ...) ESP_LOGW(TAG, format, ##__VA_ARGS__)
#define WF_SLEEP_MS(ms)      vTaskDelay(pdMS_TO_TICKS(ms))
#else
#define WF_LOGD(format, ...)
#define WF_LOGI(format, ...) printf(format, ##__VA_ARGS__)
#define WF_LOGE(format, ...) printf(format, ##__VA_ARGS__)
#define WF_LOGW(format, ...) printf(format, ##__VA_ARGS__)
#define WF_SLEEP_MS(ms)      // Sleep(ms)
#endif

/* Private variables ---------------------------------------------------------*/
static wf_handle_t wf_handle = { 0 };
/* Private function prototypes -----------------------------------------------*/

/* Public functions ----------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
wf_list_t *
wf_list_add_next(wf_list_t *p_list)
{
    wf_list_t *p_list_next = malloc(sizeof(wf_list_t));
    if (p_list_next == NULL)
    {
        WF_LOGE("Failed to allocate memory for connectivity list\n");
        return p_list_next;
    }
    memset(p_list_next, 0, sizeof(wf_list_t));
    if (p_list != NULL)
    {
        p_list->next          = p_list_next;
        p_list_next->previous = p_list;
        p_list_next->level    = p_list->level + 1;
        wf_handle.level_set = p_list->level + 1;
    }
    else
    {
        wf_handle.p_list_current = p_list_next; /// When input list is NULL this means its the begginning of the list
        wf_handle.level_current = -1;
    }
    WF_LOGD("add next: level %d\n", p_list_next->level);

    return p_list_next;
}

bool
wf_list_add_wind_config(wf_list_t *p_list, wf_config_t *p_config)
{
    if ((p_list == NULL) || (p_config == NULL))
    {
        WF_LOGE("Invalid parameters\n");
        return false;
    }
    memcpy(&p_list->config_wind, p_config, sizeof(wf_config_t));

    return true;
}

bool
wf_list_add_unwind_config(wf_list_t *p_list, wf_config_t *p_config)
{
    if (p_list == NULL || p_config == NULL)
    {
        WF_LOGE("Invalid parameters\n");
        return false;
    }
    memcpy(&p_list->config_unwind, p_config, sizeof(wf_config_t));

    return true;
}

bool
wf_list_event_done(uint8_t level)
{
    if (wf_handle.p_list_current == NULL)
    {
        WF_LOGE("List is empty\n");
        return false;
    }
    if (wf_handle.p_list_current->level != level)
    {
        return false;
    }
    wf_handle.p_list_current->_event_done = true;
    return true;
}

bool wf_is_busy(void)
{
    return wf_handle.p_list_current != NULL;
}

int8_t wf_list_wind(void)
{
    if (wf_handle.p_list_current != NULL)
    {
        return wf_handle.p_list_current->level;
    }
    wf_handle.wind = true;
    return -1;
}

int8_t wf_list_unwind(void)
{
    if (wf_handle.p_list_current != NULL)
    {
        return wf_handle.p_list_current->level;
    }
    wf_handle.unwind = true;
    return -1;
}

wf_list_t *
wf_list_execute(void)
{
    if (wf_handle.wind && (wf_handle.dir != WF_DIR_WIND)) /// Handle if WINDING is triggered
    {
        WF_LOGI("Unwinding triggered\n");
        wf_handle.p_list_current = wf_handle.p_list_previous;
        wf_handle.dir = WF_DIR_WIND;
        wf_handle.wind = false;
    }
    else if (wf_handle.unwind && (wf_handle.dir != WF_DIR_UNWIND)) /// Handle if UNWINDING is triggered
    {
        wf_handle.p_list_current = wf_handle.p_list_previous;
        wf_handle.dir = WF_DIR_UNWIND;
        wf_handle.unwind = false;
    }

    if (wf_handle.p_list_current == NULL)
    {
        return wf_handle.p_list_current;
    }

    if (wf_handle.dir == WF_DIR_WIND)
    {
        /// Checking if there is an event to wait for before continuing
        if ((wf_handle.p_list_current->event_wait == true) && (wf_handle.p_list_current->_wind_done == true))
        {
            if (wf_handle.p_list_current->_event_done == true)
            {
                wf_handle.p_list_current = wf_handle.p_list_current->next;
                WF_LOGI("Waiting for event DONE\n");
            }
            return wf_handle.p_list_current;
        }

        if (wf_handle.p_list_current->config_wind.fp_call != NULL)
        {
            if (wf_handle.p_list_current->config_wind.fp_call(wf_handle.p_list_current, wf_handle.p_list_current->config_wind.p_args) != true)
            {
                WF_LOGE("Failed to execute wind function\n");
                if (wf_handle.p_list_current->config_wind._retries_cnt++ >= wf_handle.p_list_current->config_wind.retries_max)
                {
                    WF_LOGE("Max retries reached\n");
                    wf_handle.p_list_current->config_wind._retries_cnt = 0;
                    wf_handle.dir                                      = WF_DIR_UNWIND;
                    wf_handle.has_failed                               = true;
                    wf_handle.level_current--;
                    wf_handle.p_list_previous = wf_handle.p_list_current;
                    return wf_handle.p_list_current                    = wf_handle.p_list_current->previous;
                }
                WF_SLEEP_MS(wf_handle.p_list_current->config_wind.delay_ms);
                return wf_handle.p_list_current;
            }
            wf_handle.p_list_current->_wind_done = true;
            if (wf_handle.p_list_current->event_wait != true) /// Continiue to next if no event is needed, otherwise wait
            {
                wf_handle.level_current++;
                wf_handle.p_list_previous = wf_handle.p_list_current;
                wf_handle.p_list_current = wf_handle.p_list_current->next;
            }
        }
        else
        {
            WF_LOGI("END OF LIST\n");
        }
    }
    else if (wf_handle.dir == WF_DIR_UNWIND)
    {
        if ((wf_handle.p_list_current->config_unwind.level_safe <= wf_handle.p_list_current->level) && (wf_handle.has_failed == true))
        {
            WF_LOGI("Go back to wind\n");
            wf_handle.has_failed = false;
            wf_handle.dir        = WF_DIR_WIND;
            return wf_handle.p_list_current;
        }
        if (wf_handle.p_list_current->config_unwind.fp_call != NULL)
        {
            if (wf_handle.p_list_current->config_unwind.fp_call(wf_handle.p_list_current, wf_handle.p_list_current->config_unwind.p_args) != true)
            {
                WF_LOGE("Failed to execute unwind function\n");
                if (wf_handle.p_list_current->config_wind._retries_cnt++ >= wf_handle.p_list_current->config_wind.retries_max)
                {
                    WF_LOGE("Max retries reached\n");
                    wf_handle.p_list_current->config_wind._retries_cnt = 0;
                    wf_handle.dir                                      = WF_DIR_WIND;
                    wf_handle.has_failed                               = true;
                }
                return wf_handle.p_list_current;
            }
            wf_handle.p_list_previous = wf_handle.p_list_current;
            wf_handle.p_list_current = wf_handle.p_list_current->previous;
        }
        else
        {
            WF_LOGI("Doesnt contain deinit function or at end\n");
        }
    }

    return wf_handle.p_list_current;
}