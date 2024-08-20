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
static bool function_3a_init(wf_list_t *list, void *p_args);
static bool function_3a_deinit(wf_list_t *list, void *p_args);
static bool function_4a_init(wf_list_t *list, void *p_args);
static bool function_4a_deinit(wf_list_t *list, void *p_args);
static bool function_3b_init(wf_list_t *list, void *p_args);
static bool function_3b_deinit(wf_list_t *list, void *p_args);
static bool function_4b_init(wf_list_t *list, void *p_args);
static bool function_4b_deinit(wf_list_t *list, void *p_args);
static bool function_5b_init(wf_list_t *list, void *p_args);
static bool function_5b_deinit(wf_list_t *list, void *p_args);

static bool go_a_path = true;

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
                    else if (strcmp(input, "d") == 0)
                    {
                        printf("Event done...\n");
                        wf_list_event_done(2);
                    }
                    else if (strcmp(input, "w") == 0)
                    {
                        printf("Winding...\n");	
                        int8_t stat = wf_list_wind();
                        printf("Winding status: %d\n", stat);
                    }
                    else if (strcmp(input, "u") == 0)
                    {
                        printf("Unwinding...\n");	
                        int8_t stat = wf_list_unwind();
                        printf("Unwinding status: %d\n", stat);
                    }
                    else if (strcmp(input, "p") == 0)
                    {
                        go_a_path = !go_a_path;
                        printf("path - %c\n", go_a_path ? 'A' : 'B');
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
    if (go_a_path)
    {
        WF_NEXT_CONFIG_CYCLE_DEFAULT(p_list, function_3a_init);
        WF_NEXT_CONFIG_UNWIND_DEFAULT(p_list, function_3a_deinit);
    } else {
        WF_NEXT_CONFIG_CYCLE_DEFAULT(p_list, function_3b_init);
        WF_NEXT_CONFIG_UNWIND_DEFAULT(p_list, function_3b_deinit);
    }

    return true;
}
static bool
function_2_deinit(wf_list_t *list, void *p_args)
{
    printf("> function_2_deinit\n");
    return true;
}

static bool
function_3a_init(wf_list_t *list, void *p_args)
{
    printf("> function_3A_init\n");

    wf_list_t *p_list = wf_list_add_next(list);
    if (p_list == NULL)
    {
        return false;
    }
    WF_NEXT_CONFIG_CYCLE_DEFAULT(p_list, function_4a_init);
    WF_NEXT_CONFIG_UNWIND_DEFAULT(p_list, function_4a_deinit);

    return true;
}
static bool
function_3a_deinit(wf_list_t *list, void *p_args)
{
    printf("> function_3A_deinit\n");
    return true;
}

static bool
function_4a_init(wf_list_t *list, void *p_args)
{
    printf("> function_4A_init\n");
    return true;
}
static bool
function_4a_deinit(wf_list_t *list, void *p_args)
{
    printf("> function_4A_deinit\n");
    return true;
}

static bool
function_3b_init(wf_list_t *list, void *p_args)
{
    printf("> function_3B_init\n");

    wf_list_t *p_list = wf_list_add_next(list);
    if (p_list == NULL)
    {
        return false;
    }
    WF_NEXT_CONFIG_CYCLE_DEFAULT(p_list, function_4b_init);
    WF_NEXT_CONFIG_UNWIND_DEFAULT(p_list, function_4b_deinit);

    return true;
}
static bool
function_3b_deinit(wf_list_t *list, void *p_args)
{
    printf("> function_3B_deinit\n");
    return true;
}

static bool
function_4b_init(wf_list_t *list, void *p_args)
{
    printf("> function_4B_init\n");

    wf_list_t *p_list = wf_list_add_next(list);
    if (p_list == NULL)
    {
        return false;
    }
    WF_NEXT_CONFIG_CYCLE_DEFAULT(p_list, function_5b_init);
    WF_NEXT_CONFIG_UNWIND_DEFAULT(p_list, function_5b_deinit);
    return true;
}
static bool
function_4b_deinit(wf_list_t *list, void *p_args)
{
    printf("> function_4B_deinit\n");
    return true;
}

static bool
function_5b_init(wf_list_t *list, void *p_args)
{
    printf("> function_5B_init\n");

    return true;
}
static bool
function_5b_deinit(wf_list_t *list, void *p_args)
{
    printf("> function_5B_deinit\n");
    return true;
}