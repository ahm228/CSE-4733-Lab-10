#include <iostream>
#include <queue>
#include <thread>
#include <random>
#include <condition_variable>
#include <atomic>

std::queue<int> temperatureQueue;
std::mutex queueMutex;
std::condition_variable queueCV;
std::atomic<bool> isThread1Running(true);

bool shouldWakeUp()
{
    return !temperatureQueue.empty() || !isThread1Running;
}

void pushTemperature(int temperature)
{

    std::unique_lock<std::mutex> lock(queueMutex);
    queueCV.wait(lock, [] { return temperatureQueue.size() < 10; });

    temperatureQueue.push(temperature);
    std::cout << "Thread 1 pushed: " << temperature << std::endl;

    lock.unlock();
    queueCV.notify_all();
    // TODO (part 1):  
    //
    // Implement the logic to push temperature onto the queue
    // using a mutex and condition variable for synchronization.
    // Make sure to notify the consumer thread after pushing the temperature.
    // 
    //     1. Acquire the lock on the queueMutex using std::unique_lock.
    //     2. Wait until the condition variable queueCV is signaled and the size of the temperatureQueue is less than 10.
    //     3. Push the temperature value onto the temperatureQueue.
    //     4. Print a message indicating that Thread 1 pushed the temperature value onto the queue.
    //     5. Notify all threads waiting on the condition variable queueCV.


    // End TODO part 1

}

void thread1()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 50);

    for (int i = 0; i < 100; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        int temperature = dist(gen);
        pushTemperature(temperature);
    }

    isThread1Running = false;
    queueCV.notify_all();

    // TODO: (part 2)
    // 
	// 1. Set the variable i to 0.
        //
	// 2. Enter a loop while i is less than 100:
	//     a. Sleep for 100 milliseconds to simulate processing time.
	//     b. Generate a random temperature using the random number generator dist.
	//     c. Call the pushTemperature function with the generated temperature.
	//     d. Increment the value of i by 1.
        //
	// 3. Set the value of isThread1Running to false.
	// 4. Notify all threads waiting on the condition variable queueCV.


        // End TODO part 2

}

void convertTemperature(int temperature)
{
    // Convert temperature to Celsius
    double celsius = (temperature - 32) * 5 / 9.0;
    std::cout << "Thread 2 converted temperature: " << temperature << "F to " << celsius << "C" << std::endl;
}

void convertAndReportTemperature()
{

    while (isThread1Running || !temperatureQueue.empty())
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        queueCV.wait(lock, shouldWakeUp);

        while (!temperatureQueue.empty())
        {
            int temperature = temperatureQueue.front();
            temperatureQueue.pop();
            lock.unlock();
            convertTemperature(temperature);
            lock.lock();
        }
    }
    // TODO (part 3):
    //
    // 1. Enter a loop while the isThread1Running flag is true or the temperatureQueue is not empty.
    // 2. Acquire the lock on the queueMutex.
    // 3. Wait until the condition variable queueCV is signaled and either the temperatureQueue is not empty or isThread1Running is true.
    // 4. If the temperatureQueue is not empty:
    //     a. Retrieve the front temperature value from the temperatureQueue.
    //     b. Remove the front temperature value from the temperatureQueue.
    //     c. Release the lock on the queueMutex.
    //     d. Call the convertTemperature function with the retrieved temperature value.


    // End TODO part 3

}

int main()
{
    std::thread t1(thread1);
    std::thread t2(convertAndReportTemperature);

    t1.join();
    t2.join();

    std::cout << "Threads completed." << std::endl;

    return 0;
}
