// MIT License
//
// Copyright (c) 2020 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Evan Pezent (epezent@rice.edu)

#include <Mahi/Util.hpp>
#include <thread>

using namespace mahi::util;

// Usage:
// Run example from command line as either of the following:
// Terminal: lockables mutex
// or
// Terminal: lockables spinlock
// or
// Terminal 1: lockables named_mutex_A
// Terminal 2: lockables named_mutex_B

void thread1_func(Lockable& lockable) {
    println("Thread 1: Entering. Aquiring lock!");
    Lock lock(lockable);
    sleep(milliseconds(10));
    prompt("Thread 1: Press enter to allow Thread 2 to continue.");
    println("Thread 1: Exiting");
}

void thread2_func(Lockable& lockable) {
    println("Thread 2: Entering. Can't continue, reached lock!");
    Lock lock(lockable);
    sleep(milliseconds(10));
    prompt("Thread 2: Press enter to allow Main to continue.");
    println("Thread 2: Exiting");
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        std::string id = argv[1];
        if (id == "mutex") {
            println("Main: Spawning Threads");
            Mutex mutex;
            std::thread thread1(thread1_func, std::ref(mutex));
            sleep(milliseconds(1));
            std::thread thread2(thread2_func, std::ref(mutex));
            thread1.join();    // main will wait for thread1 to complete
            thread2.detach();  // but continue despite thread2 completing or not
            println("Main: Can't continue, reached lock!");
            Lock lock(mutex);
            sleep(milliseconds(10));
            println("Main: Thanks!");
        } else if (id == "spinlock") {
            println("Main: Spawning Threads");
            Spinlock spinlock;
            std::thread thread1(thread1_func, std::ref(spinlock));
            sleep(milliseconds(1));
            std::thread thread2(thread2_func, std::ref(spinlock));
            thread1.join();    // main will wait for thread1 to complete
            thread2.detach();  // but continue despite thread2 completing or not
            println("Main: Can't continue, reached lock!");
            Lock lock(spinlock);
            sleep(milliseconds(10));
            println("Main: Thanks!");
        }
        else if (id == "named_mutex_A") {
            NamedMutex named_mutex("my_named_mutex");
            // lock the mutex using a Lock RAII wrapper
            Lock lock(named_mutex);
            prompt("Press ENTER to unlock the named_mutex");
        } else if (id == "named_mutex_B") {
            NamedMutex named_mutex("my_named_mutex");
            // you *can* lock and unlock a mutex like this, but this is not
            // exception safe and you run the risk of unlock never getting
            // called.
            named_mutex.lock();
            println("B can continue now");
            named_mutex.unlock();
        }
    }
    return 0;
}
