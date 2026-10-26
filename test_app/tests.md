
Right-sized: 8 tests
These are the ones that each back a specific resume claim or produce a real interview story:

heap_caps_malloc returns NULL → clean error, no crash - done 
Oversized payload → dropped, no write past the end
Wrong magic byte → dropped, and downstream not called
Sequence gap → error counter increments, pipeline continues
Sequence counter wraparound → no false error
spi_device_transmit fails → error propagated and buffer released
Transaction length set in bits, not bytes
Alloc/free loop → zero heap delta

my goal is to go fast but still write code. do i have enough info for the first heap caps test?

FAQ:

Why one big func for process bool?