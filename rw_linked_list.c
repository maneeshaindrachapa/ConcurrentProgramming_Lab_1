
/*
 * Linked List with read write lock
 *
 * Compile : gcc -g -Wall -o rw_linked_list rw_linked_list.c -pthread -lm
 * Run : ./rw_linked_list <n> <m> <tc> <mMember> <mInsert> <mDelete>
 *
 * */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <math.h>

#define MAX_THREADS 1024
#define MAX_RANDOM 65535

// Number of nodes in the linked list
int n = 0;

// Number of random operations
int m = 0;

// Number of threads to execute
int thread_count = 0;

//no of times
int times_run=0;

// Fractions of each operation
float m_insert_fraction = 0.0, m_delete_fraction = 0.0, m_member_fraction = 0.0;

// Total number of each operation
int m_insert = 0, m_delete = 0, m_member = 0;

struct list_node_s *head = NULL;
pthread_rwlock_t rwlock;

//Definition of the node
struct list_node_s
{
    int data;
    struct list_node_s *next;
};

// Member function for Linked List
int Member(int value, struct list_node_s *head_p)
{
    struct list_node_s *current_p = head_p;

    while (current_p != NULL && current_p->data < value)
        current_p = current_p->next;

    if (current_p == NULL || current_p->data > value)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

// Insert function for Linked List
int Insert(int value, struct list_node_s **head_pp)
{
    struct list_node_s *curr_p = *head_pp;
    struct list_node_s *pred_p = NULL;
    struct list_node_s *temp_p = NULL;

    while (curr_p != NULL && curr_p->data < value)
    {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p == NULL || curr_p->data > value)
    {
        temp_p = malloc(sizeof(struct list_node_s));
        temp_p->data = value;
        temp_p->next = curr_p;

        if (pred_p == NULL)
            *head_pp = temp_p;
        else
            pred_p->next = temp_p;

        return 1;
    }
    else
        return 0;
}

// Delete function for Linked List
int Delete(int value, struct list_node_s **head_pp)
{
    struct list_node_s *curr_p = *head_pp;
    struct list_node_s *pred_p = NULL;

    while (curr_p != NULL && curr_p->data < value)
    {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p != NULL && curr_p->data == value)
    {
        if (pred_p == NULL)
        {
            *head_pp = curr_p->next;
            free(curr_p);
        }
        else
        {
            pred_p->next = curr_p->next;
            free(curr_p);
        }

        return 1;
    }
    else
        return 0;
}

// Get inputs
void getInput(int argc, char *argv[])
{

    // Validation of the arguments
    if (argc != 7)
    {
        printf("Please give the command: ./serial_linked_list <n> <m> <thread_count> <mMember> <mInsert> <mDelete>\n");
        exit(0);
    }

    // Setting the input values of n,m and thread count
    n = (int)strtol(argv[1], (char **)NULL, 10);
    m = (int)strtol(argv[2], (char **)NULL, 10);
    thread_count = (int)strtol(argv[3], (char **)NULL, 10);

    // Setting the input values of operation fraction values
    m_member_fraction = (float)atof(argv[4]);
    m_insert_fraction = (float)atof(argv[5]);
    m_delete_fraction = (float)atof(argv[6]);

    // Validating the thread count
    if (thread_count <= 0 || thread_count > MAX_THREADS)
    {
        printf("Please give provide a valid number of threads in the range of 0 to %d\n", MAX_THREADS);
        exit(0);
    }

    //Validating the arguments
    if (n <= 0 || m <= 0 || m_member_fraction + m_insert_fraction + m_delete_fraction != 1.0)
    {
        printf("Please give the command with the arguements: ./serial_linked_list <n> <m> <mMember> <mInsert> <mDelete>\n");

        if (n <= 0)
            printf("Please provide a valid number of nodes for the linked list (value of n)\n");

        if (m <= 0)
            printf("Please provide a valid number of operations for the linked list (value of m)\n");

        if (m_member_fraction + m_insert_fraction + m_delete_fraction != 1.0)
            printf("Please provide valid fractions of operations for the linked list (value of mMember, mInsert, mDelete)\n");

        exit(0);
    }
}

// Thread Operations
void *Thread_Operation(void *thread_id)
{

    int local_m = 0;
    int local_m_insert = 0;
    int local_m_delete = 0;
    int local_m_member = 0;

    int id = *(int *)thread_id;

    // Generate local no of insertion operations without loss
    if (m_insert % thread_count == 0 || m_insert % thread_count <= id)
    {
        local_m_insert = m_insert / thread_count;
    }
    else if (m_insert % thread_count > id)
    {
        local_m_insert = m_insert / thread_count + 1;
    }

    // Generate local no of member operations without loss
    if (m_member % thread_count == 0 || m_member % thread_count <= id)
    {
        local_m_member = m_member / thread_count;
    }
    else if (m_member % thread_count > id)
    {
        local_m_member = m_member / thread_count + 1;
    }

    // Generate local no of deletion operations without loss
    if (m_delete % thread_count == 0 || m_delete % thread_count <= id)
    {
        local_m_delete = m_delete / thread_count;
    }
    else if (m_delete % thread_count > id)
    {
        local_m_delete = m_delete / thread_count + 1;
    }

    local_m = local_m_insert + local_m_delete + local_m_member;

    int count_tot = 0;
    int count_member = 0;
    int count_insert = 0;
    int count_delete = 0;

    int finished_member = 0;
    int finished_insert = 0;
    int delete_finished = 0;

    int i = 0;
    while (count_tot < local_m)
    {

        int random_value = rand() % MAX_RANDOM;
        int random_select = rand() % 3;

        // Member operation
        if (random_select == 0 && finished_member == 0)
        {
            if (count_member < local_m_member)
            {
                pthread_rwlock_rdlock(&rwlock);
                Member(random_value, head);
                pthread_rwlock_unlock(&rwlock);
                count_member++;
            }
            else
                finished_member = 1;
        }

        // Insert Operation
        else if (random_select == 1 && finished_insert == 0)
        {
            if (count_insert < local_m_insert)
            {
                pthread_rwlock_wrlock(&rwlock);
                Insert(random_value, &head);
                pthread_rwlock_unlock(&rwlock);
                count_insert++;
            }
            else
                finished_insert = 1;
        }

        // Delete Operation
        else if (random_select == 2 && delete_finished == 0)
        {

            if (count_delete < local_m_delete)
            {
                pthread_rwlock_wrlock(&rwlock);
                Delete(random_value, &head);
                pthread_rwlock_unlock(&rwlock);
                count_delete++;
            }
            else
                delete_finished = 1;
        }
        count_tot = count_insert + count_member + count_delete;
        i++;
    }
    return NULL;
}

// Calculating time
double CalcTime(struct timeval time_start, struct timeval time_end)
{
    return (double)(time_end.tv_usec - time_start.tv_usec) / 1000000 + (double)(time_end.tv_sec - time_start.tv_sec);
}

//calculate optimal execution count within an accuracy of ±5% and 95% confidence level
double calcOptExp(double mean, double std)
{
    times_run = (100 * 1.960 * std / (5 * mean)) * (100 * 1.960 * std / (5 * mean));
    return times_run;
}

int main(int argc, char *argv[])
{

    // Get inputs
    getInput(argc, argv);

    pthread_t *thread_handlers;
    thread_handlers = (pthread_t *)malloc(sizeof(pthread_t) * thread_count);

    int count = 0; //to count how many time program happened
    float mean = 0.0;
    float std = 0.0;

    // get initial execution rounds as 10
    int cur_exe_count = 10;
    int pre_exe_count = cur_exe_count - 2;
    int exe_count = 0;

    // time variables
    struct timeval time_start, time_end;

    int *thread_id;
    thread_id = (int *)malloc(sizeof(int) * thread_count);

    m_insert = (int)(m_insert_fraction * m);
    m_delete = (int)(m_delete_fraction * m);
    m_member = (int)(m_member_fraction * m);

    while (abs(cur_exe_count - pre_exe_count) > 1)
    {
        count = 0;
        mean = 0.0;
        std = 0.0;
        float timeCalc[(int)cur_exe_count]; //keep track of the time for each execution
        exe_count++;
        while (count < cur_exe_count)
        {

            m_member = 0;
            m_insert = 0;
            m_delete = 0;

            head = NULL;
            // Linked List Generation with Random values
            int i = 0;
            while (i < n)
            {
                if (Insert(rand() % 65535, &head) == 1)
                    i++;
            }

            pthread_rwlock_init(&rwlock, NULL);
            gettimeofday(&time_start, NULL);

            // Thread Creation
            i = 0;
            while (i < thread_count)
            {
                thread_id[i] = i;
                pthread_create(&thread_handlers[i], NULL, (void *)Thread_Operation, (void *)&thread_id[i]);
                i++;
            }

            // Thread Join
            i = 0;
            while (i < thread_count)
            {
                pthread_join(thread_handlers[i], NULL);
                i++;
            }
            gettimeofday(&time_end, NULL);
            pthread_rwlock_destroy(&rwlock);

            double timeDiff = CalcTime(time_start, time_end);
            timeCalc[count] = timeDiff;
            count++;
            mean = mean + timeDiff;
        }
        mean = mean / cur_exe_count;
        for (int i = 0; i < cur_exe_count; i++)
        {
            std = std + (timeCalc[i] - mean) * (timeCalc[i] - mean);
        }
        std = sqrt(std / cur_exe_count);
        printf("\n********************** Code Run Count**********************\n");
        printf("Std is %f \n", std);
        printf("Mean is %f \n", mean);
        printf("Experiment batch size %d \n", cur_exe_count);

        // Update the execution count variable till optimal value
        pre_exe_count = cur_exe_count;
        cur_exe_count = calcOptExp(mean, std);

        // Skip the 0 value
        if (cur_exe_count == 0)
        {
            cur_exe_count = 10;
            pre_exe_count = cur_exe_count - 2;
        }
    }
    printf("\n>>>>>>>>>>>>>>>>>>>>>> Final Results <<<<<<<<<<<<<<<<<<<<<<<\n");
    printf("Std : %.5f \n", std);
    printf("Mean : %.5f\n", mean);
    printf("Optimal exeperiments : %d \n", pre_exe_count);
    return 0;
}
