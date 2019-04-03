# ConcurrentProgramming_Lab_1
## How To Run

<strong>1.Serial Linked List </strong><br>
 Compile : gcc -g -Wall -o serial_linked_list serial_linked_list.c -lm <br>
 Run : ./serial_linked_list <n> <m> <mMember> <mInsert> <mDelete> <br>
 
<strong>2.Mutex Linked List </strong><br>
Compile : gcc -g -Wall -o mutex_linked_list mutex_linked_list.c -pthread -lm <br>
Run : ./mutex_linked_list <n> <m> <mMember> <mInsert> <mDelete> <br>

<strong>3.RW Linked List </strong><br>
Compile : gcc -g -Wall -o rw_linked_list rw_linked_list.c -pthread -lm <br>
Run : ./rw_linked_list <n> <m> <mMember> <mInsert> <mDelete> <br>
