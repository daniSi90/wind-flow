#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <windows.h>
#include "cycle_master.h"

static bool wifi_off(cm_list_t *list, void *p_args);
static bool wifi_on(cm_list_t *list, void *p_args);
static bool wifi_config1_init(cm_list_t *list, void *p_args);
static bool wifi_config1_deinit(cm_list_t *list, void *p_args);
static bool wifi_config2_init(cm_list_t *list, void *p_args);
static bool wifi_config2_deinit(cm_list_t *list, void *p_args);
static bool wifi_config3_init(cm_list_t *list, void *p_args);
static bool wifi_config3_deinit(cm_list_t *list, void *p_args);
static bool wifi_config4_init(cm_list_t *list, void *p_args);
static bool wifi_config4_deinit(cm_list_t *list, void *p_args);

void
main(void)
{
    cm_config_t config = {};
    cm_list_t  *p_list = cm_list_add_next(NULL);
    if (p_list == NULL)
    {
        printf("Failed to allocate memory for connectivity list\n");
        return;
    }
    config.between_retries_delay = 0;
    config.delay_ms              = 0;
    config.fp_call               = wifi_on;
    config.retries_max           = 3;
    if (cm_list_add_cycle_config(p_list, &config) != true)
    {
        printf("Failed to add cycle init configuration\n");
        return;
    }
    config.fp_call = wifi_off;
    if (cm_list_add_unwind_config(p_list, &config) != true)
    {
        printf("Failed to add cycle deinit configuration\n");
        return;
    }

    uint8_t cnt = 0;
    while (cnt++ < 30)
    {
        if (cm_list_execute() == NULL)
        {
            return;
        }
        // printf("#############\n");
        Sleep(50);
    }
}

static bool
wifi_off(cm_list_t *list, void *p_args)
{
    printf("> wifi_off\n");
    return true;
}

static bool
wifi_on(cm_list_t *list, void *p_args)
{
    printf("> wifi_on\n");

    cm_config_t config = {};
    cm_list_t  *p_list = cm_list_add_next(list);
    if (p_list == NULL)
    {
        printf("Failed to allocate memory for connectivity list\n");
        return false;
    }
    config.between_retries_delay = 0;
    config.delay_ms              = 0;
    config.fp_call               = wifi_config1_init;
    config.retries_max           = 3;
    if (cm_list_add_cycle_config(p_list, &config) != true)
    {
        printf("Failed to add cycle init configuration\n");
        return false;
    }
    config.fp_call = wifi_config1_deinit;
    if (cm_list_add_unwind_config(p_list, &config) != true)
    {
        printf("Failed to add cycle deinit configuration\n");
        return false;
    }

    return true;
}

static bool
wifi_config1_init(cm_list_t *list, void *p_args)
{
    printf("> wifi_config1_init\n");

    cm_config_t config = {};
    cm_list_t  *p_list = cm_list_add_next(list);
    if (p_list == NULL)
    {
        printf("Failed to allocate memory for connectivity list\n");
        return false;
    }
    config.between_retries_delay = 0;
    config.delay_ms              = 0;
    config.fp_call               = wifi_config2_init;
    config.retries_max           = 3;
    if (cm_list_add_cycle_config(p_list, &config) != true)
    {
        printf("Failed to add cycle init configuration\n");
        return false;
    }
    config.fp_call = wifi_config2_deinit;
    if (cm_list_add_unwind_config(p_list, &config) != true)
    {
        printf("Failed to add cycle deinit configuration\n");
        return false;
    }

    return true;
}

static bool
wifi_config1_deinit(cm_list_t *list, void *p_args)
{
    printf("> wifi_config1_deinit\n");
    return true;
}

uint8_t cnt_config2 = 0;
static bool
wifi_config2_init(cm_list_t *list, void *p_args)
{
    printf("> wifi_config2_init\n");

    if (cnt_config2++ < 2)
    {
        return false;
    }
    cm_config_t config = {};
    cm_list_t  *p_list = cm_list_add_next(list);
    if (p_list == NULL)
    {
        printf("Failed to allocate memory for connectivity list\n");
        return false;
    }
    config.between_retries_delay = 0;
    config.delay_ms              = 0;
    config.fp_call               = wifi_config3_init;
    config.retries_max           = 3;
    if (cm_list_add_cycle_config(p_list, &config) != true)
    {
        printf("Failed to add cycle init configuration\n");
        return false;
    }
    config.fp_call = wifi_config3_deinit;
    if (cm_list_add_unwind_config(p_list, &config) != true)
    {
        printf("Failed to add cycle deinit configuration\n");
        return false;
    }
    return true;
}
static bool
wifi_config2_deinit(cm_list_t *list, void *p_args)
{
    printf("> wifi_config2_deinit\n");
    return true;
}

static bool
wifi_config3_init(cm_list_t *list, void *p_args)
{
    printf("> wifi_config3_init\n");

    cm_config_t config = {};
    cm_list_t  *p_list = cm_list_add_next(list);
    if (p_list == NULL)
    {
        printf("Failed to allocate memory for connectivity list\n");
        return false;
    }
    config.between_retries_delay = 0;
    config.delay_ms              = 0;
    config.fp_call               = wifi_config4_init;
    config.retries_max           = 3;
    if (cm_list_add_cycle_config(p_list, &config) != true)
    {
        printf("Failed to add cycle init configuration\n");
        return false;
    }
    config.fp_call = wifi_config4_deinit;
    if (cm_list_add_unwind_config(p_list, &config) != true)
    {
        printf("Failed to add cycle deinit configuration\n");
        return false;
    }
    return true;
}
static bool
wifi_config3_deinit(cm_list_t *list, void *p_args)
{
    printf("> wifi_config3_deinit\n");
    return true;
}

static bool
wifi_config4_init(cm_list_t *list, void *p_args)
{
    printf("> wifi_config4_init\n");
    return true;
}
static bool
wifi_config4_deinit(cm_list_t *list, void *p_args)
{
    printf("> wifi_config4_deinit\n");
    return true;
}