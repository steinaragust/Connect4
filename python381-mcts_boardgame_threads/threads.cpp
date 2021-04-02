using namespace std;
#include <iostream>
#include <condition_variable>
#include <thread>
#include <chrono>

void printing(int &i) {
  printf("%d\n", i);
}
 
int main()
{
  int a = 10;
  thread t1;
  t1 = thread(printing, ref(a));
  t1.join();
}