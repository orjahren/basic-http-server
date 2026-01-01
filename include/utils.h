#pragma once

typedef struct
{
    const char *ext;
    const char *filetype;
} mime_entry_t;

extern const mime_entry_t extensions[];