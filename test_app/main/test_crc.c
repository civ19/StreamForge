#include "unity.h"
#include "cmock.h"
#include "app_types.h"

#include "mock_queue.h"




/*
Test 1: Absolute Packet Drop (Invalid Magic Byte)

What it tests: A packet arrives with correct CRC but Byte 0 is missing the 0x5A token
 (simulating a loose connection or bit-shift alignment error).

Expected behavior: The application must instantly discard the block, 
execute zero business logic, and recycle the exact same pointer back to the empty_queue
*/

