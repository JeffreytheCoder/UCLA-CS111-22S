#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef uint32_t u32;
typedef int32_t i32;

struct process
{
  u32 pid;
  u32 arrival_time;
  u32 burst_time;

  TAILQ_ENTRY(process)
  pointers;

  /* Additional fields here */
  u32 remaining_time;
  u32 quantum_time;
  /* End of "Additional fields here" */
};

TAILQ_HEAD(process_list, process);

u32 next_int(const char **data, const char *data_end)
{
  u32 current = 0;
  bool started = false;
  while (*data != data_end)
  {
    char c = **data;

    if (c < 0x30 || c > 0x39)
    {
      if (started)
      {
        return current;
      }
    }
    else
    {
      if (!started)
      {
        current = (c - 0x30);
        started = true;
      }
      else
      {
        current *= 10;
        current += (c - 0x30);
      }
    }

    ++(*data);
  }

  printf("Reached end of file while looking for another integer\n");
  exit(EINVAL);
}

u32 next_int_from_c_str(const char *data)
{
  char c;
  u32 i = 0;
  u32 current = 0;
  bool started = false;
  while ((c = data[i++]))
  {
    if (c < 0x30 || c > 0x39)
    {
      exit(EINVAL);
    }
    if (!started)
    {
      current = (c - 0x30);
      started = true;
    }
    else
    {
      current *= 10;
      current += (c - 0x30);
    }
  }
  return current;
}

void init_processes(const char *path,
                    struct process **process_data,
                    u32 *process_size)
{
  int fd = open(path, O_RDONLY);
  if (fd == -1)
  {
    int err = errno;
    perror("open");
    exit(err);
  }

  struct stat st;
  if (fstat(fd, &st) == -1)
  {
    int err = errno;
    perror("stat");
    exit(err);
  }

  u32 size = st.st_size;
  const char *data_start = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (data_start == MAP_FAILED)
  {
    int err = errno;
    perror("mmap");
    exit(err);
  }

  const char *data_end = data_start + size;
  const char *data = data_start;

  *process_size = next_int(&data, data_end);

  *process_data = calloc(sizeof(struct process), *process_size);
  if (*process_data == NULL)
  {
    int err = errno;
    perror("calloc");
    exit(err);
  }

  for (u32 i = 0; i < *process_size; ++i)
  {
    (*process_data)[i].pid = next_int(&data, data_end);
    (*process_data)[i].arrival_time = next_int(&data, data_end);
    (*process_data)[i].burst_time = next_int(&data, data_end);
  }

  munmap((void *)data, size);
  close(fd);
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    return EINVAL;
  }
  struct process *data;
  u32 size;
  init_processes(argv[1], &data, &size);

  u32 quantum_length = next_int_from_c_str(argv[2]);

  struct process_list list;
  TAILQ_INIT(&list);

  u32 total_waiting_time = 0;
  u32 total_response_time = 0;

  /* Your code here */
  // struct process *current;
  // TAILQ_FOREACH(current, &list, pointers)

  // create an array of time left for each process
  // int procs_time_left[size];
  // for (int i = 0; i < size; i++)
  // {
  //   procs_time_left[i] = data[i].burst_time;
  // }

  // init remaining time to burst time for each process
  struct process *proc;
  for (int i = 0; i < size; i++)
  {
    proc = &data[i];
    proc->remaining_time = proc->burst_time;
    proc->quantum_time = quantum_length;
  }

  // int proc_left = size;
  int cur_time = 0;
  // int cur_proc = 0;
  // int cur_elapse = 0;
  int finished = false;

  // add arrived process at time 0 to queue
  for (int i = 0; i < size; i++)
  {
    if (data[i].arrival_time == cur_time)
    {
      TAILQ_INSERT_TAIL(&list, &data[i], pointers);
      // printf("Added process %d\n", data[i].pid);
    }
  }

  while (!finished)
  {
    // printf("\nTime: %d\n", cur_time);
    // add arrived process at current time
    // for (int i = 0; i < size; i++)
    // {
    //   if (data[i].arrival_time == cur_time)
    //   {
    //     TAILQ_INSERT_TAIL(&list, &data[i], pointers);
    //     printf("Added process %d\n", data[i].pid);
    //   }
    // }

    // printf("Processes in queue: ");
    struct process *start_proc;
    TAILQ_FOREACH(start_proc, &list, pointers)
    {
      // printf("%d ", start_proc->pid);
    }
    // printf("\n");

    // pop the first process from the queue
    struct process *top_proc = TAILQ_FIRST(&list);
    // cur_proc = top_proc->pid - 1;

    // printf("Running process %d\n", top_proc->pid);

    // add to response time if the current process is runned the first time
    if (top_proc->remaining_time == top_proc->burst_time)
    {
      total_response_time += cur_time - (*top_proc).arrival_time;
      // printf("Process %d response time: %d\n", top_proc->pid, cur_time - (*top_proc).arrival_time);
    }

    // run the process for 1 time slice
    top_proc->remaining_time -= 1;
    top_proc->quantum_time -= 1;

    // if the current process is not finished
    // if (top_proc->remaining_time > 0)
    // {

    //   // if the time left for the current process is less than the quantum length
    //   // elapse the process left time and update total waiting and response time
    //   // add to queue again
    //   if (procs_time_left[cur_proc] <= quantum_length)
    //   {
    //     cur_time += procs_time_left[cur_proc];
    //     cur_elapse = procs_time_left[cur_proc];
    //     procs_time_left[cur_proc] = 0;
    //     // proc_left--;

    //     total_waiting_time += cur_time - (*top_proc).arrival_time - (*top_proc).burst_time;
    //     printf("Process %d waiting time: %d\n", cur_proc + 1, cur_time - (*top_proc).arrival_time - (*top_proc).burst_time);
    //   }
    //   // if the time left for the current process is greater than the quantum length
    //   // elapse the quantum length and decrease the time left for the current process
    //   else
    //   {
    //     cur_time += quantum_length;
    //     procs_time_left[cur_proc] -= quantum_length;
    //     cur_elapse = quantum_length;
    //   }
    // }

    // // go to next process
    // // if the current process is last process, go to the first one
    // if (cur_proc == size - 1)
    // {
    //   cur_proc = 0;
    // }
    // // if the next process has arrived, go to the next process
    // else if (data[cur_proc + 1].arrival_time <= cur_time)
    // {
    //   cur_proc++;
    // }
    // // if the next process has not arrived, go to the first process
    // else
    // {
    //   cur_proc = 0;
    // }

    // add arrived process to queue by ascending arrived time
    // for (int elapse = cur_time - cur_elapse; elapse <= cur_time; elapse++)
    // {
    //   for (int i = 0; i < size; i++)
    //   {
    //     if (data[i].pid != cur_proc + 1 && data[i].arrival_time == elapse)
    //     {
    //       TAILQ_INSERT_TAIL(&list, &data[i], pointers);
    //       printf("\nAdded process %d to queue\n", data[i].pid);
    //     }
    //   }
    // }

    // if current process is finished, add to waiting time
    // else, add to tail of queue
    // printf("Process %d left time: %d, quantum time left: %d\n", top_proc->pid, top_proc->remaining_time, top_proc->quantum_time);
    TAILQ_REMOVE(&list, top_proc, pointers);

    // check if all processes are finished
    finished = true;
    for (u32 i = 0; i < size; i++)
    {
      proc = &data[i];
      if (proc->remaining_time > 0)
      {
        finished = false;
        break;
      }
    }

    // printf("\nProcesses in queue: ");
    // struct process *traverse_proc;
    // TAILQ_FOREACH(traverse_proc, &list, pointers)
    // {
    //   printf("%d ", traverse_proc->pid);
    // }
    // printf("\n");

    // if (cur_time > 0)
    // {
    //   break;
    // }

    cur_time += 1;

    // add arrived process at current time
    for (int i = 0; i < size; i++)
    {
      if (data[i].arrival_time == cur_time)
      {
        TAILQ_INSERT_TAIL(&list, &data[i], pointers);
        // printf("Added process %d\n", data[i].pid);
      }
    }

    // if the current process is finished, add to waiting time
    if (top_proc->remaining_time == 0)
    {
      total_waiting_time += cur_time - top_proc->arrival_time - top_proc->burst_time;
      // printf("Process %d waiting time: %d\n", top_proc->pid, cur_time - top_proc->arrival_time - top_proc->burst_time);
    }
    // if the current process still has quantum time, add to head of queue
    else if (top_proc->quantum_time > 0)
    {
      TAILQ_INSERT_HEAD(&list, top_proc, pointers);
    }
    // if the current process used up quantum time, add to tail of queue
    else
    {
      top_proc->quantum_time = quantum_length;
      TAILQ_INSERT_TAIL(&list, top_proc, pointers);
    }
  }

  /* End of "Your code here" */

  printf("Average waiting time: %.2f\n", (float)total_waiting_time / (float)size);
  printf("Average response time: %.2f\n", (float)total_response_time / (float)size);

  free(data);
  return 0;
}
