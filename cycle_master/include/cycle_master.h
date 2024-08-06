/**
 ********************************************************************************
 * @file    cycle_master.h
 * @author  Danijel Sipos
 * @date    06.08.2024
 * @brief   Implementation of cycle_master
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2024 SIMARINE d.o.o. . All rights reserved.
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