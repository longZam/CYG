class CircularQueue
{
private:
    float *pArr;
    int front, rear, count, N;

public:
    CircularQueue(int N);
    void Enqueue(float value);
    float Dequeue();
    int size();
};