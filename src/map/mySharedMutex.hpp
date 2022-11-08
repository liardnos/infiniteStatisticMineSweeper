#include <mutex>

#ifndef MYSHAREDMUTEX_HPP_
#define MYSHAREDMUTEX_HPP_

class MySharedMutexKey {

};

class MySharedMutex {
    std::mutex _read_count_mutex;
    int _read_count = 0;
    std::mutex _read;
    std::mutex _write;
public:
    void lock_write(){
        _read.lock();
        _write.lock();
        
    }

    void unlock_write(){
        _write.unlock();
        _read.unlock();
    }   

    void try_lock_write(){
        
    }
    
    void lock_read(){
        _read.lock();
        _read_count_mutex.lock();
        _read_count++;
        if (_read_count == 1)
            _write.lock();
        _read_count_mutex.unlock();
        _read.unlock();
    }

    void unlock_read(){
        _read_count_mutex.lock();
        _read_count--;
        if (_read_count <= 0)
            _write.unlock();
        _read_count_mutex.unlock();
    }

    void try_lock_read(){
        
    }
};

#endif