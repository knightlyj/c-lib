#include <stdlib.h>
#include <time.h>
#include "stdio.h"
#include "queue.h"
#include <iostream>
#include <vector>

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

int main(int argc, char **argv)
{
    bool success = true;
    srand(time(NULL));


    if (!test_poll_when_full<testStruct, 10>())
    {
        success = false;
        goto out;
    }

    //if (!test_poll_when_full<uint32_t, 50>())
    //{
    //    success = false;
    //    goto out;
    //}


    //if (!test_discard_when_full<testStruct, 256>())
    //{
    //    success = false;
    //    goto out;
    //}

    //if (!test_discard_when_full<uint32_t, 50>())
    //{
    //    success = false;
    //    goto out;
    //}

   /* if (!test_discard_random<testStruct, 10>())
    {
        success = false;
        goto out;
    }*/
/*
    if (!test_discard_random<uint32_t, 50>())
    {
        success = false;
        goto out;
    }
*/
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