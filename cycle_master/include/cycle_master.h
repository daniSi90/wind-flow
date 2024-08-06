/**
 ********************************************************************************
 * @file    cycle_master.h
 * @author  Danijel Sipos
 * @date    06.08.2024
 * @brief   Implementation of cycle_master
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

#ifndef _CYCLE_MASTER_H_
#define _CYCLE_MASTER_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CM_LIST_RETRIES_UNLIMITED 0xFF

#define CM_CYCLE_RETRY_COUNTS_DEFAULT  3 /**< Default number of retries for cycling */
#define CM_UNWIND_RETRY_COUNTS_DEFAULT 3 /**< Default number of retries for unwinding */

#define CM_NEXT_CONFIG_CYCLE_DEFAULT(_fp, _p_list)                    \
    do                                                                \
    {                                                                 \
        cm_config_t config           = {};                            \
        config.between_retries_delay = 0;                             \
        config.delay_ms              = 0;                             \
        config.fp_call               = _fp;                           \
        config.retries_max           = CM_CYCLE_RETRY_COUNTS_DEFAULT; \
        if (cm_list_add_cycle_config(_p_list, &config) != true)       \
        {                                                             \
            printf("Failed to add cycle init configuration\n");       \
            return false;                                             \
        }                                                             \
    } while (0);

#define CM_NEXT_CONFIG_UNWIND_DEFAULT(_fp, _p_list)                    \
    do                                                                 \
    {                                                                  \
        cm_config_t config           = {};                             \
        config.between_retries_delay = 0;                              \
        config.delay_ms              = 0;                              \
        config.fp_call               = _fp;                            \
        config.retries_max           = CM_UNWIND_RETRY_COUNTS_DEFAULT; \
        config.level_safe            = 0xff;                           \
        if (cm_list_add_unwind_config(_p_list, &config) != true)       \
        {                                                              \
            printf("Failed to add cycle init configuration\n");        \
            return false;                                              \
        }                                                              \
    } while (0);

typedef enum
{
    CM_DIR_CYCLE = 0,
    CM_DIR_UNWIND,
} cm_dir_t;

typedef struct cm_list_t cm_list_t;
typedef bool (*fp_call_t)(cm_list_t *, void *);

typedef struct
{
    uint8_t   retries_max;           /**< Number of retries */
    uint32_t  delay_ms;              /**< Delay in milliseconds */
    uint32_t  between_retries_delay; /**< Delay between retries */
    fp_call_t fp_call;               /**< Function pointer to be called */
    void     *p_args;                /**< Arguments to be passed to the function */
    uint8_t   _retries_cnt;          /**< Retries counter, for internal use only */
    uint8_t   level_safe;            /**< If function dont success unwind until this level */
} cm_config_t;

typedef struct cm_list_t
{
    cm_list_t  *previous;      /**< Pointer to the previous element in the list */
    cm_list_t  *next;          /**< Pointer to the next element in the list */
    cm_config_t config_cycle;  /**< Function parameters to be executed when a specific initialization is required */
    cm_config_t config_unwind; /**< Function parameters to be executed when a specific deinitialization is required */
    uint8_t     level;         /**< Level of the state machine */
} cm_list_t;

cm_list_t *cm_list_add_next(cm_list_t *p_list);
bool       cm_list_add_cycle_config(cm_list_t *p_list, cm_config_t *p_config);
bool       cm_list_add_unwind_config(cm_list_t *p_list, cm_config_t *p_config);
cm_list_t *cm_list_execute(void);

#ifdef __cplusplus
}
#endif

#endif /* _CYCLE_MASTER_H_ */