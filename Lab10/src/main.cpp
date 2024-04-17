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

    std::unique_lock<std::mutex> lock(queueMutex); // Acquire the lock on the queueMutex using std::unique_lock.
    queueCV.wait(lock, [] { return temperatureQueue.size() < 10; }); // Wait until the condition variable queueCV is signaled and the size of the temperatureQueue is less than 10.


    temperatureQueue.push(temperature); // Push the temperature value onto the temperatureQueue.
    std::cout << "Thread 1 pushed: " << temperature << std::endl;

    lock.unlock();
    queueCV.notify_all(); // Notify all threads waiting on the condition variable queueCV.

}

void thread1()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 50);

    for (int i = 0; i < 100; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Sleep for 100 milliseconds to simulate processing time.
        int temperature = dist(gen); // Generate a random temperature using the random number generator dist.
        pushTemperature(temperature); // Call the pushTemperature function with the generated temperature.
    }

    isThread1Running = false; // Set the value of isThread1Running to false.
    queueCV.notify_all(); // Notify all threads waiting on the condition variable queueCV.

}

void convertTemperature(int temperature)
{
    // Convert temperature to Celsius
    double celsius = (temperature - 32) * 5 / 9.0;
    std::cout << "Thread 2 converted temperature: " << temperature << "F to " << celsius << "C" << std::endl;
}

void convertAndReportTemperature()
{

    while (isThread1Running || !temperatureQueue.empty()) // Enter a loop while the isThread1Running flag is true or the temperatureQueue is not empty.
    {
        std::unique_lock<std::mutex> lock(queueMutex); // Acquire the lock on the queueMutex.
        queueCV.wait(lock, shouldWakeUp);

        while (!temperatureQueue.empty()) // Wait until the condition variable queueCV is signaled and either the temperatureQueue is not empty or isThread1Running is true.
        {
            int temperature = temperatureQueue.front(); // Retrieve the front temperature value from the temperatureQueue.
            temperatureQueue.pop(); // Remove the front temperature value from the temperatureQueue.
            lock.unlock(); // Release the lock on the queueMutex.
            convertTemperature(temperature); // Call the convertTemperature function with the retrieved temperature value.
            lock.lock();
        }
    }

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
