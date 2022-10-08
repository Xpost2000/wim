# wim
 exercise to make freestanding console text editor. Not finished.

This was basically done within an evening + a bit of the midnight.

It's essentially a working text editor, though not the most ergonomic mostly cause
I don't want to spend my entire break on this excursion.

I use fixed memory allocation based on a virtual memory aware allocator so while it's kind of
stupid, it's not really super duper wasteful.

Simple is best whenever possible and as I'm in a freestanding environment basically none of the
nice things about C++ are avaliable (although I program in C usually so I guess I wouldn't notice too much.)

Could probably augment the data structures in a less tedious way (which would just be creating a bunch of linked pools),
by implementing a heap allocator on top of this... Not a super difficult task thankfully but it's a lot more complexity
than a small text editor really needs...

there's also a bad snprintf that I kind of need, and the final executable is like 20 KiB, which is surprisingly smaller
than the source code.

Anyways, this **only works on windows**.
