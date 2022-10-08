#include <stdlib.h>
#include <stdio.h>

class CircularQueue
{
private:
    float *pArr;
    int front, rear, count, N;

public:
    CircularQueue(int N)
    {
        pArr = (float *)malloc(sizeof(float) * N);
        front = rear = -1;
        this->N = N;
    }

    void Enqueue(float value)
    {
        if (count < N)
            count++;

        rear = (rear + 1) % N;
        *(pArr + rear) = value;
    }

    float Dequeue()
    {
        if (count > 0)
            count--;

        front = (front + 1) % N;
        return *(pArr + front);
    }

    int size()
    {
        return count;
    }
};