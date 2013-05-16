#ifndef MM_H_
#define MM_H_

#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#ifdef DEBUG_MEM
extern void * debug_mm_malloc (const char * file, const char * function, int line, size_t size);
extern void * debug_mm_calloc (const char * file, const char * function, int line, size_t count, size_t size);
extern void * debug_mm_realloc(const char * file, const char * function, int line, void * ptr, size_t size);
extern void   debug_mm_free   (const char * file, const char * function, int line, void * ptr);

#define mm_malloc(S)    debug_mm_malloc (__FILE__, __FUNCTION__, __LINE__, S)
#define mm_calloc(C,S)  debug_mm_calloc (__FILE__, __FUNCTION__, __LINE__, C, S)
#define mm_realloc(P,S) debug_mm_realloc(__FILE__, __FUNCTION__, __LINE__, P, S)
#define mm_free(P)      debug_mm_free   (__FILE__, __FUNCTION__, __LINE__, P)

#ifndef MM_USE_SYSTEM_MEMORY_FUNCTIONS
#define malloc(S)    mm_malloc(S)
#define calloc(C,S)  mm_calloc(C,S)
#define realloc(P,S) mm_realloc(P,S)
#define free(P)      mm_free(P)
#endif /*MM_USE_SYSTEM_MEMORY_FUNCTIONS*/

#else /*DEBUG_MEM*/
#define mm_malloc(S)       malloc(S)
#define mm_calloc(C,S)     calloc(C,S)
#define mm_realloc(P,S)    realloc(P,S)
#define mm_free(P)         free(P)
#endif

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* MM_H_ */
