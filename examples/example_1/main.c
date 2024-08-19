#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
//#include <windows.h>
#include <sys/select.h>
#include <unistd.h>
#include "wind_flow.h"

static bool function_start(void);
static bool function_0_init(wf_list_t *list, void *p_args);
static bool function_0_deinit(wf_list_t *list, void *p_args);
static bool function_1_init(wf_list_t *list, void *p_args);
static bool function_1_deinit(wf_list_t *list, void *p_args);
static bool function_2_init(wf_list_t *list, void *p_args);
static bool function_2_deinit(wf_list_t *list, void *p_args);
static bool function_3_init(wf_list_t *list, void *p_args);
static bool function_3_deinit(wf_list_t *list, void *p_args);
static bool function_4_init(wf_list_t *list, void *p_args);
static bool function_4_deinit(wf_list_t *list, void *p_args);

bool
main(void)
{
    char           input[100];
    struct timeval timeout;
    fd_set         readfds;

    function_start();

    uint8_t cnt = 0;
    while (1)
    {
        if (wf_list_execute() == NULL)
        {
            return true;
        }
        //Sleep(50);

        // Initialize the file descriptor set
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);

        // Set timeout duration (e.g., 10 seconds)
        timeout.tv_sec  = 1;
        timeout.tv_usec = 0;

        fflush(stdout);

        int result = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);

        if (result > 0)
        {
            if (FD_ISSET(STDIN_FILENO, &readfds))
            {
                // Read input from user
                if (scanf("%99s", input) == 1)
                {
                    // Compare input to known commands
                    if (strcmp(input, "q") == 0)
                    {
                        printf("Exiting...\n");
                        break;
                    }
                    else if (strcmp(input, "d2") == 0)
                    {
                        printf("fn2 set to done\n");
                        wf_list_event_done(2);
                    }
                    else if (strcmp(input, "command3") == 0)
                    {
                        // command_three();
                    }
                    else
                    {
                        // unknown_command();
                    }
                }
            }
        }
    }
    return false;
}

static bool
function_start(void)
{
    printf("> START\n");
    wf_list_t *p_list = wf_list_add_next(NULL);
    if (p_list == NULL)
    {
        return false;
    }
    WF_NEXT_CONFIG_CYCLE_DEFAULT(p_list, function_0_init);
    WF_NEXT_CONFIG_UNWIND_DEFAULT(p_list, function_0_deinit);
}

static bool
function_0_init(wf_list_t *list, void *p_args)
{
    printf("> function_0_init\n");

    wf_list_t *p_list = wf_list_add_next(list);
    if (p_list == NULL)
    {
        return false;
    }
    WF_NEXT_CONFIG_CYCLE_DEFAULT(p_list, function_1_init);
    WF_NEXT_CONFIG_UNWIND_DEFAULT(p_list, function_1_deinit);

    return true;
}

static bool
function_0_deinit(wf_list_t *list, void *p_args)
{
    printf("> function_0_deinit\n");

    return true;
}

static bool
function_1_init(wf_list_t *list, void *p_args)
{
    printf("> function_1_init\n");

    wf_list_t *p_list = wf_list_add_next(list);
    if (p_list == NULL)
    {
        return false;
    }
    WF_NEXT_CONFIG_CYCLE_DEFAULT(p_list, function_2_init);
    WF_NEXT_CONFIG_UNWIND_DEFAULT(p_list, function_2_deinit);
    WF_NEXT_WAIT_FOR_EVENT(p_list);

    return true;
}

static bool
function_1_deinit(wf_list_t *list, void *p_args)
{
    printf("> function_1_deinit\n");
    return true;
}

static bool
function_2_init(wf_list_t *list, void *p_args)
{
    printf("> function_2_init\n");

    wf_list_t *p_list = wf_list_add_next(list);
    if (p_list == NULL)
    {
        return false;
    }
    WF_NEXT_CONFIG_CYCLE_DEFAULT(p_list, function_3_init);
    WF_NEXT_CONFIG_UNWIND_DEFAULT(p_list, function_3_deinit);

    return true;
}
static bool
function_2_deinit(wf_list_t *list, void *p_args)
{
    printf("> function_2_deinit\n");
    return true;
}

uint8_t cnt_config3 = 0;
static bool
function_3_init(wf_list_t *list, void *p_args)
{
    printf("> function_3_init\n");

    if (cnt_config3++ < 5)
    {
        return false;
    }

    wf_list_t *p_list = wf_list_add_next(list);
    if (p_list == NULL)
    {
        return false;
    }
    WF_NEXT_CONFIG_CYCLE_DEFAULT(p_list, function_4_init);
    WF_NEXT_CONFIG_UNWIND_DEFAULT(p_list, function_4_deinit);

    return true;
}
static bool
function_3_deinit(wf_list_t *list, void *p_args)
{
    printf("> function_3_deinit\n");
    return true;
}

static bool
function_4_init(wf_list_t *list, void *p_args)
{
    printf("> function_4_init\n");
    return true;
}
static bool
function_4_deinit(wf_list_t *list, void *p_args)
{
    printf("> function_4_deinit\n");
    return true;
}