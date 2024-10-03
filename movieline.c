//Saikiran Annareddy
//03-06-23
//movieline

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 50
#define MAX_TICKETS 500
#define MAX_TIME 1000000000
#define NUM_QUEUES 12

// customer struct
struct Customer {
    char name[MAX_NAME_LENGTH + 1];
    int tickets;
    int arrivalTime;
    int queueNum;
};

// node structure for the linked list
struct Node {
    struct Customer customer;
    struct Node* next;
};

// queue struct
struct Queue {
    struct Node* front;
    struct Node* rear;
    int size;
};

// array of queues
struct Queue* queues[NUM_QUEUES];

// Function to create a new node
struct Node* createNode(struct Customer customer) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->customer = customer;
    newNode->next = NULL;
    return newNode;
}

// function to create an empty queue
struct Queue* createQueue() {
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
    return queue;
}

// function to insert a customer into the queue
void enqueue(struct Queue* queue, struct Customer customer) {
    struct Node* newNode = createNode(customer);
    if (queue->rear == NULL) {
        queue->front = newNode;
        queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
    queue->size++;
}

// function to remove a customer from the queue
struct Customer dequeue(struct Queue* queue) {
    if (queue->front == NULL) {
        printf("Queue is empty.\n");
        struct Customer emptyCustomer = {"", 0, 0, 0};
        return emptyCustomer;
    }
    struct Customer customer = queue->front->customer;
    struct Node* temp = queue->front;
    queue->front = queue->front->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    free(temp);
    queue->size--;
    return customer;
}

// function to peek at the front customer of the queue
struct Customer peek(struct Queue* queue) {
    if (queue->front == NULL) {
        printf("Queue is empty.\n");
        struct Customer emptyCustomer = {"", 0, 0, 0};
        return emptyCustomer;
    }
    return queue->front->customer;
}

// function to check if the queue is empty
int isEmpty(struct Queue* queue) {
    return queue->front == NULL;
}

// function to get the size of the queue
int getSize(struct Queue* queue) {
    return queue->size;
}

//function to free the memory allocated for the queue
void freeQueue(struct Queue* queue) {
    struct Node* temp;
    while (queue->front != NULL) {
        temp = queue->front;
        queue->front = queue->front->next;
        free(temp);
    }
    free(queue);
}

void assignQueuesToBooths(int boothAssignments[], int nonEmptyQueueIndices[], int k, int b);
void processCustomers(int boothAssignments[], int b);

int main() {
    int n, b;
    scanf("%d %d", &n, &b); //read input

    struct Customer customers[n]; //array to store customers
    int nextAvailableTime[NUM_QUEUES] = {0}; // array to track the next available time for each queue


    // initialize the queues array
    for (int i = 0; i < NUM_QUEUES; i++) {
        queues[i] = createQueue();
    }
    
    // find out which queues are non-empty and keep their indices
    int nonEmptyQueueIndices[NUM_QUEUES];
    int k = 0; 

    for (int i = 0; i < NUM_QUEUES; i++) {
        if (!isEmpty(queues[i])) {
            nonEmptyQueueIndices[k++] = i;
        }
    }

    // array to keep track of which queue is assigned to which booth
    int boothAssignments[NUM_QUEUES]; 
    
    assignQueuesToBooths(boothAssignments, nonEmptyQueueIndices, k, b);

    // processing the customers in each booth
    processCustomers(boothAssignments, b);

    // reading and processing each customers information
    for (int i = 0; i < n; i++) {
        scanf("%s %d %d", customers[i].name, &customers[i].tickets, &customers[i].arrivalTime);
        int p = customers[i].name[0] - 'A';
        if (p % NUM_QUEUES == 0) {
            int minQueue = 0, minSize = n + 1;
            for (int j = 0; j < NUM_QUEUES; j++) {
                if (queues[j]->size < minSize) {
                    minQueue = j;
                    minSize = queues[j]->size;
                }
            }
            customers[i].queueNum = minQueue;
            enqueue(queues[minQueue], customers[i]);
        } else {
            customers[i].queueNum = p % NUM_QUEUES;
            enqueue(queues[p % NUM_QUEUES], customers[i]);
        }
    }

    // process customers
    int checkoutTime[n];
    for (int i = 0; i < n; i++) {
        checkoutTime[i] = -1; 
    }

    for (int i = 0; i < n; i++) {
        int queueNum = customers[i].queueNum;
        int queue = i;
        int time = customers[i].arrivalTime;
        while (checkoutTime[queue] >= time) {
            queue = (queue + 1) % n;
        }
        int processingTime = 30 + customers[i].tickets * 5;
        checkoutTime[queue] = (checkoutTime[queue] == -1) ? time + processingTime : checkoutTime[queue] + processingTime;
    }

    // print output
    for (int i = 0; i < NUM_QUEUES; i++) {
        if (!isEmpty(queues[i])) {
            printf("Booth %d\n", i + 1);
            while (!isEmpty(queues[i])) {
                struct Customer customer = dequeue(queues[i]);
                printf("%s from line %d checks out at time %d.\n", customer.name, customer.queueNum + 1, checkoutTime[customer.name[0] - 'A']);
            }
            printf("\n");
        }
    }

    // free memory
    for (int i = 0; i < NUM_QUEUES; i++) {
        freeQueue(queues[i]);
    }

    return 0;
}
void assignQueuesToBooths(int boothAssignments[], int nonEmptyQueueIndices[], int k, int b) {
    int boothsPerQueue = k / b; 
    int extraQueues = k % b; 

    // initialize all to -1 indicating no assignment
    for (int i = 0; i < NUM_QUEUES; i++) {
        boothAssignments[i] = -1;
    }

    // assign booths to queues
    for (int i = 0, booth = 0; i < k; i++) {
        boothAssignments[nonEmptyQueueIndices[i]] = booth;
        
        // check if enough queues are assigned to this booth before moving on
        if ((i < extraQueues && (i % (boothsPerQueue + 1) == boothsPerQueue)) ||
            (i >= extraQueues && ((i - extraQueues) % boothsPerQueue == boothsPerQueue - 1))) {
            booth++;
        }
    }
}

void processCustomers(int boothAssignments[], int b) {
    // time each booth becomes available
    int boothAvailableTimes[b];
    for (int i = 0; i < b; i++) {
        boothAvailableTimes[i] = 0;
    }

    // process each queue based on the booth assignments
    for (int booth = 0; booth < b; booth++) {
        for (int queueIndex = 0; queueIndex < NUM_QUEUES; queueIndex++) {
            if (boothAssignments[queueIndex] == booth) {
                while (!isEmpty(queues[queueIndex])) {
                    struct Customer customer = dequeue(queues[queueIndex]);
                    int arrivalTime = customer.arrivalTime;
                    int processingTime = 30 + customer.tickets * 5;

                    // customer can only be processed after their arrival and after the booth is available
                    int startTime = (boothAvailableTimes[booth] > arrivalTime) ? boothAvailableTimes[booth] : arrivalTime;
                    int endTime = startTime + processingTime;
                    boothAvailableTimes[booth] = endTime; // Update the time the booth will be available next

                    // print the checkout details
                    printf("Customer %s from line %d goes to booth %d and checks out at time %d.\n",
                           customer.name, queueIndex + 1, booth + 1, endTime);
                }
            }
        }
    }
}
