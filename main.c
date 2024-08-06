#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <windows.h>
#include "cycle_master.h"

static bool function_0_init(cm_list_t *list, void *p_args);
static bool function_0_deinit(cm_list_t *list, void *p_args);
static bool function_1_init(cm_list_t *list, void *p_args);
static bool function_1_deinit(cm_list_t *list, void *p_args);
static bool function_2_init(cm_list_t *list, void *p_args);
static bool function_2_deinit(cm_list_t *list, void *p_args);
static bool function_3_init(cm_list_t *list, void *p_args);
static bool function_3_deinit(cm_list_t *list, void *p_args);
static bool function_4_init(cm_list_t *list, void *p_args);
static bool function_4_deinit(cm_list_t *list, void *p_args);

bool
main(void)
{
    cm_list_t *p_list = cm_list_add_next(NULL);
    if (p_list == NULL)
    {
        return false;
    }
    CM_NEXT_CONFIG_CYCLE_DEFAULT(function_0_init, p_list);
    CM_NEXT_CONFIG_UNWIND_DEFAULT(function_0_deinit, p_list);

    uint8_t cnt = 0;
    while (cnt++ < 30)
    {
        if (cm_list_execute() == NULL)
        {
            return true;
        }
        // printf("#############\n");
        Sleep(50);
    }
    return false;
}

static bool
function_0_init(cm_list_t *list, void *p_args)
{
    printf("> function_0_init\n");

    cm_list_t *p_list = cm_list_add_next(list);
    if (p_list == NULL)
    {
        return false;
    }
    CM_NEXT_CONFIG_CYCLE_DEFAULT(function_1_init, p_list);
    CM_NEXT_CONFIG_UNWIND_DEFAULT(function_1_deinit, p_list);

    return true;
}

static bool
function_0_deinit(cm_list_t *list, void *p_args)
{
    printf("> function_0_deinit\n");

    return true;
}

static bool
function_1_init(cm_list_t *list, void *p_args)
{
    printf("> function_1_init\n");

    cm_list_t *p_list = cm_list_add_next(list);
    if (p_list == NULL)
    {
        return false;
    }
    CM_NEXT_CONFIG_CYCLE_DEFAULT(function_2_init, p_list);
    CM_NEXT_CONFIG_UNWIND_DEFAULT(function_2_deinit, p_list);

    return true;
}

static bool
function_1_deinit(cm_list_t *list, void *p_args)
{
    printf("> function_1_deinit\n");
    return true;
}

static bool
function_2_init(cm_list_t *list, void *p_args)
{
    printf("> function_2_init\n");

    cm_list_t *p_list = cm_list_add_next(list);
    if (p_list == NULL)
    {
        return false;
    }
    CM_NEXT_CONFIG_CYCLE_DEFAULT(function_3_init, p_list);
    CM_NEXT_CONFIG_UNWIND_DEFAULT(function_3_deinit, p_list);

    return true;
}
static bool
function_2_deinit(cm_list_t *list, void *p_args)
{
    printf("> function_2_deinit\n");
    return true;
}

uint8_t cnt_config3 = 0;
static bool
function_3_init(cm_list_t *list, void *p_args)
{
    printf("> function_3_init\n");

    if (cnt_config3++ < 5)
    {
        return false;
    }

    cm_list_t *p_list = cm_list_add_next(list);
    if (p_list == NULL)
    {
        return false;
    }
    CM_NEXT_CONFIG_CYCLE_DEFAULT(function_4_init, p_list);
    CM_NEXT_CONFIG_UNWIND_DEFAULT(function_4_deinit, p_list);

    return true;
}
static bool
function_3_deinit(cm_list_t *list, void *p_args)
{
    printf("> function_3_deinit\n");
    return true;
}

static bool
function_4_init(cm_list_t *list, void *p_args)
{
    printf("> function_4_init\n");
    return true;
}
static bool
function_4_deinit(cm_list_t *list, void *p_args)
{
    printf("> function_4_deinit\n");
    return true;
}