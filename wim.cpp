/*
  This is just a project that I made within a few hours, and I don't intend
  to maintain this, this is really just an exercise.

  If it's on GitHub, 99% chance I'm not touching this again, but it's kind of
  cool I guess.
 */
/*
  Weightwatchers(in executable size, but piggy at runtime!) Windows VI?
  like whim!

  Should be an example of how to make a semi-useful free-standing program. Should be a mini/tiny executable
  that's like vim-ish.

  Not a good example of how to make a text editor since otherwise it's a very naive text editor, it's just small and
  freestanding (line buffer is my data structure of choice.)

  This also only works on classic windows consoles, which is going to die soon so yeah.

  This is very naive and uses lots of fixed allocations, this is really bad but I don't want to write
  malloc.

  no syntax highlighting, so sorry.

  Also no, I didn't attempt to architect this, whatever is here is likely whatever first came in my head and out
  through my fingers.

  It's simple certainly, but by no means good learning material, but it's cool I guess.

  TODO:
  - Resizing this
  I'm not doing this since I don't think it's worth the hassle for something that should seem so simple...
  You should ideally make your own "screen buffer" and just render to the screen that way...

  But that's more pain than I'm willing to stomach right now.
*/
#include <windows.h>
#include <cstdint>

#define array_count(x) (sizeof(x)/sizeof(x[0]))
#define local    static
#define internal static
#define Kilobyte(x)                 (uint64_t)(x * 1024LL)
#define Megabyte(x)                 (uint64_t)(x * 1024LL * 1024LL)
#define Gigabyte(x)                 (uint64_t)(x * 1024LL * 1024LL * 1024LL)
#define Terabyte(x)                 (uint64_t)(x * 1024LL * 1024LL * 1024LL * 1024LL)
#define Petabyte(x)                 (uint64_t)(x * 1024LL * 1024LL * 1024LL * 1024LL * 1024LL)
#define unused(x) (void)(x)

typedef float  f32;
typedef double f64;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8; /* byte */

typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t  s8;

#include <cstdarg>

void vmem_deallocate(void* base_addr, size_t amount) {
    VirtualFree(base_addr, amount, MEM_RELEASE | MEM_DECOMMIT);
}
void* vmem_reserve(void* base_addr, size_t amount) {
    void* address = VirtualAlloc(base_addr, amount, MEM_RESERVE, PAGE_NOACCESS);
    return address;
}

local HANDLE get_stdout(void) {
    local bool found_stdout = false;
    local HANDLE stdout_handle;

    if (!found_stdout) {
        stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    }

    return stdout_handle;
}

local HANDLE get_stdin(void) {
    local bool found_stdin = false;
    local HANDLE stdin_handle;

    if (!found_stdin) {
        stdin_handle = GetStdHandle(STD_INPUT_HANDLE);
    }

    return stdin_handle;
}

local u32 get_page_size(void) {
    static bool found_page_size = false;
    static u32  page_size       = 0;

    if (!found_page_size) {
        SYSTEM_INFO info;
        GetSystemInfo(&info);
        page_size = info.dwPageSize;
    }

    return page_size;
}

size_t vmem_commit(void* addr, size_t amount) {
    if (amount == 0) {
        return 0;
    }

    auto page_size = get_page_size();
    size_t pages_to_commit = (amount / page_size)+1;
    VirtualAlloc(addr, page_size * pages_to_commit, MEM_COMMIT, PAGE_READWRITE);
    return pages_to_commit;
}

size_t cstring_length(const char* input) {
    size_t result = 0;
    while (*input) {
        result++, input++;
    }
    return result;
}

bool cstring_equal(const char* a, const char* b) {
    s32 length = cstring_length(a);
    if (length != cstring_length(b)) {
        return false;
    }
    for (s32 index = 0; index < length; ++index) {
        if (a[index]!=b[index])
            return false;
    }

    return true;
}

void cstring_copy(const char* source, char* destination, size_t amount=0) {
    if (amount == 0)
        amount = cstring_length(source);

    for (unsigned index = 0; index < amount; ++index) {
        destination[index] = source[index];
    }
}
/* would normally have long names, but I just wanna get this in. so whatever */
/*
  Supported:
  %s no length delimited, only null-terminated
  %d
  %c
*/
/* NOTE: not full snprintf compliant implementation, just good enough for the needs of my wim  */
/* did I say not full? I meant absolutely not at all! Actually why didn't I just invent my own syntax if I was going to do this myself? */
int wim_snprintf(char* target_buffer, int length_of_buffer, const char* format_string, ...) {
    int format_string_length = cstring_length(format_string);
    /* please don't do this */
    va_list args;
    va_start(args, format_string);
    int write_cursor = 0;
    for (int i = 0; i < format_string_length && write_cursor < length_of_buffer; ++i) {
        switch (format_string[i]) {
            case '%': {
                i++;
                switch (format_string[i]) {
                    case 'd': {
                        auto d = va_arg(args, int);
                        int digit_count = 0;
                        {auto _d = d; while (_d) digit_count++, _d /= 10;}

                        /* 0 is the end, digit_count-1 is the front */
                        for (int digit_index = 0; digit_index < digit_count; ++digit_index) {
                            int digit = 0;
                            {
                                int _d = d;
                                int cur_digit = digit_count-1;
                                while (cur_digit != digit_index) {
                                    cur_digit--;
                                    _d /= 10;
                                }
                                digit = _d % 10;
                            }
                            target_buffer[write_cursor++] = "0123456789"[digit];
                        }
                    } break;
                    case 'c': {
                        int c = va_arg(args, int);
                        target_buffer[write_cursor++] = (char)c;
                    } break;
                    case 's': {
                        auto s = va_arg(args, char*);
                        int strlen = cstring_length(s);
                        for (int j = 0; j < strlen; ++j) {
                            target_buffer[write_cursor++] = s[j];
                        }
                    } break;
                }
            } break;
            default: {
                target_buffer[write_cursor++] = format_string[i];
            } break;
        }
    }
    target_buffer[write_cursor+1] = 0;
    va_end(args);

    return write_cursor;
}

struct VMem_Arena {
    void* base_address;
    u64   used;
    u64   capacity;
    u64   pages_commited;       /* in bytes, not pages */

    void* allocate(u64 amount) {
        void* new_address = (char*)base_address + used;
        used += amount;
        commit_pages_as_needed();
        return new_address;
    }

    void reserve_bytes(u64 amount) {
        u64 old_used = used;
        allocate(amount);
        used = old_used;
    }

    void clear(void) {
        used = 0;
    }

    void finish(void) {
        vmem_deallocate(base_address, capacity);
    }

private:
    void commit_pages_as_needed() {
        if (used == 0) {
            return;
        }

        auto   page_size  = get_page_size();
        size_t pages_used = (used / page_size) + 1;

        auto pages_to_commit = pages_used - pages_commited;

        if (pages_to_commit > 0) {
            vmem_commit(base_address, pages_to_commit * page_size);
            pages_commited += pages_to_commit;
        }
    }
};

VMem_Arena vmem_arena_create(u64 size) {
    VMem_Arena result = {};
    result.base_address   = vmem_reserve(nullptr, size);
    result.used           = 0;
    result.pages_commited = 0;
    result.capacity       = size;
    return result;
}

/* for debugging purposes really... */
void _print(const char* cstr) {
    DWORD t; WriteConsole(get_stdout(), cstr, cstring_length(cstr), &t, 0);
}

#define MAX_SWAPBUFFERS (4)
int    backbuffer = 0;
HANDLE consoles[MAX_SWAPBUFFERS] = {};
void init_buffers(void) {
    for (s32 i = 0; i < MAX_SWAPBUFFERS; ++i) {
        consoles[i] = CreateConsoleScreenBuffer(GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CONSOLE_TEXTMODE_BUFFER, 0);
        if (consoles[i] == INVALID_HANDLE_VALUE) {
            // MessageBox(0, "???", "Couldn't open extra buffer", MB_ICONERROR);
        }
    }
}
void swap_buffers(void) {
    SetConsoleActiveScreenBuffer(consoles[backbuffer]);
    backbuffer++;
    if (backbuffer >= MAX_SWAPBUFFERS) backbuffer = 0;
}
HANDLE current_buffer_handle(void) {
    return consoles[backbuffer];
}

struct Console_Dimensions {int width; int height;};
Console_Dimensions get_console_dimensions(void) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(get_stdout(), &csbi);
    int columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int rows    = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    return {columns, rows};
}

enum wim_attrib {
    WIM_VIDEO_ATTRIBUTE_FG_RED   = FOREGROUND_RED,
    WIM_VIDEO_ATTRIBUTE_FG_BLUE  = FOREGROUND_BLUE,
    WIM_VIDEO_ATTRIBUTE_FG_GREEN = FOREGROUND_GREEN,
    WIM_VIDEO_ATTRIBUTE_BG_RED   = BACKGROUND_RED,
    WIM_VIDEO_ATTRIBUTE_BG_BLUE  = BACKGROUND_BLUE,
    WIM_VIDEO_ATTRIBUTE_BG_GREEN = BACKGROUND_GREEN,

    WIM_VIDEO_ATTRIBUTE_INVERSE  = WIM_VIDEO_ATTRIBUTE_BG_GREEN | WIM_VIDEO_ATTRIBUTE_BG_RED | WIM_VIDEO_ATTRIBUTE_BG_BLUE,
    WIM_DEFAULT_FLAGS            = WIM_VIDEO_ATTRIBUTE_FG_GREEN | WIM_VIDEO_ATTRIBUTE_FG_RED | WIM_VIDEO_ATTRIBUTE_FG_BLUE
};

void wim_set_title(const char* title) {
    SetConsoleTitle(title);
}

/* from msdn */

void wim_clear_screen(void) {
    HANDLE hConsole = current_buffer_handle();
    COORD coordScreen = { 0, 0 };
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;

    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {return;}
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

    if (!FillConsoleOutputCharacter(hConsole, ' ', dwConSize, coordScreen, &cCharsWritten)) {return;}
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {return;}
    if (!FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten)) {return;}

    SetConsoleCursorPosition(hConsole, coordScreen);
}

void wim_set_attribs(WORD attribs) {
    SetConsoleTextAttribute(current_buffer_handle(), attribs);
}

void wim_set_attrib_at(int x, int y, WORD attribs) {
    DWORD written;
    WriteConsoleOutputAttribute(current_buffer_handle(), &attribs, 1, {(short)x, (short)y}, &written);
}

/* should avoid and just use output attribute, but too lazy for full stuff yet */
void wim_move_cursor_to(int x, int y) {
    SetConsoleCursorPosition(current_buffer_handle(), {(short)x, (short)y});
}
void wim_write_string_at(int x, int y, char* text) {
    DWORD written; 
    wim_move_cursor_to(x, y);
    WriteConsole(current_buffer_handle(), text, cstring_length(text), &written, 0);
}
void wim_write_char_at(int x, int y, char c) {
    DWORD written;
    wim_move_cursor_to(x, y);
    WriteConsole(current_buffer_handle(), &c, 1, &written, 0);
}

local VMem_Arena main_arena;
local VMem_Arena scratch_arena;
local bool       wim_running = true;

#define MAX_INPUT_RECORD_EVENTS_TO_RECORD (32)
local INPUT_RECORD input_events[MAX_INPUT_RECORD_EVENTS_TO_RECORD];
DWORD              events_to_read = 0;

/* LMAO */
#define LINE_MAX_CHAR         (2048)
#define MAX_LINES             (65000)
#define MAX_SUPPORTED_BUFFERS (16)

struct Line {
    char str[LINE_MAX_CHAR];
    s32  used;
};

enum wim_mode {
    WIM_MODE_NORMAL,
    WIM_MODE_INSERT,
    WIM_MODE_COMMAND,
};

const char* wim_mode_strings[] = {
    "NORMAL", "INSERT", "COMMAND"
};
struct WIM_Buffer {
    /* This is a naive line buffer for now, but this API can be swapped pretty simply...*/
    int  line_count;
    Line lines[MAX_LINES];
};

struct wim_state {
    int mode = 0;

    int cursor_x = 0; /* col */
    int cursor_y = 0; /* row */
    int current_filesize = 0;

    int  line_count = 0;
    Line lines[MAX_LINES] = {};
    s32  scroll_y = 0;

    int  command_cursor_current = 0;
    int  command_length = 0;
    char command_line[LINE_MAX_CHAR] = {};

    int chord_write_cursor = 0;
    char chord_state[64];

    bool linenum;
} *global_wim;

local void reset_chord(void) {
    global_wim->chord_write_cursor = 0;
}
local void push_chord(char c) {
    global_wim->chord_state[global_wim->chord_write_cursor++] = c;
}
local bool in_chord_mode(void) {
    return global_wim->chord_write_cursor > 0;
}

local void reset_buffer(void) {
    global_wim->line_count = 0;
    global_wim->cursor_x = global_wim->cursor_y = 0;
}
/* this is making an absolutely new line */
local Line* new_line(int at=-1) {
    if (at == -1) {
        at = global_wim->line_count;
        global_wim->line_count++;
        for (int i = at; i < global_wim->line_count; ++i) {
            global_wim->lines[i+1] = global_wim->lines[i];
        }
    } else {
        global_wim->line_count++;
        /* copy and push forward lines, new line */
        for (int i = global_wim->line_count-1; i >= at; --i) {
            global_wim->lines[i] = global_wim->lines[i-1];
        }
    }
    global_wim->lines[at].used   = 0;
    global_wim->lines[at].str[0] = 0;
    return &global_wim->lines[at];
}

local void delete_line(int at=-1) {
    if (at == -1) {
        at = global_wim->line_count;
        global_wim->line_count--;
    } else {
        /* copy and push forward lines, new line */
        if (global_wim->line_count > 0) {
            if (global_wim->line_count == 1) {
                global_wim->line_count = 0;
            } else {
                for (int i = at+1; i < global_wim->line_count; ++i) {
                    global_wim->lines[i-1] = global_wim->lines[i];
                }
                global_wim->line_count--;
            }
        }
    }
}

local void clear_command(void) {
    for (unsigned index = 0; index < global_wim->command_length; ++index) {
        global_wim->command_line[index] = 0;
    }
    global_wim->command_cursor_current = 0;
    global_wim->command_length         = 0;
}

local void draw_buffer(void) {
    auto [w,h] = get_console_dimensions();
    h-=3; /*account for status bar stealing stuff*/

    s32 line_draw_start = global_wim->scroll_y;
    s32 line_draw_end   = global_wim->line_count;

    if (line_draw_end > h+global_wim->scroll_y) line_draw_end = h+global_wim->scroll_y;

    s32 pad = 0;

    if (global_wim->linenum) {
        pad = 1;
        {auto _d = global_wim->line_count; while (_d) pad++, _d /= 10;}
    }

    s32 y = 0;
    for (s32 line_index = line_draw_start; line_index < line_draw_end+1; ++line_index) {
        Line* current_line = global_wim->lines + line_index;
        char linenum[16];
        int wrote = wim_snprintf(linenum, 16, "%d", line_index+1);
        linenum[wrote] = 0;
        wim_set_attribs(WIM_VIDEO_ATTRIBUTE_FG_RED);
        wim_write_string_at(0, y, linenum);
        wim_set_attribs(WIM_DEFAULT_FLAGS);
        wim_write_string_at(pad, y, current_line->str);
        for (s32 i = 0; i < w - current_line->used; ++i) wim_write_char_at(pad+current_line->used+i, y, ' ');
        y++;
    }

    s32 draw_cursor_y = global_wim->cursor_y-global_wim->scroll_y;
    if (draw_cursor_y > h) {
        draw_cursor_y = h;
    }
    wim_set_attrib_at(global_wim->cursor_x+pad, draw_cursor_y, WIM_VIDEO_ATTRIBUTE_INVERSE);
}

local void draw_status_mode_line(int height) {
    auto [w, h] = get_console_dimensions();
    auto y_of_status_line = h-height;

    w -= 1;

    auto modeline_text = (char*)scratch_arena.allocate(w);
    int written = wim_snprintf(modeline_text, w, "WIM, MODE: %s,   ", wim_mode_strings[global_wim->mode]);
    if (in_chord_mode()) {
        for (int i = 0; i < global_wim->chord_write_cursor; ++i) {
            char* to_write_fmt = "%c, ";
            if (i == 0) {
                to_write_fmt = "CHRD: [%c, ";
                if (global_wim->chord_write_cursor == 1) {
                to_write_fmt = "CHRD: [%c]";
                }
            } else if (i+1 >= global_wim->chord_write_cursor) {
                to_write_fmt = "%c]";
            }
            written += wim_snprintf(modeline_text+written-1, w-written, to_write_fmt, global_wim->chord_state[i]);
        }
    }
    for (int i = written-1; i < w; ++i) modeline_text[i] = ' ';
    modeline_text[w] = 0;

    wim_set_attribs(WIM_VIDEO_ATTRIBUTE_INVERSE);
    wim_write_string_at(0, y_of_status_line, modeline_text);

    wim_set_attribs(WIM_DEFAULT_FLAGS);
    {
        if (global_wim->mode == WIM_MODE_COMMAND) {
            wim_write_string_at(0, y_of_status_line+1, global_wim->command_line);
            wim_set_attrib_at(global_wim->command_cursor_current, y_of_status_line+1, WIM_VIDEO_ATTRIBUTE_INVERSE);
        }
        for (int i = global_wim->command_length; i < w; ++i) wim_write_char_at(i, y_of_status_line+1, ' ');
    }
}

local bool load_buffer(const char* buffer, size_t buffer_length) {
    Line* current_line = new_line();

    for (unsigned index = 0; index < buffer_length; ++index) {
        switch (buffer[index]) {
            case '\n': {
                current_line = new_line();
            } break;
            default: {
                if (current_line->used < LINE_MAX_CHAR)
                    current_line->str[current_line->used++] = buffer[index];
            } break;
        }
    }

    return true;
}

local bool open_buffer(const char* filepath) {
    HANDLE file_handle = CreateFile(filepath,
                                    GENERIC_READ,
                                    FILE_SHARE_READ|FILE_SHARE_WRITE,
                                    NULL,
                                    OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL,
                                    NULL);

    if (file_handle == INVALID_HANDLE_VALUE) {
        CloseHandle(file_handle);
        return false;
    }

    LARGE_INTEGER filesize;
    if (!GetFileSizeEx(file_handle, &filesize)) {
        CloseHandle(file_handle);
        return false;
    }

    scratch_arena.reserve_bytes(filesize.QuadPart);
    void* data_buffer = scratch_arena.allocate(0);

    if (ReadFile(file_handle, data_buffer, filesize.QuadPart, 0, 0)) {
        /* okay buffer got read. */
        load_buffer((char*)data_buffer, filesize.QuadPart);
        CloseHandle(file_handle);
        return true;
    } else {
        // MessageBox(0, filepath, "Failure to open buffer!", MB_ICONERROR);
    }

    CloseHandle(file_handle);
    return false;
}

local void quit(void) {
    wim_running = false;
}

local void enter_insert_mode(void) {
    global_wim->mode = WIM_MODE_INSERT;
}

local void enter_command_mode(void) {
    global_wim->mode                   = WIM_MODE_COMMAND;
    global_wim->command_cursor_current = 0;
}

local void enter_normal_mode(void) {
    clear_command();
    global_wim->mode = WIM_MODE_NORMAL;
}

local void confirm_and_process_command(void) {
    /* TODO */
    if (cstring_equal(global_wim->command_line, "quit")) {
        quit();
    }
    if (cstring_equal(global_wim->command_line, "line")) {
        global_wim->linenum ^= 1;
    }
    enter_normal_mode();
}
local void reset_cursor_on_newline(void) {
    s32 current_line_length = global_wim->lines[global_wim->cursor_y].used;
    if (current_line_length == 0) {
        global_wim->cursor_x = 0;
    } else if (global_wim->cursor_x > current_line_length) {
        global_wim->cursor_x = current_line_length-1;
    }
}

/* not suitable for pages yet */
local bool try_scroll_down(void) {
    auto [w,h] = get_console_dimensions();
    if ((global_wim->cursor_y-global_wim->scroll_y) > h-2) {
        global_wim->scroll_y += 1;
        return true;
    }

    return false;
}
local bool try_scroll_up(void) {
    auto [w,h] = get_console_dimensions();
    if ((global_wim->cursor_y-global_wim->scroll_y) < 0) {
        global_wim->scroll_y -= 1;
        return true;
    }

    return false;
}

local void move_left(int amount=1) {
    global_wim->cursor_x -= amount;
    if (global_wim->cursor_x < 0) {
        global_wim->cursor_x = 0;
    }
}
local void move_up(int amount=1) {
    global_wim->cursor_y -= amount;
    if (global_wim->cursor_y < 0) {
        global_wim->cursor_y = 0;
    }
    reset_cursor_on_newline();

    try_scroll_up();
}
local void move_down(int amount=1) {
    global_wim->cursor_y += amount;
    s32 line_count = global_wim->line_count;
    if (global_wim->cursor_y >= line_count) {
        global_wim->cursor_y =  line_count-1;
    }
    reset_cursor_on_newline();

    try_scroll_down();
}
local void move_right(int amount=1) {
    s32 current_line_length = global_wim->lines[global_wim->cursor_y].used;
    if (current_line_length == 0) {
        return;
    }

    global_wim->cursor_x += amount;
    if (current_line_length == 1) {
        global_wim->cursor_x = 0;
    }
    if (global_wim->cursor_x >= current_line_length+1) {
        global_wim->cursor_x =  current_line_length;
    }
}
local void keep_cursor_in_column_bounds(void) {
    s32 current_line_length = global_wim->lines[global_wim->cursor_y].used;
    if (global_wim->cursor_x < 0) {
        global_wim->cursor_x = 0;
    }
    if (global_wim->cursor_x >= current_line_length+1) {
        global_wim->cursor_x =  current_line_length;
    }
}
local void goto_start_of_line(void) {
    global_wim->cursor_x = 0;
}
local void goto_end_of_line(void) {
    global_wim->cursor_x = global_wim->lines[global_wim->cursor_y].used-1;
}
local void delete_character_at(int at_x, int at_y) {
    global_wim->cursor_y = at_y;
    global_wim->cursor_x = at_x;

    auto& current_line = global_wim->lines[global_wim->cursor_y];

    if (current_line.used != 0) {
        for (int i = global_wim->cursor_x; i < current_line.used; ++i) {
            current_line.str[i-1] = current_line.str[i];
        }
        current_line.used--;
    }

    keep_cursor_in_column_bounds();
};

local char character_at(int at_x, int at_y) {
    auto& current_line = global_wim->lines[at_y];
    return current_line.str[at_x];
}

/* uhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh this is bad */
local void seek_to_start_of_buffer(void) {
    /* uhhhhhhhhhhhhh */
    s32 current_y = global_wim->cursor_y;
    move_up();
    while (global_wim->cursor_y != current_y) {
        current_y = global_wim->cursor_y;
        move_up();
    }
}

local void seek_to_end_of_buffer(void) {
    /* uhhhhhhhhhhhhh */
    s32 current_y = global_wim->cursor_y;
    move_down();
    while (global_wim->cursor_y != current_y) {
        current_y = global_wim->cursor_y;
        move_down();
    }
}

local void wim_process_normal_mode(char ascii_input, int key_input) {
    if (in_chord_mode()) {
        if (key_input == VK_ESCAPE) {
            reset_chord();
        } else {
            push_chord(ascii_input);
            /* this is a combinatoric explosion of chord combinations */
            /* so good luck to me */
            char* chord     = global_wim->chord_state;
            int   chord_len = global_wim->chord_write_cursor;

            if (chord[0] == 'g') {
                if (chord_len == 2 && chord[1] == 'g') {
                    seek_to_start_of_buffer();
                    reset_chord();
                }
            }
        }
    } else {
        switch (ascii_input) {
            case ':': {
                enter_command_mode();
            } break;
            case 'i': {
                enter_insert_mode();
            } break;
            case 'a': {
                enter_insert_mode();
                move_right();
            } break;
            case 'A': {
                enter_insert_mode();
                goto_end_of_line();
                move_right();
            } break;
            case 'G': {
                seek_to_end_of_buffer();
            } break;
            case 'x': {
                delete_character_at(global_wim->cursor_x+1, global_wim->cursor_y);
                move_left();
            } break;
            case 'h': {
                move_left();
            } break;
            case 'j': {
                move_down();
            } break;
            case 'k': {
                move_up();
            } break;
            case 'l': {
                move_right();
            } break;
            case '0': {
                goto_start_of_line();
            } break;
            case '$': {
                goto_end_of_line();
            } break;

                /* all the chord keys here */
            case 'g': {
                push_chord(ascii_input);
            } break;
        }
    }
}

local void insert_character(int at_x, int at_y, char c) {
    global_wim->cursor_y = at_y;
    global_wim->cursor_x = at_x;

    auto& current_line = global_wim->lines[global_wim->cursor_y];
    if (global_wim->cursor_x < LINE_MAX_CHAR) {
        for (int i = current_line.used; i >= global_wim->cursor_x; --i) {
            current_line.str[i] = current_line.str[i-1];
        }
        current_line.used++;
        current_line.str[global_wim->cursor_x++] = c;
    }
}

local void wim_process_insert_mode(char ascii_input, int key_input) {
    if (key_input == VK_ESCAPE) {
        enter_normal_mode();
    } else if (key_input == VK_BACK) {
        if (global_wim->cursor_x >= 1) {
            if (character_at(global_wim->cursor_x-1, global_wim->cursor_y) == ' ') {
                while (character_at(global_wim->cursor_x-1, global_wim->cursor_y) == ' ') {
                    delete_character_at(global_wim->cursor_x, global_wim->cursor_y);
                    move_left();
                }
            } else {
                delete_character_at(global_wim->cursor_x, global_wim->cursor_y);
                move_left();
            }
        } else if (global_wim->cursor_x == 0) {
            /* NOTE, the correct behavior is to merge the remenants of the line! */
            delete_line(global_wim->cursor_y);
            move_up();
            goto_end_of_line();
            move_right();
            keep_cursor_in_column_bounds();
        }
    } else if (key_input == VK_TAB) {
        for (int i = 0; i < 4; ++i) insert_character(global_wim->cursor_x, global_wim->cursor_y, ' ');
    } else if (key_input == VK_UP) {
        move_up();
    } else if (key_input == VK_DOWN) {
        move_down();
    } else if (key_input == VK_RIGHT) {
        move_right();
    } else if (key_input == VK_LEFT) {
        move_left();
    } else if (key_input == VK_RETURN) {
        /* NOTE, the correct behavior is to split the remenants of the line! */
        move_down();
        new_line(global_wim->cursor_y);
        goto_start_of_line();
    } else {
        if (ascii_input) {
            insert_character(global_wim->cursor_x, global_wim->cursor_y, ascii_input);
        }
    }
}

local void wim_process_command_mode(char ascii_input, int key_input) {
    if (key_input == VK_ESCAPE) {
        enter_normal_mode();
    } else if (key_input == VK_RETURN) {
        confirm_and_process_command();
    } else if (key_input == VK_BACK) {
        if (global_wim->command_length == 0) {
            return;
        } else {
            for (int current = global_wim->command_cursor_current; current < global_wim->command_length; ++current) {
                global_wim->command_line[current-1] = global_wim->command_line[current];
            }
            global_wim->command_cursor_current -= 1;
            global_wim->command_length         -= 1;
        }
    } else {
        if (ascii_input) {
            global_wim->command_line[global_wim->command_cursor_current++] = ascii_input;
            global_wim->command_length += 1;
        }
    }
}

int wim_main(int argc, char** argv) {
    wim_set_title("WIM - v.0.0");
    {
        SetConsoleMode(get_stdin(), ENABLE_PROCESSED_INPUT | ENABLE_WINDOW_INPUT);
        CONSOLE_CURSOR_INFO info;
        info.bVisible = false;
        SetConsoleCursorInfo(get_stdout(), &info);
    }

    init_buffers();
    if (argc > 2) {
        wim_clear_screen();
        _print("WIM only wants one file! Sorry! Bye!");
        return 0;
    } else {
        char* target_file = argv[1];

        /* for editing? */
        if (open_buffer(target_file)) {
        }
    }

    while (wim_running == true) {
        wim_set_attribs(WIM_DEFAULT_FLAGS);
        wim_clear_screen();
        draw_buffer();
        draw_status_mode_line(2);
        {ReadConsoleInput(get_stdin(), input_events, MAX_INPUT_RECORD_EVENTS_TO_RECORD, &events_to_read);}

        for (int event_index = 0; event_index < events_to_read; ++event_index) {
            switch (input_events[event_index].EventType) {
                case KEY_EVENT: {
                    KEY_EVENT_RECORD key = input_events[event_index].Event.KeyEvent;
                    if (key.bKeyDown) {
                        switch (global_wim->mode) {
                            case WIM_MODE_NORMAL: {
                                wim_process_normal_mode(key.uChar.AsciiChar, key.wVirtualKeyCode);
                            } break;
                            case WIM_MODE_INSERT: {
                                wim_process_insert_mode(key.uChar.AsciiChar, key.wVirtualKeyCode);
                            } break;
                            case WIM_MODE_COMMAND: {
                                wim_process_command_mode(key.uChar.AsciiChar, key.wVirtualKeyCode);
                            } break;
                        }
                    } else {
                        
                    }
                } break;
            }
        }

        swap_buffers();
        scratch_arena.clear();
    }

    wim_set_attribs(WIM_DEFAULT_FLAGS);
    wim_clear_screen();
    _print("Bye! I hope you saved your files");
    return 0;
}

extern "C" int WINAPI mainCRTStartup(void) {
    const char* command_line = GetCommandLineA();
    main_arena    = vmem_arena_create(Gigabyte(2));
    scratch_arena = vmem_arena_create(Gigabyte(1));

    global_wim    = (wim_state*)main_arena.allocate(sizeof(*global_wim));

    char** argument_values = (char**)scratch_arena.allocate(1024 * sizeof(char**));
    int    argument_count  = 0;

    /* make argc&argv */
    {
        size_t   command_line_length = cstring_length(command_line);
        unsigned character_index     = 0;

        while (character_index < command_line_length) {
            if (command_line[character_index] == ' ') {
                character_index++;
                continue;
            } else {
                size_t word_start = character_index;

                while (command_line[character_index] != ' ' && character_index < command_line_length) {
                    character_index++;
                }

                size_t word_end    = character_index;
                size_t word_length = word_end - word_start;

                char* word                        = (char*)scratch_arena.allocate(word_length+1);
                cstring_copy(command_line+word_start, word, word_length);
                word[word_length] = 0;
                argument_values[argument_count++] = word;
            }
        }
    }

    int result = wim_main(argument_count, argument_values);
    ExitProcess(result);
    scratch_arena.finish();
    main_arena.finish();
    return result;
}
