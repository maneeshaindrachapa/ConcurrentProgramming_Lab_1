/*
 * Linked List with a single mutex for the whole list
 *
 * Compile : gcc -g -Wall -o mutex_linked_list mutex_linked_list.c -pthread -lm
 * Run : ./mutex_linked_list <n> <m> <mMember> <mInsert> <mDelete>
 *
 * */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <math.h>

#define MAX_THREADS 1024
#define MAX_RANDOM 65535

// Count variables to store each thread operation execution times
int count_member = 0;
int count_insert = 0;
int count_delete = 0;

// Number of nodes in the linked list
int n = 0;

// Number of random operations
int m = 0;

// Number of executing threads
int thread_count = 0;

//no of times
int times_run = 0;

// Fractions for operation
float m_insert_fraction = 0.0, m_delete_fraction = 0.0, m_member_fraction = 0.0;

// Total number of each operation
float m_insert = 0.0, m_delete = 0.0, m_member = 0.0;

struct list_node_s *head = NULL;
pthread_mutex_t mutex;

// Node definition
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

//calculate optimal execution count within an accuracy of ±5% and 95% confidence level
double calcOptExp(double mean, double std)
{
    times_run = (100 * 1.960 * std / (5 * mean)) * (100 * 1.960 * std / (5 * mean));
    return times_run;
}

//Getting the inputs
void getInput(int argc, char *argv[])
{

    // Number of arguments validation
    if (argc != 7)
    {
        printf("Please give the command: ./mutex_linkedList <n> <m> <thread_count> <noftimes> <mMember> <mInsert> <mDelete>\n");
        exit(0);
    }

    // Setting the input values of n,m and thread count
    n = (int)strtol(argv[1], (char **)NULL, 10);
    m = (int)strtol(argv[2], (char **)NULL, 10);
    thread_count = (int)strtol(argv[3], (char **)NULL, 10);
    //times_run=(int) strtol(argv[4], (char **) NULL, 10);

    // Setting the input values of operation fraction values
    m_member_fraction = (float)atof(argv[4]);
    m_insert_fraction = (float)atof(argv[5]);
    m_delete_fraction = (float)atof(argv[6]);

    // Thread count validation
    if (thread_count <= 0 || thread_count > MAX_THREADS)
    {
        printf("Please give provide a valid number of threads in the range of 0 to %d\n", MAX_THREADS);
        exit(0);
    }

    //Arguments validation
    if (n <= 0 || m <= 0 || m_member_fraction + m_insert_fraction + m_delete_fraction != 1.0)
    {
        printf("Please give the command with the arguements: ./mutex_linkedList <n> <m> <thread_count> <mMember> <mInsert> <mDelete>\n");

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
void *Thread_Operation()
{

    int count_tot = 0;

    int finished_member = 0;
    int finished_insert = 0;
    int delete_finished = 0;

    while (count_tot < m)
    {

        // Variable to randomly generate values for operations
        int random_value = rand() % MAX_RANDOM;

        // Variable to randomly select one of the three operations
        int random_select = rand() % 3;

        // Member operation
        if (random_select == 0 && finished_member == 0)
        {

            pthread_mutex_lock(&mutex);
            if (count_member < m_member)
            {
                Member(random_value, head);
                count_member++;
            }
            else
                finished_member = 1;
            pthread_mutex_unlock(&mutex);
        }

        // Insert Operation
        if (random_select == 1 && finished_insert == 0)
        {

            pthread_mutex_lock(&mutex);
            if (count_insert < m_insert)
            {
                Insert(random_value, &head);
                count_insert++;
            }
            else
                finished_insert = 1;
            pthread_mutex_unlock(&mutex);
        }

        // Delete Operation
        else if (random_select == 2 && delete_finished == 0)
        {

            pthread_mutex_lock(&mutex);
            if (count_delete < m_delete)
            {
                Delete(random_value, &head);
                count_delete++;
            }
            else
                delete_finished = 1;
            pthread_mutex_unlock(&mutex);
        }

        // Updating the count
        count_tot = count_insert + count_member + count_delete;
    }
    return NULL;
}

// Calculating time
double CalcTime(struct timeval time_start, struct timeval time_end)
{

    return (double)(time_end.tv_usec - time_start.tv_usec) / 1000000 + (double)(time_end.tv_sec - time_start.tv_sec);
}

int main(int argc, char *argv[])
{
    // Obtaining the inputs
    getInput(argc, argv);

    pthread_t *thread_handlers;
    thread_handlers = malloc(sizeof(pthread_t) * thread_count);

    int count = 0; //to count how many time program happened
    float mean = 0.0;
    float std = 0.0;

    // get initial execution rounds as 10
    int cur_exe_count = 10;
    int pre_exe_count = cur_exe_count - 2;
    int exe_count = 0;

    // time variables
    struct timeval time_start, time_end;

    // Calculating the total number of each operation
    m_insert = m_insert_fraction * m;
    m_delete = m_delete_fraction * m;
    m_member = m_member_fraction * m;

    while (abs(cur_exe_count - pre_exe_count) > 1)
    {
        count = 0;
        mean = 0.0;
        std = 0.0;
        float timeCalc[(int)cur_exe_count]; //keep track of the time for each execution
        exe_count++;

        while (count < cur_exe_count)
        {
            m_insert = m_insert_fraction * m;
            m_delete = m_delete_fraction * m;
            m_member = m_member_fraction * m;
            count_member = 0;
            count_insert = 0;
            count_delete = 0;

            head = NULL;
            // Linked List Generation with Random values
            int i = 0;
            while (i < n)
            {
                if (Insert(rand() % 65535, &head) == 1)
                    i++;
            }

            // Initializing the mutex
            pthread_mutex_init(&mutex, NULL);

            // Getting the begin time stamp
            gettimeofday(&time_start, NULL);

            // Thread Creation
            i = 0;
            while (i < thread_count)
            {
                pthread_create(&thread_handlers[i], NULL, (void *)Thread_Operation, NULL);
                i++;
            }

            // Thread Join
            i = 0;
            while (i < thread_count)
            {
                pthread_join(thread_handlers[i], NULL);
                i++;
            }

            // Getting the end time stamp
            gettimeofday(&time_end, NULL);

            // Destroying the mutex
            pthread_mutex_destroy(&mutex);

            //free(thread_handlers);

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
