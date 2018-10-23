#include <stdlib.h>
#include <time.h>
#include "stdio.h"
#include "queue.h"
#include <iostream>
#include <vector>
#include <Windows.h>
#include "processthreadsapi.h"

using namespace std;

struct testStruct;

//满了就poll一个
template<typename T, unsigned int size>
bool test_poll_when_full()
{
    vector<T> vec;
    QUEUE_DEF_INIT(test_queue, size, T, NOTHING);

    for (int i = 0; i < 4096; i++) {
        T num = rand();
        //push to vector
        if (vec.size() >= size)
            vec.erase(vec.begin());
        vec.push_back(num);

        //push to queue
        T data;
        if (queue_is_full(&test_queue))
            queue_poll(&test_queue, &data);

        queue_add(&test_queue, &num);
    }

    if (vec.size() != queue_length(&test_queue)) {
        cout << "length not equal" << endl;
        return false;
    }

    T* data;
    int cnt = 0;
    QUEUE_FOR_EACH(&test_queue, data, T) {
        if (*data != vec[cnt++])
            return false;
    }

    return true;
}

//满了则丢弃
template<typename T, unsigned int size>
bool test_discard_when_full()
{
    vector<T> vec;
    QUEUE_DEF_INIT(test_queue, size, T, NOTHING);

    for (int i = 0; i < 4096; i++) {
        T num = rand();

        if (i % 10 == 1) {
            T vdata = vec[0];
            vec.erase(vec.begin());
            T data;
            queue_poll(&test_queue, &data);

            if (vdata != data) {
                return false;
            }
        }

        //push to vector
        if (vec.size() < size)
            vec.push_back(num);

        //push to queue
        if (!queue_is_full(&test_queue))
            queue_add(&test_queue, &num);
    }

    if (vec.size() != queue_length(&test_queue)) {
        cout << "length not equal" << endl;
        return false;
    }

    T* data;
    int cnt = 0;
    QUEUE_FOR_EACH(&test_queue, data, T) {
        if (*data != vec[cnt++])
            return false;
    }

    return true;
}

//随机入队列或丢弃
template<typename T, unsigned int size>
bool test_discard_random()
{
    vector<T> vec;
    QUEUE_DEF_INIT(test_queue, size, T, NOTHING);

    for (int i = 0; i < 4096; i++) {
        T num = rand();

        if ((num % 10) == 1 && i > 0) {
            T vdata = vec[0];
            vec.erase(vec.begin());
            T data;
            queue_poll(&test_queue, &data);

            if (vdata != data) {
                return false;
            }
        }

        //push to vector
        if (vec.size() < size)
            vec.push_back(num);

        //push to queue
        if (!queue_is_full(&test_queue))
            queue_add(&test_queue, &num);
    }

    if (vec.size() != queue_length(&test_queue)) {
        cout << "length not equal" << endl;
        return false;
    }

    T* data;
    int cnt = 0;
    QUEUE_FOR_EACH(&test_queue, data, T) {
        if (*data != vec[cnt++])
            return false;
    }


    return true;
}

struct testStruct
{
    int a;
    float b;

    testStruct() : a(0), b(0) {

    }

    testStruct(int n) {
        a = n;
        b = (float)n - 2;
    }

    bool operator==(testStruct& other) {
        return this->a == other.a && this->b == other.b;
    }

    bool operator!=(testStruct& other) {
        return this->a != other.a || this->b != other.b;
    }

    int operator%(int n) {
        return a % n;
    }
};

uint8_t testStr[] = "one line 1234\n";

QUEUE_DEF_INIT(testq, 256, uint8_t, );
uint32_t poll_counter = 0, add_counter = 0;
DWORD __stdcall thread_poll(LPVOID lpThreadParameter) {
    int index = 0;
    while (1) {
        
        uint8_t data;
        if (queue_poll(&testq, &data) == SUCCESS) {
            if (data != testStr[index]) {
                int i = 0;
            }
            printf("%c", data);

            index++;
            if (index >= 14) {
                index = 0;
            }

            poll_counter++;
        }
        
        Sleep(1);
    }
}




int main(int argc, char **argv)
{
    bool success = true;
    srand(time(NULL));

    static HANDLE thread = NULL;
    thread = CreateThread(NULL, 0, thread_poll, NULL, 0, NULL);

    while (1) {
        for (int i = 0; i < sizeof(testStr) - 1; i++) {
            queue_add(&testq, &testStr[i]);
            add_counter++;
        }
        Sleep(100);
    }

out:

    if (success) {
        cout << "all test ok" << endl;
        getchar();
        return 0;
    }
    else
    {
        cout << "some test failed" << endl;
        getchar();
        return -1;
    }
}