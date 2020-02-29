    #define _XOPEN_SOURCE
    #define _XOPEN_SOURCE_EXTENDED

    #include "scheduler.h"

    #include <assert.h>

    #include<stdio.h>
    #include <curses.h>
    #include <ucontext.h>
    #include <time.h>

    #include "util.h"

    // This is an upper limit on the number of tasks we can create.
    #define MAX_TASKS 128

    // This is the size of each task's stack memory
    #define STACK_SIZE 65536

    typedef enum 
    { 
      ACTIVE = 0,
      SLEEPING = 1, 
      COMPLETED = 2,
      READY = 3,
      WAITING = 4,
      NEEDSCHAR = 5
    } task_States_e; 

    // This struct will hold the all the necessary information for each task
    typedef struct task_info {
      // This field stores all the state required to switch back to this task
      ucontext_t context;
      
      // This field stores another context. This one is only used when the task
      // is exiting.
      ucontext_t exit_context;


      task_States_e taskState; 

      time_t sleepUntil;

      int waitingFor;
      


      // TODO: Add fields here so you can:
      //   a. Keep track of this task's state.
      //   b. If the task is sleeping, when should it wake up?
      //   c. If the task is waiting for another task, which task is it waiting for?
      //   d. Was the task blocked waiting for user input? Once you successfully
      //      read input, you will need to save it here so it can be returned.
    } task_info_t;

    int current_task = 0; //< The handle of the currently-executing task
    int num_tasks = 1;    //< The number of tasks created so far
    task_info_t tasks[MAX_TASKS]; //< Information for every task
    task_info_t readyTasks[MAX_TASKS];

    int c;

    ucontext_t mainContext; 



    bool checkTasks() {
      for(int i = 1; i < num_tasks; i++) {
        if(tasks[i].taskState == 3 || tasks[i].taskState == 1) {
          return true;
        }
      }
      return false;
    }

    void scheduler() {
      int index = current_task + 1;
      
      if(!checkTasks()) { 
        current_task = 0;
        setcontext(&tasks[0].context);
      }

      while (checkTasks())
      {
        if(tasks[index].taskState == 5) {
          c = getch();
          if(c != ERR) {
            tasks[index].taskState = 3;
          }
        }
        if(index == current_task && tasks[index].taskState == 0) {
          return;
        }
        if(tasks[index].taskState == 1 && tasks[index].sleepUntil <= time(0)) {
          if(index == current_task) {
            return;
          }
          int temp = current_task;
          current_task = index;
          tasks[index].taskState = 0;
          swapcontext(&tasks[temp].context, &tasks[index].context);
        } else if(tasks[index].taskState == 4 && tasks[tasks[index].waitingFor].taskState == 2) {
          int temp = current_task;
          current_task = index;
          tasks[index].taskState = 0;
          swapcontext(&tasks[temp].context, &tasks[index].context);
        } else if(tasks[index].taskState == 3) {
          if(current_task == index) return;
          int temp = current_task;
          current_task = index;
          tasks[index].taskState = 0;
          swapcontext(&tasks[temp].context, &tasks[index].context);
        }


        index++;
        if(index >= num_tasks) {
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
      tasks[current_task].taskState = 2;
      scheduler();
      // TODO: Handle the end of a task's execution here
    }


    /**
     * Initialize the scheduler. Programs should call this before calling any other
     * functiosn in this file.
     */
    void scheduler_init() {
      // TODO: Initialize the state of the scheduler 
      int index = 0;

      getcontext(&tasks[index].exit_context);
      
      // Set up a stack for the exit context
      tasks[index].exit_context.uc_stack.ss_sp = malloc(STACK_SIZE);
      tasks[index].exit_context.uc_stack.ss_size = STACK_SIZE;
      
      // Set up a context to run when the task function returns. This should call task_exit.
      makecontext(&tasks[index].exit_context, task_exit, 0);
      
      // Now we start with the task's actual running context
      getcontext(&tasks[index].context);
      tasks[index].context.uc_stack.ss_sp = malloc(STACK_SIZE);
      tasks[index].context.uc_stack.ss_size = STACK_SIZE;
      //makecontext(&tasks[index].context, NULL, 0);
      
      // Now set the uc_link field, which sets things up so our task will go to the exit context when the task function finishes
      tasks[index].context.uc_link = &tasks[index].exit_context;
      
      // And finally, set up the context to execute the task function
      makecontext(&tasks[index].context, NULL, 0);
      tasks[index].taskState = 3;
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
      tasks[index].context.uc_stack.ss_sp = malloc(STACK_SIZE);
      tasks[index].context.uc_stack.ss_size = STACK_SIZE;
      //makecontext(&tasks[index].context, fn, 0);
      
      // Now set the uc_link field, which sets things up so our task will go to the exit context when the task function finishes
      tasks[index].context.uc_link = &tasks[index].exit_context;
      
      // And finally, set up the context to execute the task function
      makecontext(&tasks[index].context, fn, 0);
      tasks[index].taskState = 3;
    }

    /**
     * Wait for a task to finish. If the task has not yet finished, the scheduler should
     * suspend this task and wake it up later when the task specified by handle has exited.
     *
     * \param handle  This is the handle produced by task_create
     */
    void task_wait(task_t handle) {
      if(tasks[handle].taskState == 0) return;

      tasks[current_task].waitingFor = handle;
      tasks[current_task].taskState = 4;
      scheduler();
      // TODO: Block this task until the specified task has exited.
    }

    /**
     * The currently-executing task should sleep for a specified time. If that time is larger
     * than zero, the scheduler should suspend this task and run a different task until at least
     * ms milliseconds have elapsed.
     * 
     * \param ms  The number of milliseconds the task should sleep.
     */
    void task_sleep(size_t ms) {
      time_t now = time(0);
      tasks[current_task].taskState = 1;
      time_t needed = now + ms/1000;
      tasks[current_task].sleepUntil = needed;
      scheduler();
      
      // TODO: Block this task until the requested time has elapsed.
      // Hint: Record the time the task should wake up instead of the time left for it to sleep. The bookkeeping is easier this way.
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

      c = ERR;
      tasks[current_task].taskState = 5; 
      if (c == ERR)
        scheduler();
      tasks[current_task].taskState = 3;
      return c;
    }
