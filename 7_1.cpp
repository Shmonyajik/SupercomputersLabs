#include <iostream>
#include <math.h>
#include <omp.h>
#define N 10
int main(int argc, char** argv) {
    double x[N],y[N];
	
    x[0] = 2;
    y[0] = 2;
	
    omp_set_num_threads(2);

	//Critiacal чтобы избежать гонок за данными
    #pragma omp criticial
    for (int i=1; i<N; i++) {      
		x[i] = x[i-1]*x[i-1];
		y[i] = x[i] /y[i-1];
		int tid = omp_get_thread_num();
		std::cout << "Thread №" << tid << " " <<x[i] << std::endl;

     }

    y[0]=x[N-1]; 
    
    return 0;
}
// Переменные x и y используются как общие для всех потоков. Внутри цикла идет обращение к элементам этих массивов с использованием индексов, что может привести к гонке за данными (race condition). В данном случае, значения элементов зависят от предыдущих значений, и при параллельном выполнении могут возникнуть проблемы.

// Для избежания гонок за данными, можно воспользоваться директивой critical или реорганизовать логику так, чтобы каждый поток работал с независимыми порциями данных.

// Зависимость между итерациями цикла:
// Ваш цикл содержит зависимость между итерациями: y[i] зависит от y[i - 1]. При использовании nowait вы не обеспечиваете правильной синхронизации между потоками, и результаты могут быть неправильными. Например, один поток может выполняться быстрее другого и использовать значение y[i - 1], которое еще не было вычислено другим потоком.

// В данном случае, лучше использовать #pragma omp barrier после цикла, чтобы гарантировать завершение всех потоков перед продолжением выполнения.