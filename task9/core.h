#ifndef CORE_H
#define CORE_H
#include <pthread.h>

#include "simul_os.h"

/**
 * @brief   Number of processes that will fit into the process table
 */
#define MAX_PROCESSES                   8

/**
 * @brief   An enum over the possible process states
 */
enum so_state {
	STATE_NEW,
	STATE_READY,
	STATE_RUNNING,
	STATE_BLOCKED,
	STATE_ZOMBIE,
	STATE_EXIT,
};

/**
 * @brief   Process Control Block
 */
struct so_pcb {
	struct so_pcb *next;       /**< Pointer to the next process in the queue */
	struct so_pcb *prev;       /**< Pointer to the previous process */
	struct so_pcb *parent;     /**< Parent process or NULL */
	struct so_pcb *dead_childs;/**< List of childs that have exited */
	int (*program)(void);      /**< Function to run as simulated process */
	pthread_t thread;          /**< Thread used to simulate the process */
	so_pid_t id;               /**< ID of this thread */
	enum so_state state;       /**< State of this process */
	int exit_code;             /**< Exit code of the process, if exited */
	char name[64];             /**< Name of the process */
};

struct sched_q {
	struct so_pcb *head;       /**< Pointer to the head of the queue */
	struct so_pcb *tail;       /**< Pointer to the tail of the queue */
};

/**
 * @brief   Queue of ready processes
 */
extern struct sched_q sched_ready_q;

/**
 * @brief   Queue of processes blocked by a non I/O system call
 */
extern struct sched_q sched_blocked_q;

/**
 * @brief   Queue of processes blocked for I/O
 */
extern struct sched_q sched_io_q;

/**
 * @brief   The currently running process
 */
extern struct so_pcb *sched_running;

/**
 * @brief   The "process" simulated by the running posix thread
 */
extern _Thread_local struct so_pcb *sched_me;

/**
 * @brief   Mutex to protect concurrent accesses to core data structures
 */
extern pthread_mutex_t core_mutex;

/**
 * @brief   Condition to wait for I/O tasks being enqueued
 */
extern pthread_cond_t cond_io;

/**
 * @brief   Simul-OS's process table
 */
extern struct so_pcb process_table[MAX_PROCESSES];

/**
 * @brief   Process ID of the most recently created process
 * @details A value of 0 means no process does exist yet
 */
extern so_pid_t last_pid;

/**
 * @brief   Initialize the scheduler
 * @details This function must be called at "boot" of Simul OS
 */
void sched_init(void);

/**
 * @brief   Initialize the I/O simulator
 * @details This function must be called at "boot" of Simul OS
 */
void io_init(void);

/**
 * @brief   Runs the scheduler
 */
void schedule(void);

/**
 * @brief   Blocks the calling thread until the matching simulated process
 *          is scheduled
 * @warning The core mutex must be obtained prior to call this and is still
 *          owned by the calling thread when this function returns
 */
void sched_wait_to_be_scheduled(void);

/**
 * @brief   Removes and returns the first item the given queue
 * @param   queue   Queue to remove the first item from
 * @return  The item that was previously at the head of the queue
 * @retval  NULL    The queue was emtpy
 */
struct so_pcb *sched_pop_head(struct sched_q *queue);

/**
 * @brief   Removes and returns the last item the given queue
 * @param   queue   Queue to remove the last item from
 * @return  The item that was previously at the tail of the queue
 * @retval  NULL    The queue was emtpy
 */
struct so_pcb *sched_pop_tail(struct sched_q *queue);

/**
 * @brief   Enqueues the given process to the given queue
 * @param   queue   Queue to enqueue the given process to
 * @param   process Process to enqueue
 */
void sched_enqueue(struct sched_q *queue, struct so_pcb *process);

/**
 * @brief   Remove the given process from the given queue
 * @param   queue   Queue to remove the given process from
 * @param   process Process to remove from que
 * @pre     @p process is enqueued in @p queue
 * @post    @p process is no longer enqueued in @p queue
 */
void sched_remove_from_queue(struct sched_q *queue, struct so_pcb *process);

/**
 * @brief   Blocks the calling thread until the simulated process is scheduled
 */
void sched_wait_to_be_run(void);

/**
 * @brief   Prints the current scheduler state (including all Queues)
 */
void print_scheduler_state(void);

/**
 * @brief   Checks if all scheduling queues are empty
 * @retval  0       At least on queue still contains processes
 * @retval  1       No remaining processes to run
 */
int sched_are_all_queues_empty(void);

/**
 * @brief   Function that selects the next process to run
 * @return  One process from @ref sched_ready_q
 * @retval  NULL    No process in @ref sched_ready_q
 * @post    The returned process was removed from the ready queue
 * @warning This function must not return NULL unless the ready queue is empty
 */
struct so_pcb *sched_select_next(void);

/**
 * @brief   Function that implements the scheduling strategy
 * @details This function should call @ref schedule as required for the
 *          scheduling strategy, but at least once
 */
void sched_algo(void);
#endif /* CORE_H */
