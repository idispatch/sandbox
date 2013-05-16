#define MM_USE_SYSTEM_MEMORY_FUNCTIONS
#include "mm.h"

#ifdef DEBUG_MEM
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#pragma pack(push, 1)

static const char DEBUG_MEM_MARKER[8] = {0xAA,0xBB,0xCC,0xDD,0x55,0x77,0x88,0x99};
#define DEBUG_MEM_MARKER_SIZE sizeof(DEBUG_MEM_MARKER)

struct debug_mem_header {
    const char * file;
    const char * function;
    const char * context;
    int          line;
    size_t       size;
    char         marker[DEBUG_MEM_MARKER_SIZE];
};

struct debug_mem_footer {
    char         marker[DEBUG_MEM_MARKER_SIZE];
};

static void * debug_check_memory(const char * const file,
                                 const char * const function,
                                 int line,
                                 const char * const context,
                                 void const * const ptr) {
    if(ptr) {
        size_t size;
        const char * real_mem = (const char*)ptr - sizeof(struct debug_mem_header);
        struct debug_mem_header const * const header = (struct debug_mem_header const *)real_mem;

        // check header marker
        if(memcmp(header->marker, DEBUG_MEM_MARKER, DEBUG_MEM_MARKER_SIZE) != 0) {
            //TODO: log broken header
            // header context may contain garbage here, because header is broken
            abort();
        } else {
            size = header->size;
        }
        struct debug_mem_footer const * const footer = (struct debug_mem_footer const *)(real_mem +
                                                                                         sizeof(struct debug_mem_header) +
                                                                                         size);
        // check footer marker
        if(memcmp(footer->marker, DEBUG_MEM_MARKER, DEBUG_MEM_MARKER_SIZE) != 0) {
            // context is valid here, because header is checked being valid
            const size_t num_bytes = sizeof(struct debug_mem_header) + size +
                                     sizeof(struct debug_mem_footer);
            fprintf(stderr,
                    "Heap memory overrun before '%s' call: block of size=%lu at %p (debug size=%lu), allocated in %s:%s@%d by %s\n",
                    (context ? context : "N/A"),
                    size,
                    ptr,
                    num_bytes,
                    (header->file ? header->file : "N/A"),
                    (header->function ? header->function : "N/A"),
                    header->line,
                    (header->context ? header->context : "N/A"));
            abort();
        }

        return (void*)real_mem;
    }
    return NULL;
}

void * debug_mm_malloc(const char * file, const char * function, int line, size_t size) {
    const size_t num_bytes = sizeof(struct debug_mem_header) +
                             size +
                             sizeof(struct debug_mem_footer);
#if DEBUG_LOG_MEM
    fprintf(stderr, "mm: malloc %lu bytes (at %s@%d)\n", size, function, line);
#endif
    void * p = malloc(num_bytes);
    if(p) {
        struct debug_mem_header * header = (struct debug_mem_header *)p;
        struct debug_mem_footer * footer = (struct debug_mem_footer *)((char*)p +
                                                                       sizeof(struct debug_mem_header) +
                                                                       size);
        header->file = file;
        header->function = function;
        header->line = line;
        header->context = "malloc";
        header->size = size;
        memcpy(header->marker, DEBUG_MEM_MARKER, DEBUG_MEM_MARKER_SIZE);
        memcpy(footer->marker, DEBUG_MEM_MARKER, DEBUG_MEM_MARKER_SIZE);
    } else {
        fprintf(stderr,
                "Failed to malloc %lu bytes (debug size=%lu) in %s:%s@%d: %s (errno=%d)\n",
                size,
                num_bytes,
                (file ? file : "N/A"),
                (function ? function : "N/A"),
                line,
                strerror(errno),
                errno);
        abort();
    }
    p = ((char*)p + sizeof(struct debug_mem_header));
#if DEBUG_LOG_MEM
    fprintf(stderr, "mm: malloc %lu bytes at %p\n", size, p);
#endif
    return p;
}

void * debug_mm_calloc (const char * file, const char * function, int line, size_t count, size_t size) {
    const size_t num_bytes = sizeof(struct debug_mem_header) +
                             size * count +
                             sizeof(struct debug_mem_footer);
#if DEBUG_LOG_MEM
    fprintf(stderr, "mm: calloc %lu bytes (at %s@%d)\n", size, function, line);
#endif
    void * p = calloc(num_bytes, 1);
    if(p) {
        struct debug_mem_header * header = (struct debug_mem_header *)p;
        struct debug_mem_footer * footer = (struct debug_mem_footer *)((char*)p +
                                                                       sizeof(struct debug_mem_header) +
                                                                       size * count);
        header->file = file;
        header->function = function;
        header->line = line;
        header->context = "calloc";
        header->size = size;
        memcpy(header->marker, DEBUG_MEM_MARKER, DEBUG_MEM_MARKER_SIZE);
        memcpy(footer->marker, DEBUG_MEM_MARKER, DEBUG_MEM_MARKER_SIZE);
    } else {
        fprintf(stderr,
                "Failed to calloc %lu bytes (debug size=%lu) in %s:%s@%d: %s (errno=%d)\n",
                size,
                num_bytes,
                (file ? file : "N/A"),
                (function ? function : "N/A"),
                line,
                strerror(errno),
                errno);
        abort();
    }

    p = ((char*)p + sizeof(struct debug_mem_header));
#if DEBUG_LOG_MEM
    fprintf(stderr, "mm: calloc %lu bytes at %p\n", size, p);
#endif
    return p;
}

void * debug_mm_realloc(const char * file, const char * function, int line, void * ptr, size_t size) {
    void * p;
    const size_t num_bytes = size + sizeof(struct debug_mem_header) + sizeof(struct debug_mem_footer);
#if DEBUG_LOG_MEM
    fprintf(stderr, "mm: realloc %lu bytes (address=%p, at %s@%d)\n", size, ptr, function, line);
#endif
    if(ptr) {
        ptr = debug_check_memory(file, function, line, "realloc", ptr);
    }

    p = realloc(ptr, num_bytes);

    if(p) {
        struct debug_mem_header * header = (struct debug_mem_header *)p;
        struct debug_mem_footer * footer = (struct debug_mem_footer *)((char*)p +
                                                                       sizeof(struct debug_mem_header) +
                                                                       size);
        header->file = file;
        header->function = function;
        header->line = line;
        header->context = "realloc";
        header->size = size;
        memcpy(header->marker, DEBUG_MEM_MARKER, DEBUG_MEM_MARKER_SIZE);
        memcpy(footer->marker, DEBUG_MEM_MARKER, DEBUG_MEM_MARKER_SIZE);
    } else {
        fprintf(stderr,
                "Failed to realloc %lu bytes (debug size=%lu, initial address=%p) in %s:%s@%d: %s (errno=%d)\n",
                size,
                num_bytes,
                ptr,
                (file ? file : "N/A"),
                (function ? function : "N/A"),
                line,
                strerror(errno),
                errno);
        abort();
    }

    p = ((char*)p + sizeof(struct debug_mem_header));
#if DEBUG_LOG_MEM
    fprintf(stderr, "mm: realloc %lu bytes at %p\n", size, p);
#endif
    return p;
}

void debug_mm_free(const char * file, const char * function, int line, void * ptr) {
#if DEBUG_LOG_MEM
    fprintf(stderr, "mm: free at %p (at %s@%d)\n", ptr, function, line);
#endif
    struct debug_mem_header const * header = (struct debug_mem_header const *)((char const *)ptr - sizeof(struct debug_mem_header));
    const size_t size = header->size;
    const char * const allocated_at_function = header->function;
    int allocated_at_line = header->line;
    const char * const context = header->context;
    free(debug_check_memory(file, function, line, "free", ptr));
#if DEBUG_LOG_MEM
    fprintf(stderr,
            "mm: freed %lu bytes at %p allocated by %s at %s@%d\n",
            size,
            ptr,
            context,
            allocated_at_function,
            allocated_at_line);
#endif
}

#pragma pack(pop)

#endif /*DEBUG_MEM*/
