/*
 * Serial Linked List
 *
 * Compile : gcc -g -Wall -o serial_linked_list serial_linked_list.c -lm
 * Run : ./serial_linked_list <n> <m> <mMember> <mInsert> <mDelete>
 *
 * */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

#define MAX_RANDOM 65535

// Number of nodes in the linked list
int n = 0;

// Number of random operations in the linked list
int m = 0;

// Fractions of each operation
float m_insert_fraction, m_delete_fraction, m_member_fraction;

//Node definition
struct list_node_s
{
    int data;
    struct list_node_s *next;
};

//Getting the inputs
void getInput(int argc, char *argv[])
{
    if (argc != 6)
    {
        printf("Please give the command: ./serial_linkedList <n> <m> <mMember> <mInsert> <mDelete>\n");
        exit(0);
    }

    n = (int)strtol(argv[1], (char **)NULL, 10);
    m = (int)strtol(argv[2], (char **)NULL, 10);

    m_member_fraction = (float)atof(argv[3]);
    m_insert_fraction = (float)atof(argv[4]);
    m_delete_fraction = (float)atof(argv[5]);

    //Validating the arguments
    if (n <= 0 || m <= 0 || m_member_fraction + m_insert_fraction + m_delete_fraction != 1.0)
    {
        printf("Please give valid arguments: ./serial_linkedList <n> <m> <mMember> <mInsert> <mDelete>\n");

        if (n <= 0)
            printf("Please give a valid number of nodes for the linked list (value of n)\n");

        if (m <= 0)
            printf("Please give a valid number of operations for the linked list (value of m)\n");

        if (m_member_fraction + m_insert_fraction + m_delete_fraction != 1.0)
            printf("Please provide valid fractions of operations for the linked list (value of mMember, mInsert, mDelete)\n");

        exit(0);
    }
}

//Member function in the Linked List
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
        return 1;
}

//Insert function in the Linked List
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

//Remove function in the Linked List
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

//Calculating time
double CalcTime(struct timeval time_start, struct timeval time_end)
{
    return (double)(time_end.tv_usec - time_start.tv_usec) / 1000000 + (double)(time_end.tv_sec - time_start.tv_sec);
}

//calculate optimal execution count within an accuracy of ±5% and 95% confidence level
double calcOptExp(double mean, double std)
{
    double sample_size_n = (100 * 1.960 * std / (5 * mean)) * (100 * 1.960 * std / (5 * mean));
    return sample_size_n;
}

int main(int argc, char *argv[])
{

    int count = 0; //to count how many time program happened
    float mean = 0.0;
    float std = 0.0;
    // get initial execution rounds as 10
    int cur_exe_count = 10;
    int pre_exe_count = cur_exe_count - 2;
    int exe_count = 0;

    // Check the execution rounds converges
    while (abs(cur_exe_count - pre_exe_count) > 1)
    {
        count = 0;
        mean = 0.0;
        std = 0.0;
        float timeCalc[(int)cur_exe_count]; //keep track of the time for each execution
        exe_count++;
        while (count < cur_exe_count)
        {
            struct list_node_s *head = NULL;
            struct timeval time_start, time_end;

            //Get Inputs
            getInput(argc, argv);

            //Generate Linked List with Random values
            int i = 0;
            while (i < n)
            {
                if (Insert(rand() % MAX_RANDOM, &head) == 1)
                    i++;
            }

            //Operations in the linked list
            int count_total = 0;
            int count_member = 0;
            int count_insert = 0;
            int count_delete = 0;

            float m_insert = m_insert_fraction * m;
            float m_delete = m_delete_fraction * m;
            float m_member = m_member_fraction * m;

            gettimeofday(&time_start, NULL);

            while (count_total < m)
            {

                int rand_value = rand() % MAX_RANDOM;
                int rand_select = rand() % 3;

                if (rand_select == 0 && count_member < m_member)
                {
                    Member(rand_value, head);
                    count_member++;
                }

                if (rand_select == 1 && count_insert < m_insert)
                {
                    Insert(rand_value, &head);
                    count_insert++;
                }

                else if (rand_select == 2 && count_delete < m_delete)
                {
                    Delete(rand_value, &head);
                    count_delete++;
                }

                count_total = count_insert + count_member + count_delete;
            }

            gettimeofday(&time_end, NULL);

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
        printf("\n********************** Code Run Count %d **********************\n", exe_count);
        printf("\tStd : %.5f \n", std);
        printf("\tMean : %.5f\n", mean);
        printf("\tExeperiments : %d \n", cur_exe_count);

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
    printf(">>>>>>>>>>>>>>>>>>>>>> Final Results <<<<<<<<<<<<<<<<<<<<<<<\n");
    return 0;
}
