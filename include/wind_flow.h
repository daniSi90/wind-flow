/**
 ********************************************************************************
 * @file    wind_flow.h
 * @author  Danijel Sipos
 * @date    06.08.2024
 * @brief   Implementation of wind_flow
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

#ifndef _WIND_FLOW_H_
#define _WIND_FLOW_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WF_LIST_RETRIES_UNLIMITED 0xFF

#define WF_CYCLE_RETRY_COUNTS_DEFAULT  3 /**< Default number of retries for cycling */
#define WF_UNWIND_RETRY_COUNTS_DEFAULT 3 /**< Default number of retries for unwinding */

#define WF_NEXT_CONFIG_CYCLE_DEFAULT(_p_list, _fp)             \
    do                                                         \
    {                                                          \
        wf_config_t config   = {};                             \
        config.n_skip_period = 0;                              \
        config.fp_call       = _fp;                            \
        config.retries_max   = WF_CYCLE_RETRY_COUNTS_DEFAULT;  \
        if (wf_list_add_wind_config(_p_list, &config) != true) \
        {                                                      \
            printf("Failed to add wind init configuration\n"); \
            return false;                                      \
        }                                                      \
    } while (0);

#define WF_NEXT_CONFIG_UNWIND_DEFAULT(_p_list, _fp)              \
    do                                                           \
    {                                                            \
        wf_config_t config   = {};                               \
        config.n_skip_period = 0;                                \
        config.fp_call       = _fp;                              \
        config.retries_max   = WF_UNWIND_RETRY_COUNTS_DEFAULT;   \
        config.level_safe    = 0xff;                             \
        if (wf_list_add_unwind_config(_p_list, &config) != true) \
        {                                                        \
            printf("Failed to add wind init configuration\n");   \
            return false;                                        \
        }                                                        \
    } while (0);

#define WF_NEXT_WAIT_FOR_EVENT(_p_list) _p_list->event_wait = true;

typedef enum
{
    WF_DIR_UNDEFINED = 0, /**< Undefined direction */
    WF_DIR_WIND,          /**< Wind direction */
    WF_DIR_UNWIND,        /**< Unwind direction */
} wf_dir_t;

typedef struct wf_list_t wf_list_t;
typedef bool (*fp_call_t)(wf_list_t *, void *);
typedef bool (*fp_init_call_t)(void);

typedef struct
{
    fp_init_call_t fp_init; /**< Function pointer to be called */
    wf_dir_t       dir;
    wf_list_t     *p_list_current;
    wf_list_t     *p_list_previous;
    uint32_t       update_period; /**< Update period in milliseconds */
    uint32_t       retry_cnt;
    bool           has_failed;
    bool           wind;
    bool           is_winded; /**< If the state machine has unwided to the end */
    bool           unwind;
    int8_t         level_current; /**< Current level of the state machine */
    int8_t         level_set;     /**< Level to unwind to */
} wf_handle_t;

typedef struct
{
    uint8_t   retries_max;   /**< Number of retries */
    uint32_t  n_skip_period; /**< N skip periods (delay between retries) */
    fp_call_t fp_call;       /**< Function pointer to be called */
    void     *p_args;        /**< Arguments to be passed to the function */
    uint8_t   _retries_cnt;  /**< Retries counter, for internal use only */
    uint8_t   level_safe;    /**< If function dont success unwind until this level */
} wf_config_t;

typedef struct wf_list_t
{
    wf_list_t  *previous;      /**< Pointer to the previous element in the list */
    wf_list_t  *next;          /**< Pointer to the next element in the list */
    wf_config_t config_wind;   /**< Function parameters to be executed when a specific initialization is required */
    wf_config_t config_unwind; /**< Function parameters to be executed when a specific deinitialization is required */
    uint8_t     level;         /**< Level of the state machine */
    bool        event_wait;    /**< If set to true, the state machine will wait for the event to be set before executing the next configuration */
    bool        _wind_done;    /**< If set to true, the wind configuration has been executed */
    bool        _event_done;   /**< If set to true, the event has been set, internal use */
} wf_list_t;

wf_handle_t *wf_handle_init(fp_init_call_t fp_init);
wf_list_t   *wf_list_add_next(wf_list_t *p_list);
bool         wf_list_add_wind_config(wf_list_t *p_list, wf_config_t *p_config);
bool         wf_list_add_unwind_config(wf_list_t *p_list, wf_config_t *p_config);
bool         wf_list_event_done(uint8_t level);
bool         wf_is_busy(void);
int8_t       wf_list_wind(void);
int8_t       wf_list_unwind(void);
wf_list_t   *wf_list_execute(void);

#ifdef __cplusplus
}
#endif

#endif /* _WIND_FLOW_H_ */