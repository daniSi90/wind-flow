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

#define WF_CB_LEVEL_CHANGED(_state)                         \
    if (wf_handle.level_prev != wf_handle.level_current)    \
    {                                                       \
        if (wf_handle.fp_level_changed != NULL)             \
        {                                                   \
            wf_handle.fp_level_changed(&wf_handle, _state); \
        }                                                   \
        wf_handle.level_prev = wf_handle.level_current;     \
    }

#define WF_CB_WIND_UNWIND_STATE(_state)                     \
    if (wf_handle.fp_wind_unwind_state != NULL)             \
    {                                                       \
        wf_handle.fp_wind_unwind_state(&wf_handle, _state); \
    }

/* Private macro -------------------------------------------------------------*/
#ifdef CONFIG_IDF_TARGET
#define WF_LOGD(format, ...) ESP_LOGD(TAG, format, ##__VA_ARGS__)
#define WF_LOGI(format, ...) ESP_LOGI(TAG, format, ##__VA_ARGS__)
#define WF_LOGE(format, ...) ESP_LOGE(TAG, format, ##__VA_ARGS__)
#define WF_LOGW(format, ...) ESP_LOGW(TAG, format, ##__VA_ARGS__)
#else
#define WF_LOGD(format, ...)
#define WF_LOGI(format, ...) printf(format, ##__VA_ARGS__)
#define WF_LOGE(format, ...) printf(format, ##__VA_ARGS__)
#define WF_LOGW(format, ...) printf(format, ##__VA_ARGS__)
#endif

#define RETRY_CNT_DEFAULT 0xffffffff /**< Default value for the retry_cnt to make sure it enters at first retry */
#define WF_LEVEL_NULL     -1         /**< Default value for the level */

static wf_handle_t wf_handle = { .retry_cnt = RETRY_CNT_DEFAULT, .level_prev = -1 };

wf_handle_t *
wf_handle_init(fp_init_call_t fp_init)
{
    if (fp_init == NULL)
    {
        WF_LOGE("Invalid parameters\n");
        return NULL;
    }

    wf_handle.fp_init = fp_init;
    return &wf_handle;
}

void
wf_handle_add_level_changed(fp_state_t fp_level_changed)
{
    wf_handle.fp_level_changed = fp_level_changed;
}

void
wf_handle_add_wind_unwind_state(fp_state_t fp_wind_unwind_state)
{
    wf_handle.fp_wind_unwind_state = fp_wind_unwind_state;
}

void
wf_handle_set_period(uint32_t period_ms)
{
    wf_handle.update_period = period_ms;
}

wf_list_t *
wf_list_add_next(wf_list_t *p_list)
{
    wf_list_t *p_list_next = malloc(sizeof(wf_list_t));
    // printf("## malloc - %p\n", p_list_next);
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
        wf_handle.level_set   = p_list->level + 1;
    }
    else
    {
        wf_handle.p_list_previous = p_list_next; /// When input list is NULL this means its the beginning of the list
        wf_handle.level_current   = WF_LEVEL_NULL;
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

bool
wf_is_busy(void)
{
    return wf_handle.p_list_current != NULL;
}

int8_t
wf_list_wind(void)
{
    if (wf_handle.p_list_current != NULL)
    {
        return wf_handle.p_list_current->level;
    }
    wf_handle.wind = true;
    return -1;
}

int8_t
wf_list_unwind(void)
{
    wf_handle.unwind = true;
    if (wf_handle.p_list_current != NULL)
    {
        wf_handle.p_list_current->_event_done = false;
        wf_state_t wu_state                   = wf_handle.p_list_current->level == WF_LEVEL_NULL ? WF_STATE_UNWINDED : WF_STATE_BUSY;
        WF_CB_WIND_UNWIND_STATE(wu_state);
        return wf_handle.p_list_current->level;
    }
    WF_CB_WIND_UNWIND_STATE(WF_STATE_UNWINDED);

    return WF_LEVEL_NULL;
}

void
wf_list_execute(void)
{
    // WF_LOGI("level %d\n", wf_handle.level_current);
    wf_state_t wu_state = WF_STATE_BUSY;
    /// Handle if WINDING is triggered
    if (wf_handle.wind && (wf_handle.dir != WF_DIR_WIND))
    {
        WF_LOGI("WIND triggered\n");
        if (wf_handle.p_list_current == NULL && (wf_handle.p_list_previous == NULL))
        {
            wf_handle.fp_init();
        }
        wf_handle.p_list_current = wf_handle.p_list_previous;

        wf_handle.dir  = WF_DIR_WIND;
        wf_handle.wind = false;
    }
    else if (wf_handle.wind)
    {
        wf_handle.wind = false;
    }
    /// Handle if UNWINDING is triggered
    if (wf_handle.unwind && (wf_handle.level_current != WF_LEVEL_NULL))
    {
        WF_LOGI("UNWIND triggered\n");
        if (wf_handle.p_list_current == NULL)
        {
            wf_handle.p_list_current = wf_handle.p_list_previous;
        }
        else if (!wf_handle.p_list_current->_wind_done)
        {
            wf_handle.p_list_current = wf_handle.p_list_previous;
        }
        wf_handle.dir    = WF_DIR_UNWIND;
        wf_handle.unwind = false;
    }
    else if (wf_handle.unwind)
    {
        wf_handle.unwind = false;
    }

    if (wf_handle.p_list_current == NULL)
    {
        return;
    }
    wf_handle.level_current = wf_handle.p_list_current->level;

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
            return;
        }

        if (wf_handle.p_list_current->config_wind.fp_call != NULL)
        {
            if (wf_handle.retry_cnt >= wf_handle.p_list_current->config_wind.n_skip_period)
            {
                if (wf_handle.p_list_current->config_wind.fp_call(wf_handle.p_list_current, wf_handle.p_list_current->config_wind.p_args) != true)
                {
                    WF_LOGE("Failed to execute wind function\n");
                    if (wf_handle.p_list_current->config_wind._retries_cnt++ >= wf_handle.p_list_current->config_wind.retries_max)
                    {
                        WF_LOGE("Max retries reached\n");
                        wf_handle.retry_cnt                                = RETRY_CNT_DEFAULT; /// At this point we set it back to default and make sure it immediately enters at first retry
                        wf_handle.p_list_current->config_wind._retries_cnt = 0;
                        wf_handle.dir                                      = WF_DIR_UNWIND;
                        wf_handle.has_failed                               = true;
                        wf_handle.p_list_previous                          = wf_handle.p_list_current;
                        wf_handle.p_list_current                           = wf_handle.p_list_current->previous;
                        return;
                    }
                    wf_handle.retry_cnt = 0;
                    return;
                }
                wf_handle.retry_cnt                  = RETRY_CNT_DEFAULT;
                wf_handle.p_list_current->_wind_done = true;
            }
            else
            {
                wf_handle.retry_cnt++;
            }
            if (wf_handle.p_list_current->event_wait != true) /// Continue to next if no event is needed, otherwise wait
            {
                // wf_handle.level_current++;
                wf_handle.p_list_previous            = wf_handle.p_list_current;
                wf_handle.p_list_current->_wind_done = false;
                wf_handle.p_list_current             = wf_handle.p_list_current->next;
                if (wf_handle.p_list_current == NULL)
                {
                    /// At this point the winding is done
                    wu_state = WF_STATE_WINDED;
                }
            }
        }
    }
    else if (wf_handle.dir == WF_DIR_UNWIND)
    {
        if ((wf_handle.p_list_current->config_unwind.level_safe <= wf_handle.p_list_current->level) && (wf_handle.has_failed == true))
        {
            WF_LOGI("Go back to wind\n");
            wf_handle.has_failed = false;
            wf_handle.dir        = WF_DIR_WIND;
            return;
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
                return;
            }
            wf_handle.p_list_previous = wf_handle.p_list_current;
            wf_handle.p_list_current  = wf_handle.p_list_current->previous;
            if (wf_handle.p_list_current != NULL)
            {
                free(wf_handle.p_list_current->next); /// When we go back free is needed
                wf_handle.p_list_current->next = NULL;
            }
            else
            {
                free(wf_handle.p_list_previous); /// When we go back free is needed
                wf_handle.p_list_previous = NULL;
                wu_state                  = WF_STATE_UNWINDED;
            }
            wf_handle.level_current = (wf_handle.p_list_current == NULL) ? WF_LEVEL_NULL : wf_handle.level_current; /// Check if bottom is reached and set level to NULL
        }
        else
        {
            WF_LOGI("doesn't contain deinit function or at end\n");
        }
    }

    WF_CB_LEVEL_CHANGED(wu_state);
}