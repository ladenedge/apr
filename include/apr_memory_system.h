/* strikerXXX: license goes here */

#ifndef APR_MEMORY_SYSTEM_H
#define APR_MEMORY_SYSTEM_H

/* strikerXXX: includes go here */
/* strikerXXX: HACK ALERT */
//#include "apr_stuff.h"
/* END OF HACK */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @package APR memory system
 */


/*
 * 
 */

typedef struct apr_memory_system_t apr_memory_system_t;

struct apr_memory_system_cleanup;

/**
 * The memory system structure
 */
struct apr_memory_system_t
{
  apr_memory_system_t  *parent_memory_system;
  apr_memory_system_t  *child_memory_system;
  apr_memory_system_t  *sibling_memory_system;
  apr_memory_system_t **ref_memory_system;
  apr_memory_system_t  *accounting_memory_system;

  struct apr_memory_system_cleanup *cleanups;

  void * (*malloc_fn)(apr_memory_system_t *memory_system, apr_size_t size);
  void * (*realloc_fn)(apr_memory_system_t *memory_system, void *memory, 
		       apr_size_t size);
  void (*free_fn)(apr_memory_system_t *memory_system, void *memory);
  void (*reset_fn)(apr_memory_system_t *memory_system);
  void (*pre_destroy_fn)(apr_memory_system_t *memory_system);
  void (*destroy_fn)(apr_memory_system_t *memory_system);
  void (*threadsafe_lock_fn)(apr_memory_system_t *memory_system);
  void (*threadsafe_unlock_fn)(apr_memory_system_t *memory_system);
};

/*
 * memory allocation functions 
 */

/**
 * Allocate a block of memory using a certain memory system
 * @param memory_system The memory system to use
 * @param size The (minimal required) size of the block to be allocated
 * @return pointer to a newly allocated block of memory, NULL if insufficient
 *         memory available
 * @deffunc void *apr_memory_system_malloc(apr_memory_system_t *memory_system,
 *					   apr_size_t size)
 */
APR_DECLARE(void *) 
apr_memory_system_malloc(apr_memory_system_t *memory_system, 
                         apr_size_t size);

/**
 * Change the size of a previously allocated block of memory
 * @param memory_system The memory system to use (should be the same as the
 *        one that returned the block)
 * @param mem Pointer to the previously allocated block. If NULL, this
 *        function acts like apr_memory_system_malloc.
 * @param size The (minimal required) size of the block to be allocated
 * @return pointer to a newly allocated block of memory, NULL if insufficient
 *         memory available
 * @deffunc void *apr_memory_system_realloc(apr_memory_system_t *memory_system,
 *					    void *mem, apr_size_t size)
 */
APR_DECLARE(void *) 
apr_memory_system_realloc(apr_memory_system_t *memory_system, 
                          void *mem,
                          apr_size_t size);

/**
 * Free a block of memory
 * @param memory_system The memory system to use (should be the same as the
 *        one that returned the block)
G * @param mem The block of memory to be freed
 * @deffunc void apr_memory_system_free(apr_memory_system_t *memory_system,
 *					void *mem)
 */
APR_DECLARE(void)
apr_memory_system_free(apr_memory_system_t *memory_system,
                       void *mem);

/*
 * memory system functions
 */

/**
 * Create a memory system (actually it initialized a memory system structure)
 * @caution Call this function as soon as you have obtained a block of memory
 *          to serve as a memory system structure from your 
 *          apr_xxx_memory_system_create. Only use this function when you are
 *          implementing a memory system.
 * @param memory The memory to turn into a memory system
 * @warning The memory passed in should be at least of size 
 *          sizeof(apr_memory_system_t)
 * @param parent_memory_system The parent memory system
 * @return The freshly initialized memory system
 * @deffunc apr_memory_system_t *apr_memory_system_create(void *memory,
 *				   apr_memory_system_t *parent_memory_system)
 */
APR_DECLARE(apr_memory_system_t *)
apr_memory_system_create(void *memory, 
                         apr_memory_system_t *parent_memory_system);

/**
 * Check if a memory system is obeying all rules. 
 * @caution Call this function as the last statement before returning a new
 *          memory system from your apr_xxx_memory_system_create.
 * @deffunc void apr_memory_system_validate(apr_memory_system_t *memory_system)
 */
#ifdef APR_MEMORY_SYSTEM_DEBUG
APR_DECLARE(void) 
apr_memory_system_assert(apr_memory_system_t *memory_system);
#else
#ifdef apr_memory_system_assert
#undef apr_memory_system_assert
#endif
#define apr_memory_system_assert(memory_system)
#endif /* APR_MEMORY_SYSTEM_DEBUG */

/**
 * Reset a memory system so it can be reused. 
 * This will also run all cleanup functions associated with the memory system
 * @warning This function will fail if there is no reset function available
 *          for the given memory system (i.e. the memory system is non-
 *          tracking).
 * @param memory_system The memory system to be reset
 * @deffunc void apr_memory_system_reset(apr_memory_system_t *memory_system)
 */
APR_DECLARE(void)
apr_memory_system_reset(apr_memory_system_t *memory_system);

/**
 * Destroy a memory system, effectively freeing all of its memory, and itself. 
 * This will also run all cleanup functions associated with the memory system.
 * @caution Be carefull when using this function with a non-tracking memory
 *          system
 * @param memory_system The memory system to be destroyed
 * @deffunc void apr_memory_system_destroy(apr_memory_system_t *memory_system)
 */
APR_DECLARE(void)
apr_memory_system_destroy(apr_memory_system_t *memory_system);

/**
 * Perform thread-safe locking required whilst this memory system is modified
 * @param memory_system The memory system to be locked for thread-safety
 * @deffunc void apr_memory_system_threadsafe_lock(apr_memory_system_t *memory_system)
 */
APR_DECLARE(void)
apr_memory_system_threadsafe_lock(apr_memory_system_t *memory_system);

/**
 * Release thread-safe locking required whilst this memory system was
 * being modified
 * @param memory_system The memory system to be released from thread-safety
 * @deffunc void apr_memory_system_threadsafe_unlock(apr_memory_system_t *memory_system)
 */
APR_DECLARE(void)
apr_memory_system_threadsafe_unlock(apr_memory_system_t *memory_system);

/**
 * Determine if memory system a is an ancestor of memory system b
 * @param a The memory system to search
 * @param b The memory system to search for
 * @return TRUE if a is an ancestor of b, FALSE if a is not an ancestor of b
 * @deffunc apr_bool_t apr_memory_system_is_ancestor(apr_memory_system_t *a,
 *						     apr_memory_system_t *b)
 */
APR_DECLARE(apr_status_t) 
apr_memory_system_is_ancestor(apr_memory_system_t *a, apr_memory_system_t *b);

/*
 * memory system cleanup management functions
 */

/**
 * Register a function to be called when a memory system is reset or destroyed
 * @param memory_system The memory system to register the cleanup function with
 * @param data The data to pass to the cleanup function
 * @param cleanup_fn The function to call when the memory system is reset or
 *        destroyed
 * @deffunc void apr_memory_system_cleanup_register(apr_memory_system_t *memory_system,
 *		   void *data, apr_status_t (*cleanup_fn)(void *));
 */
APR_DECLARE(apr_status_t) 
apr_memory_system_cleanup_register(apr_memory_system_t *memory_system, 
                                   void *data, 
                                   apr_status_t (*cleanup_fn)(void *));

/**
 * Unregister a previously registered cleanup function
 * @param memory_system The memory system the cleanup function is registered
 *        with
 * @param data The data associated with the cleanup function
 * @param cleanup_fn The registered cleanup function
 * @deffunc void apr_memory_system_cleanup_unregister(apr_memory_system_t *memory_system,
 *		   void *data, apr_status_t (*cleanup_fn)(void *));
 */
APR_DECLARE(apr_status_t)
apr_memory_system_cleanup_unregister(apr_memory_system_t *memory_system,
                                     void *data,
                                     apr_status_t (*cleanup)(void *));

/**
 * Run the specified cleanup function immediately and unregister it
 * @param memory_system The memory system the cleanup function is registered
 *        with
 * @param data The data associated with the cleanup function
 * @param cleanup The registered cleanup function
 * @deffunc apr_status_t apr_memory_system_cleanup_run(apr_memory_system_t *memory_system,
 *			   void *data, apr_status_t (*cleanup)(void *));
 */
APR_DECLARE(apr_status_t) 
apr_memory_system_cleanup_run(apr_memory_system_t *memory_system,
                              void *data,
                              apr_status_t (*cleanup)(void *));

/**
 * Create a standard malloc/realloc/free memory system
 */
APR_DECLARE(apr_status_t)
apr_standard_memory_system_create(apr_memory_system_t **memory_system);


#ifdef __cplusplus
}
#endif

#endif /* !APR_MEMORY_SYSTEM_H */

