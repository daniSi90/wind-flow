/**
 ********************************************************************************
 * @file    cycle_master.c
 * @author  Danijel Sipos
 * @date    06.08.2024
 * @brief   Implementation of cycle_master
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2024 SIMARINE d.o.o. . All rights reserved.
 ********************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "cycle_master.h"

#if 0
#if 1
#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#else
#define LOG_LOCAL_LEVEL ESP_LOG_NONE
#endif
#include "esp_log.h"
#endif
/* Private define ------------------------------------------------------------*/
static const char *TAG = "conn";

/* Private macro -------------------------------------------------------------*/
#if 0
#define CM_LOGI(format, ...) ESP_LOGI(TAG, format, ##__VA_ARGS__)
#define CM_LOGE(format, ...) ESP_LOGE(TAG, format, ##__VA_ARGS__)
#define CM_LOGW(format, ...) ESP_LOGW(TAG, format, ##__VA_ARGS__)
#else
#define CM_LOGD(format, ...)
#define CM_LOGI(format, ...) printf(format, ##__VA_ARGS__)
#define CM_LOGE(format, ...) printf(format, ##__VA_ARGS__)
#define CM_LOGW(format, ...) printf(format, ##__VA_ARGS__)
#define CM_SLEEP_MS(ms)      // Sleep(ms)
#endif

/* Private variables ---------------------------------------------------------*/
static cm_dir_t   cm_dir     = CM_DIR_CYCLE; /**< Direction of the state machine */
static bool       has_failed = false;        /**< Flag to indicate if the state machine has failed */
static cm_list_t *p_list_current;
/* Private function prototypes -----------------------------------------------*/

/* Public functions ----------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
cm_list_t *
cm_list_add_next(cm_list_t *p_list)
{
    cm_list_t *p_list_next = malloc(sizeof(cm_list_t));
    if (p_list_next == NULL)
    {
        CM_LOGE("Failed to allocate memory for connectivity list\n");
        return p_list_next;
    }
    memset(p_list_next, 0, sizeof(cm_list_t));
    if (p_list != NULL)
    {
        p_list->next          = p_list_next;
        p_list_next->previous = p_list;
        p_list_next->level    = p_list->level + 1;
    }
    else
    {
        p_list_current = p_list_next; /// When input list is NULL this means its the begginning of the list
    }
    CM_LOGD("add next: level %d\n", p_list_next->level);

    return p_list_next;
}

bool
cm_list_add_cycle_config(cm_list_t *p_list, cm_config_t *p_config)
{
    if ((p_list == NULL) || (p_config == NULL))
    {
        CM_LOGE("Invalid parameters\n");
        return false;
    }
    memcpy(&p_list->config_cycle, p_config, sizeof(cm_config_t));

    return true;
}

bool
cm_list_add_unwind_config(cm_list_t *p_list, cm_config_t *p_config)
{
    if (p_list == NULL || p_config == NULL)
    {
        CM_LOGE("Invalid parameters\n");
        return false;
    }
    memcpy(&p_list->config_unwind, p_config, sizeof(cm_config_t));

    return true;
}

cm_list_t *
cm_list_execute(void)
{
    if (p_list_current == NULL)
    {
        CM_LOGI("List is empty\n");
        return p_list_current;
    }

    if (cm_dir == CM_DIR_CYCLE)
    {
        if (p_list_current->config_cycle.fp_call != NULL)
        {
            if (p_list_current->config_cycle.fp_call(p_list_current, p_list_current->config_cycle.p_args) != true)
            {
                CM_LOGE("Failed to execute cycle function\n");
                if (p_list_current->config_cycle._retries_cnt++ >= p_list_current->config_cycle.retries_max)
                {
                    CM_LOGE("Max retries reached\n");
                    p_list_current->config_cycle._retries_cnt = 0;
                    cm_dir                                    = CM_DIR_UNWIND;
                    has_failed                                = true;
                }
                CM_SLEEP_MS(p_list_current->config_cycle.delay_ms);
                return p_list_current;
            }
            p_list_current = p_list_current->next;
        }
        else
        {
            CM_LOGI("END OF LIST\n");
        }
    }
    else if (cm_dir == CM_DIR_UNWIND)
    {
        // if ((p_list_current->config_unwind.level_safe <= p_list_current->level) && (has_failed == true))
        // {
        //     has_failed = false;
        //     cm_dir     = CM_DIR_CYCLE;
        // }
        if (p_list_current->config_unwind.fp_call != NULL)
        {
            if (p_list_current->config_unwind.fp_call(p_list_current, p_list_current->config_unwind.p_args) != true)
            {
                CM_LOGE("Failed to execute unwind function\n");
                if (p_list_current->config_cycle._retries_cnt++ >= p_list_current->config_cycle.retries_max)
                {
                    CM_LOGE("Max retries reached\n");
                    p_list_current->config_cycle._retries_cnt = 0;
                    cm_dir                                    = CM_DIR_CYCLE;
                    has_failed                                = true;
                }
                return p_list_current;
            }
        }
        else
        {
            CM_LOGI("Doesnt contain deinit function or at end\n");
        }
    }

    return p_list_current;
}