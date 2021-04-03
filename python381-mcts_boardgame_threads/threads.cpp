using namespace std;
#include "printf.h"
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

int buff[10];
int counter = 0;
bool worker_running = true;
mutex m;
condition_variable cv;

int nr_threads = 10;

void worker() {
  while (worker_running) {
    unique_lock<mutex> lk(m);
    cv.wait(lk, []{ return counter == nr_threads; });
    printf("worker starting\n");

    for (int i = 0; i < nr_threads; i++) {
      buff[i] = i + nr_threads;
    }
    counter = 0;
    printf("worker finished\n");
    lk.unlock();
    cv.notify_all();
  }
}

void do_stuff(int i) {
  {
    lock_guard<mutex> lk(m);
    counter += 1;
  }
  cv.notify_all();
  {
    unique_lock<mutex> lk(m);
    cv.wait(lk, []{ return counter == 0; });
  }
}
 
int main()
{
  thread worker_t(worker);
  vector<thread> threads;
  for (int i = 0; i < nr_threads; i++) threads.push_back(thread(do_stuff, i));
  // worker.join();
  for(thread &t : threads) {
    t.join();
  }
  printf("counter: %d\n", counter);
  for (int i = 0; i < nr_threads; i++) {
    printf("%d ", buff[i]);
  }
  printf("\n");
  worker_t.detach();
  return 0;
}