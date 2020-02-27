#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED

#include "scheduler.h"

#include <assert.h>
#include <curses.h>
#include <ucontext.h>
#include <time.h>

#include "util.h"

// This is an upper limit on the number of tasks we can create.
#define MAX_TASKS 128

// This is the size of each task's stack memory
#define STACK_SIZE 65536

// This struct will hold the all the necessary information for each task
typedef struct task_info {
    // This field stores all the state required to switch back to this task
    ucontext_t context;

    // This field stores another context. This one is only used when the task
    // is exiting.
    ucontext_t exit_context;

    //   a. Keep track of this task's state.
    int state; //0 = ready, 1 = running, 2 = wait, 3 = done, 4 = sleeping

    //   b. If the task is sleeping, when should it wake up?
    time_t wake_time;

    //   c. If the task is waiting for another task, which task is it waiting for?
    task_t wait_for;

    //   d. Was the task blocked waiting for user input? Once you successfully
    //      read input, you will need to save it here so it can be returned.
    char read_input;

    // TODO: Add fields here so you can:

} task_info_t;

int current_task = 0; //< The handle of the currently-executing task
int num_tasks = 1;    //< The number of tasks created so far
task_info_t tasks[MAX_TASKS]; //< Information for every task

bool checkTasks() {
    for(int index = 0; index < num_tasks; index++)
    {
        if (tasks[index].state == 0 || tasks[index].state == 4)
        {
            return true;
        }
    }
    return false;
}


void scheduler() {
    int index = 0;

    if(!checkTasks())
    {
        setcontext(&tasks[0].context);
    }

    while(checkTasks())
    {
        if(tasks[index].state == 0)
        {
            int tmp = current_task;
            current_task = index;
            swapcontext(&tasks[tmp].context, &tasks[index].context);
        }else if(tasks[index].state == 2)
        {
            if(tasks[tasks[index].wait_for].state == 3) //wait for task is complete
            {
                int tmp = current_task;
                current_task = index;
                swapcontext(&tasks[tmp].context, &tasks[index].context);
            }
        }else if(tasks[index].state == 4)
        {
            time_t now = time(0);
            printf("made it\n");
            printf("%ld", now);
            printf("\nHuh?\n");
            if(tasks[index].wake_time - now <= 0)
            {
                printf("here?\n");
                int tmp = current_task;
                current_task = index;
                swapcontext(&tasks[tmp].context, &tasks[index].context);
            }
        }
        index++;
        if(index > num_tasks)
        {
          index = 0;
        }
    }
}


/**
 * This function will execute when a task's function returns. This allows you
 * to update scheduler states and start another task. This function is run
 * because of how the contexts are set up in the task_create function.
 */
void task_exit() {
    // TODO: Handle the end of a task's execution here
    // Tell the scheduler that the task has finished
    //called after a function completes
    //printf("hi6\n");
    tasks[current_task].state = 3;
    scheduler();
}

/**
 * Initialize the scheduler. Programs should call this before calling any other
 * functiosn in this file.
 */
void scheduler_init() {
    // TODO: Initialize the state of the scheduler
    int index = 0;

    // Set the task handle to this index, since task_t is just an int

    // We're going to make two contexts: one to run the task, and one that runs at the end of the task so we can clean up. Start with the second

    // First, duplicate the current context as a starting point
    getcontext(&tasks[index].exit_context);

    // Set up a stack for the exit context
    tasks[index].exit_context.uc_stack.ss_sp = malloc(STACK_SIZE);
    tasks[index].exit_context.uc_stack.ss_size = STACK_SIZE;

    // Set up a context to run when the task function returns. This should call task_exit.
    makecontext(&tasks[index].exit_context, task_exit, 0);

    // Now we start with the task's actual running context
    getcontext(&tasks[index].context);

    // Allocate a stack for the new task and add it to the context
    tasks[index].context.uc_stack.ss_sp = malloc(STACK_SIZE);
    tasks[index].context.uc_stack.ss_size = STACK_SIZE;

    // Now set the uc_link field, which sets things up so our task will go to the exit context when the task function finishes
    tasks[index].context.uc_link = &tasks[index].exit_context;

    // And finally, set up the context to execute the task function
    makecontext(&tasks[index].context, NULL, 0);
    tasks[index].state = 0;
}


/**
 * Create a new task and add it to the scheduler.
 *
 * \param handle  The handle for this task will be written to this location.
 * \param fn      The new task will run this function.
 */
void task_create(task_t* handle, task_fn_t fn) {
    // Claim an index for the new task
    int index = num_tasks;
    num_tasks++;

    // Set the task handle to this index, since task_t is just an int
    *handle = index;

    // We're going to make two contexts: one to run the task, and one that runs at the end of the task so we can clean up. Start with the second

    // First, duplicate the current context as a starting point
    getcontext(&tasks[index].exit_context);

    // Set up a stack for the exit context
    tasks[index].exit_context.uc_stack.ss_sp = malloc(STACK_SIZE);
    tasks[index].exit_context.uc_stack.ss_size = STACK_SIZE;

    // Set up a context to run when the task function returns. This should call task_exit.
    makecontext(&tasks[index].exit_context, task_exit, 0);

    // Now we start with the task's actual running context
    getcontext(&tasks[index].context);

    // Allocate a stack for the new task and add it to the context
    tasks[index].context.uc_stack.ss_sp = malloc(STACK_SIZE);
    tasks[index].context.uc_stack.ss_size = STACK_SIZE;

    // Now set the uc_link field, which sets things up so our task will go to the exit context when the task function finishes
    tasks[index].context.uc_link = &tasks[index].exit_context;

    // And finally, set up the context to execute the task function
    makecontext(&tasks[index].context, fn, 0);
    tasks[index].state = 0;
}

/**
 * Wait for a task to finish. If the task has not yet finished, the scheduler should
 * suspend this task and wake it up later when the task specified by handle has exited.
 *
 * \param handle  This is the handle produced by task_create
 */
void task_wait(task_t handle) {
    // TODO: Block this task until the specified task has exited.
    tasks[current_task].state = 2;
    tasks[current_task].wait_for = handle;
    scheduler();
}

/**
 * The currently-executing task should sleep for a specified time. If that time is larger
 * than zero, the scheduler should suspend this task and run a different task until at least
 * ms milliseconds have elapsed.
 *
 * \param ms  The number of milliseconds the task should sleep.
 */
void task_sleep(size_t ms) {
    // TODO: Block this task until the requested time has elapsed.
    // Hint: Record the time the task should wake up instead of the time left for it to sleep. The bookkeeping is easier this way.
    time_t now = time(NULL);
    tasks[current_task].wake_time = now + ms/1000;
    tasks[current_task].state = 4;
    scheduler();
}

/**
 * Read a character from user input. If no input is available, the task should
 * block until input becomes available. The scheduler should run a different
 * task while this task is blocked.
 *
 * \returns The read character code
 */
int task_readchar() {
    // TODO: Block this task until there is input available.
    // To check for input, call getch(). If it returns ERR, no input was available.
    // Otherwise, getch() will returns the character code that was read.
    return ERR;
}
