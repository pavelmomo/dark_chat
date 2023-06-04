#ifndef QUEUE_SAFE_H_INCLUDED
#define QUEUE_SAFE_H_INCLUDED
#include<queue>
#include<iostream>
#include<mutex>
#include<shared_mutex>
#include<queue>
#include<condition_variable>
#include<thread>

template <typename T>
class queue_safe
{private:
    std::queue <T> current_queue;
    static inline std::condition_variable_any cond_var;
    static inline std::mutex current_mutex;
public:

    void push(const T &value)
    {
        current_mutex.lock();
        current_queue.push(value);
        cond_var.notify_all();
        current_mutex.unlock();
    }

    unsigned int size()
    {
        current_mutex.lock();
        unsigned int queue_size = this->current_queue.size();
        current_mutex.unlock();
        return queue_size;
    }
    T pop_with_check_and_wait()
    {
        current_mutex.lock();

        T output_element;
        while(!current_queue.size())
        {
            cond_var.wait(current_mutex);
        }
        output_element = this->current_queue.front();
        //std::cout<<"Action: "<<output_element.action<<std::endl;
        current_queue.pop();
        current_mutex.unlock();
        return output_element;
    }

    void pop()
    {
        current_mutex.lock();
        this->current_queue.pop();
        current_mutex.unlock();
    }
    T front()
    {
        current_mutex.lock();
        T to_return = this->current_queue.front();
        current_mutex.unlock();
        return to_return;
    }
    T back()
    {
        current_mutex.lock();
        T to_return = this->current_queue.back();
        current_mutex.unlock();
        return to_return;
    }

    std::mutex get_mutex()
    {
        return current_mutex;
    }

};

#endif // QUEUE_SAFE_H_INCLUDED
