# ConcurrentProgramming_Lab_1
## How To Run

1.Serial Linked List
 Compile : gcc -g -Wall -o serial_linked_list serial_linked_list.c -lm
 Run : ./serial_linked_list <n> <m> <mMember> <mInsert> <mDelete>
 
2.Mutex Linked List
Compile : gcc -g -Wall -o mutex_linked_list mutex_linked_list.c -pthread -lm
Run : ./mutex_linked_list <n> <m> <mMember> <mInsert> <mDelete>

3. RW Linked List
Compile : gcc -g -Wall -o rw_linked_list rw_linked_list.c -pthread -lm
Run : ./rw_linked_list <n> <m> <mMember> <mInsert> <mDelete>
