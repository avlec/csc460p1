/*
 * scheduler.h
 *
 *  Created on: 17-Feb-2011
 *      Author: nrqm
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <avr/io.h>

///Up to this many tasks can be run, in addition to the idle task
#define MAXTASKS	8

///A task callback function
typedef void (*task_cb)();

/**
 * Initialise the scheduler.  This should be called once in the setup routine.
 */
void Scheduler_Init();

/**
 * Start a task.
 * The function "task" will be called roughly every "period" milliseconds starting after "delay" milliseconds.
 * The scheduler does not guarantee that the task will run as soon as it can.  Tasks are executed until completion.
 * If a task misses its scheduled execution time then it simply executes as soon as possible.  Don't pass stupid
 * values (e.g. negatives) to the parameters.
 *
 * \param id The tasks ID number.  This must be between 0 and MAXTASKS (it is used as an array index).
 * \param delay The task will start after this many milliseconds.
 * \param period The task will repeat every "period" milliseconds.
 * \param task The callback function that the scheduler is to call.
 */
void Scheduler_StartTask(int16_t delay, int16_t period, task_cb task);

/**
 * Go through the task list and run any tasks that need to be run.  The main function should simply be this
 * function called as often as possible, plus any low-priority code that you want to run sporadically.
 */
uint32_t Scheduler_Dispatch();

#endif /* SCHEDULER_H_ */

// Tossed source file in header because lazy.

/*
 * scheduler.cpp
 *
 *  Created on: 17-Feb-2011
 *      Author: nrqm
 *      Based on code by Jacob Schwartz
 */

#include "Arduino.h"
#include <avr/interrupt.h>

typedef struct
{
  int32_t period;
  int32_t remaining_time;
  uint8_t is_running;
  task_cb callback;
} task_t;

task_t tasks[MAXTASKS];

uint32_t last_runtime;

void Scheduler_Init()
{
  last_runtime = millis();
}

void Scheduler_StartTask(int16_t delay, int16_t period, task_cb task)
{
  static uint8_t id = 0;
  if (id < MAXTASKS)
  {
    tasks[id].remaining_time = delay;
    tasks[id].period = period;
    tasks[id].is_running = 1;
    tasks[id].callback = task;
    id++;
  }
}

uint32_t Scheduler_Dispatch()
{
  uint8_t i;

  uint32_t now = millis();
  uint32_t elapsed = now - last_runtime;
  last_runtime = now;
  task_cb t = NULL;
  uint32_t idle_time = 0xFFFFFFFF;

  // update each task's remaining time, and identify the first ready task (if there is one).
  for (i = 0; i < MAXTASKS; i++)
  {
    if (tasks[i].is_running)
    {
      // update the task's remaining time
      tasks[i].remaining_time -= elapsed;
      if (tasks[i].remaining_time <= 0)
      {
        if (t == NULL)
        {
          // if this task is ready to run, and we haven't already selected a task to run,
          // select this one.
          t = tasks[i].callback;
          tasks[i].remaining_time += tasks[i].period;
        }
        idle_time = 0;
      }
      else
      {
        idle_time = min((uint32_t)tasks[i].remaining_time, idle_time);
      }
    }
  }
  if (t != NULL)
  {
    // If a task was selected to run, call its function.
    t();
  }
  return idle_time;
}
