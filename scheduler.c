#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

int min(int x, int y);

void outprint(int time_x, int time_y, int pid, int arrival_time, int remaining_time);

void enqueue_new_process(int curr_time, Process* proc, int proc_num, LinkedQueue** ProcessQueue, int queue_num) {
    for (int i = 0; i < proc_num; ++i)
        if (curr_time == proc[i].arrival_time) {
            proc[i].service_time = ProcessQueue[queue_num - 1]->allotment_time;
            EnQueue(ProcessQueue[queue_num - 1], proc[i]);
        }
}

Process select_new_task(int curr_time, Process* proc, int proc_num, LinkedQueue** ProcessQueue, int queue_num) {
    for (int i = queue_num - 1; i >= 0; --i)
        if (!IsEmptyQueue(ProcessQueue[i])) {
            Process selected_new_task = DeQueue(ProcessQueue[i]);

            /* Info for CPU */
            // 1. Remaining task length ::= execution_time
            selected_new_task.execution_time = selected_new_task.execution_time;
            // 2. From which queue ::= turnaround_time
            selected_new_task.turnaround_time = i;
            // 3. When current time slice expires ::= completion_time
            selected_new_task.completion_time = selected_new_task.execution_time >= ProcessQueue[i]->time_slice ? curr_time + ProcessQueue[i]->time_slice : curr_time + selected_new_task.execution_time;
            // 4. Task start time ::= waiting_time
            selected_new_task.waiting_time = curr_time;

            return selected_new_task;
        }
    // Can't find new task
    return (Process) { .process_id = -1 };
}

Process min_proc(Process x, Process y) {
    return x.process_id < y.process_id ? x : y;
}

// Some arbitrary iterative merge sort, I guess......
void sort_process(Process* p, int num){
    Process* a = p;
    Process* b = (Process*)malloc(num * sizeof(Process));
    int seg, start;
    for (seg = 1; seg < num; seg += seg) {
        for (start = 0; start < num; start += seg + seg) {
            int low = start, mid = min(start + seg, num), high = min(start + seg + seg, num);
            int k = low;
            int start1 = low, end1 = mid;
            int start2 = mid, end2 = high;
            while (start1 < end1 && start2 < end2){
                Process minproc = min_proc(a[start1], a[start2]);
                if (minproc.process_id == a[start1].process_id)
                    b[k++] = a[start1++];
                else 
                    b[k++] = a[start2++];
            }
            while (start1 < end1)
                b[k++] = a[start1++];
            while (start2 < end2)
                b[k++] = a[start2++];
        }
        Process* tmp = a;
        a = b;
        b = tmp;
    }
    if (a != p) {
        int i;
        for (i = 0; i < num; i++)
            b[i] = a[i];
        b = a;
    }
    free(b);
}

void periodical_boost(LinkedQueue** ProcessQueue, int queue_num) {
    // Brilliant implementation of Periodical Boost, read it
    int proc_num = 0;
    for (int i = 0; i < queue_num; ++i) proc_num += Length(ProcessQueue[i]);
    if (proc_num == 0) return;
    Process proc[proc_num];
    for (int i = 0, j = 0; i < proc_num && j < queue_num; ++j)
        while (!IsEmptyQueue(ProcessQueue[j])) {
            proc[i] = DeQueue(ProcessQueue[j]);
            ++i;
        }
    sort_process(proc, proc_num);
    for (int i = 0; i < proc_num; ++i) proc[i].service_time = ProcessQueue[queue_num - 1]->allotment_time;
    for (int i = 0; i < proc_num; ++i) ProcessQueue[queue_num - 1] = EnQueue(ProcessQueue[queue_num - 1], proc[i]);
}

void scheduler(Process* proc, LinkedQueue** ProcessQueue, int proc_num, int queue_num, int period){
    if (proc_num < 1 || queue_num < 1) return;

    /* Init */
    int init = 1;
    int curr_time = proc[0].arrival_time;
    enqueue_new_process(curr_time, proc, proc_num, ProcessQueue, queue_num);
    Process curr_task = select_new_task(curr_time, proc, proc_num, ProcessQueue, queue_num);

    /* Main */
    for (int no_of_process_left = proc_num; no_of_process_left > 0; ++curr_time) {
        int need_new_task = 0;
        // 1. Time slice exhausted
        if (curr_time == curr_task.completion_time) {
            // Report
            outprint(curr_task.waiting_time, curr_task.completion_time, curr_task.process_id, curr_task.arrival_time, curr_task.execution_time);

            // Enqueue old process
            if (curr_task.execution_time == 0) {
                --no_of_process_left;
                if (no_of_process_left == 0) break;
            }
            else if (curr_task.service_time == 0) {
                // Remaining allotment ::= service_time
                // ProcessQueue[0]->allotment_time ::= inf
                if (curr_task.turnaround_time == 0) ProcessQueue[0] = EnQueue(ProcessQueue[0], curr_task);
                else {
                    curr_task.service_time = ProcessQueue[curr_task.turnaround_time - 1]->allotment_time;
                    ProcessQueue[curr_task.turnaround_time - 1] = EnQueue(ProcessQueue[curr_task.turnaround_time - 1], curr_task);
                }
            }
            else ProcessQueue[curr_task.turnaround_time] = EnQueue(ProcessQueue[curr_task.turnaround_time], curr_task);

            need_new_task = 1;
        }

        // 2. Enqueue new process
        if (!init) enqueue_new_process(curr_time, proc, proc_num, ProcessQueue, queue_num);
        else init = 0;

        // 3. Periodical boost
        if (curr_time && curr_time % period == 0) {
            // Interrupt (if CPU still working)
            if (curr_time != curr_task.completion_time) {
                // The smallest time interval is 1
                if (curr_task.waiting_time != curr_time) outprint(curr_task.waiting_time, curr_time, curr_task.process_id, curr_task.arrival_time, curr_task.execution_time);
                ProcessQueue[0] = EnQueue(ProcessQueue[0], curr_task);
            }

            periodical_boost(ProcessQueue, queue_num);
            need_new_task = 1;
        }

        // 4. Task selection (if need new task)
        if (need_new_task) {
            curr_task = select_new_task(curr_time, proc, proc_num, ProcessQueue, queue_num);

            // Timeskip (if no new task yet)
            if (curr_task.process_id == -1) {
                int k;
                for (int i = 0; i < proc_num; ++i)
                    if (proc[i].arrival_time > curr_time) {
                        k = i;
                        break;
                    }
                init = 1;
                curr_time = proc[k].arrival_time;
                enqueue_new_process(curr_time, proc, proc_num, ProcessQueue, queue_num);
                curr_task = select_new_task(curr_time, proc, proc_num, ProcessQueue, queue_num);
                --curr_time;
                continue;
            }
        }

        // 5. Task execution
        --curr_task.execution_time;
        --curr_task.service_time;
    }
}

