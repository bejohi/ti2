#ifndef SIMUL_OS_H
#define SIMUL_OS_H
#include <stdint.h>

/**
 * @brief   Process ID within Simul-OS
 */
typedef int32_t so_pid_t;

/**
 * @brief   Enum over all different scheduling priorities supported by SimulOS
 */
enum so_prio {
	SO_PRIO_NORMAL,             /**< Normal priority */
	SO_PRIO_HIGH,               /**< High priority */
};

/**
 * @brief   Simulate an syscall that does I/O on the given file descriptor
 * @retval  0       Success
 * @retval  -1      Failure
 */
int so_io(void);

/**
 * @brief   Create a simulated process.
 * @warning The "process" will actually be a thread within the very same
 *          virtual address space :-(
 * @param name      Name of the process to created
 * @param program   Function implementing the process to run
 * @param prio      Priority of the process to create
 * @return  The ID of the simulated "process"
 * @retval  -1      Failed to create the process
 */
so_pid_t so_create(const char *name, int (*program)(void), enum so_prio prio);

/**
 * @brief   Get the callers Process ID
 * @return  The Process ID (PID) of the calling simulated "process"
 */
so_pid_t so_getpid(void);

/**
 * @brief   Waits for an simulated child process to exit and gets the exit code
 * @param   code    If not `NULL`, the exit code will be stored here
 * @return  The process ID of the child exited
 */
so_pid_t so_wait(int *code);

/**
 * @brief   Give other simulated processes a chance to run
 * @details The calling/running processes will be enqueued to the ready queue
 *          and the scheduler will be run
 */
void so_yield(void);

/**
 * @brief   The init process
 */
int init(void);

#endif /*SIMUL_OS_H */
