#include <omp.h>
#include <iostream>
#include <chrono> 

// Функция для проверки, является ли число простым
bool isPrime(int n)
{
    // Угловой случай: 0 и 1 не являются простыми
    if (n <= 1)
        return false;

    // Проверка делителей от 2 до n-1 
    for (int i = 2; i < n; i++)
        if (n % i == 0)
            return false;

    return true;
}

int main() {
    // Задаем размер массива
    const unsigned long array_size = 99999;

    // Инициализируем массивы для чисел и результатов
    int numbers[array_size];
    bool primes_omp[array_size];
    bool primes_single[array_size];
    int i;

    // Заполняем массив числами от 1 до array_size
    for (int i = 0; i < array_size; i++) {
        numbers[i] = i + 1;
    }

    // Выводим максимальное количество потоков, доступное для использования
    std::cout << "Max threeds count: " << omp_get_max_threads() << std::endl;

    // Устанавливаем количество потоков для использования в параллельном регионе OpenMP
    if (omp_get_max_threads() < 4) {
        omp_set_num_threads(omp_get_max_threads());
    }
    else {
        omp_set_num_threads(8);
    }

    // Измеряем время выполнения параллельного региона с использованием OpenMP
    auto start_omp = std::chrono::high_resolution_clock::now();
#pragma omp parallel
    for (i = 0; i < array_size; i++) {
        primes_omp[i] = isPrime(numbers[i]);
        // Раскомментируйте строку ниже, чтобы выводить статус простоты для каждого числа
        //std::cout << numbers[i] << " - is prime number: "  << std::boolalpha << primes_omp[i] << std::endl;
    }
    auto stop_omp = std::chrono::high_resolution_clock::now();
    auto duration_omp = std::chrono::duration_cast<std::chrono::milliseconds>(stop_omp - start_omp);

    // Выводим время выполнения параллельного региона OpenMP
    std::cout << "Processing time using OpenMP: " << duration_omp.count() << " ms" << std::endl;

    // Измеряем время выполнения вычислений в одном потоке
    auto start_single = std::chrono::high_resolution_clock::now();
    for (i = 0; i < array_size; i++) {
        primes_single[i] = isPrime(numbers[i]);
        // Раскомментируйте строку ниже, чтобы выводить статус простоты для каждого числа
        //std::cout << numbers[i] << " - is prime number: "  << std::boolalpha << primes_single[i] << std::endl;
    }
    auto stop_single = std::chrono::high_resolution_clock::now();
    auto duration_single = std::chrono::duration_cast<std::chrono::milliseconds>(stop_single - start_single);

    std::cout << "Processing time using single thread: " << duration_single.count() << " ms" << std::endl;

    return(0);

}
