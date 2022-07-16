/**
 * @file scheduler.c
 * @author Xavier Render
 * @date 2022-05-07
 * 
 * @brief this file contains six functions that implement the six disk scheduling algorithms 
 * 
 */

#include "scheduler.h"

int FCFS(const int *reqs, int n) {
    if (n <= 3)
        return 0;

    // get current disk position
    int curr = reqs[1];
    int steps = 0; // To keep count of steps
    int i; // For loop 

    // for each request
    for (i = 3; i < n; i++) {
        // move to the next request
        steps += abs(curr - reqs[i]);
        curr = reqs[i];
    }

    return steps;
}

int SSTF(const int *reqs, int n) {
    if (n <= 3)
        return 0;

    // get current disk position
    int curr = reqs[1];
    int *reqsCopy = malloc(n * sizeof(int));
    int i;

    for (i = 0; i < n; i++) {
        reqsCopy[i] = reqs[i];
    }

    reqsCopy[0] = -1;
    reqsCopy[1] = -1;
    reqsCopy[2] = -1;

    int steps = 0;
    while(1) {
        int minDistIndex = -1;
        int j;
        // find the closest request
        for (j = 0; j < n; j++) {
            if (reqsCopy[j] == -1)
                continue;
            if (minDistIndex == -1 || abs(curr - reqsCopy[j]) < abs(curr - reqsCopy[minDistIndex])) {
                minDistIndex = j;
            }
        }
        if (minDistIndex == -1)
            break;

        // move to the closest request
        steps += abs(curr - reqsCopy[minDistIndex]);
        curr = reqsCopy[minDistIndex];
        // set it to -1 to avoid it being chosen again
        reqsCopy[minDistIndex] = -1;
    }

    free(reqsCopy);
    return steps;
}

int SCAN(const int *reqs, int n) {
    if (n <= 3)
        return 0;

    int max = reqs[0] - 1;
    int curr = reqs[1];
    int prev = reqs[2];

    int largest = -1;
    int smallest = -1;

    // find the largest and smallest requests
    int i;
    for (i = 3; i < n; i++) {
        if (largest == -1 || reqs[i] > largest) {
            largest = reqs[i];
        }
        if (smallest == -1 || reqs[i] < smallest) {
            smallest = reqs[i];
        }
    }

    // if in forward direction, go from current to the end and then to the smallest
    if (curr > prev) {
        return (max - curr) + (max - smallest);
    }

    // if in backward direction, go from current to 0 and then the largest
    return curr + largest;
}


int LOOK(const int *reqs, int n) {
    if (n <= 3)
        return 0;

    int curr = reqs[1];
    int prev = reqs[2];

    int largest = -1;
    int smallest = -1;

    // find the largest and smallest requests
    int i;
    for (i = 2; i < n; i++) {
        if (largest == -1 || reqs[i] > largest) {
            largest = reqs[i];
        }
        if (smallest == -1 || reqs[i] < smallest) {
            smallest = reqs[i];
        }
    }

    // if in forward direction, go from current to the largest and then to the smallest
    if (curr > prev) {
        return (largest - curr) + (largest - smallest);    
    }

    // if in backward direction, go from current to the smallest and then the largest
    return (curr - smallest) + (largest - smallest);
}

int CSCAN(const int *reqs, int n) {
    if (n <= 3)
        return 0;

    int max = reqs[0] - 1;
    int curr = reqs[1];
    int prev = reqs[2];

    int smallestLargerThanCurrent = -1;
    int largestSmallerThanCurrent = -1;

    int i;

    for (i = 2; i < n; i++) {
        // find the smallest request larger than current
        if (smallestLargerThanCurrent == -1 || (reqs[i] < smallestLargerThanCurrent && reqs[i] > curr)) {
            smallestLargerThanCurrent = reqs[i];
        }
        // the largest request smaller than current
        if (largestSmallerThanCurrent == -1 || (reqs[i] > largestSmallerThanCurrent && reqs[i] < curr)) {
            largestSmallerThanCurrent = reqs[i];
        }
    }

    // if in forward direction, go from current to the maximum and then to the largest remaining
    if (curr > prev) {
        return (max - curr) + max + largestSmallerThanCurrent;
    }
    return (curr) + (max) + (max - smallestLargerThanCurrent);
}


int CLOOK(const int *reqs, int n) {
    if (n <= 3)
        return 0;

    int curr = reqs[1];
    int prev = reqs[2];

    int largest = -1;
    int smallest = -1;
    int smallestLargerThanCurrent = -1;
    int largestSmallerThanCurrent = -1;

    int i;
    for (i = 2; i < n; i++) {
        // find the largest and smallest requests
        if (largest == -1 || reqs[i] > largest) {
            largest = reqs[i];
        }
        if (smallest == -1 || reqs[i] < smallest) {
            smallest = reqs[i];
        }
        // find the smallest request larger than current and the largest request smaller than current
        if (smallestLargerThanCurrent == -1 || (reqs[i] < smallestLargerThanCurrent && reqs[i] > curr)) {
            smallestLargerThanCurrent = reqs[i];
        }
        if (largestSmallerThanCurrent == -1 || (reqs[i] > largestSmallerThanCurrent && reqs[i] < curr)) {
            largestSmallerThanCurrent = reqs[i];
        }
    }

    // if in forward direction, go from current to the largest and then to the smallest, and then to the largest remaining
    if (curr > prev) {
        return (largest - curr) + (largest - smallest) + (largestSmallerThanCurrent - smallest);
    }

    // if in backward direction, go from current to the smallest and then the largest, and then to the smallest remaining
    return (curr - smallest) + (largest - smallest) + (largest - smallestLargerThanCurrent);
}
