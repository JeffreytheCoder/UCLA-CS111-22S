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

  // init remaining time to burst time for each process
  struct process *proc;
  for (int i = 0; i < size; i++)
  {
    proc = &data[i];
    proc->remaining_time = proc->burst_time;
    proc->quantum_time = quantum_length;
  }

  int cur_time = 0;
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
    // printf("Processes in queue: ");
    // struct process *start_proc;
    // TAILQ_FOREACH(start_proc, &list, pointers)
    // {
    //   printf("%d ", start_proc->pid);
    // }
    // printf("\n");

    // pop the first process from the queue
    struct process *top_proc = TAILQ_FIRST(&list);

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

    cur_time += 1;

    // add arrived process at next time
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
    // if the current process used up quantum time, reset quantum time and add to tail of queue
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
