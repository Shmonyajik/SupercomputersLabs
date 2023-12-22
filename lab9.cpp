#include <iostream>
#include <mpi.h>
#include <string>

// Функция для расшифровки сообщения с использованием шифра Цезаря
std::string decryptMessage(const std::string& encryptedMessage, int shift) {
    std::string decryptedMessage = encryptedMessage;

    // Перебираем каждый символ в сообщении
    for (char& ch : decryptedMessage) {
        // Проверяем, является ли символ буквой
        if (isalpha(ch)) {
            // Определяем базовый символ (A для заглавных, a для строчных)
            char base = isupper(ch) ? 'A' : 'a';
            // Расшифровываем символ с использованием шифра Цезаря
            ch = (ch - base - shift + 26) % 26 + base;
        }
    }

    return decryptedMessage;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int worldSize, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Проверяем корректность количества аргументов командной строки
    if (argc != 3) {
        // Выводим сообщение об использовании, если количество аргументов неверное
        if (rank == 0) {
            std::cerr << "Usage: " << argv[0] << " <encrypted_message> <shift_key>\n";
        }
        MPI_Finalize();
        return 1;
    }

    // Получаем зашифрованное сообщение и ключ сдвига из аргументов командной строки
    std::string encryptedMessage = argv[1];
    int shiftKey = std::stoi(argv[2]);

    // Рассчитываем размер и остаток для разделения сообщения между процессами
    int chunkSize = encryptedMessage.length() / worldSize;
    int remainder = encryptedMessage.length() % worldSize;

    // Рассчитываем размер и начальный индекс локальной части для каждого процесса
    int localSize = (rank < remainder) ? (chunkSize + 1) : chunkSize;
    int localStart = rank * chunkSize + std::min(rank, remainder);

    // Извлекаем локальную часть зашифрованного сообщения
    std::string localMessage = encryptedMessage.substr(localStart, localSize);

    // Расшифровываем локальную часть сообщения
    std::string decryptedLocalMessage = decryptMessage(localMessage, shiftKey);

    // Сбор результатов в процессе 0
    if (rank == 0) {
        std::string decryptedMessage = decryptedLocalMessage;

        // Собираем расшифрованные локальные части от других процессов
        for (int i = 1; i < worldSize; ++i) {
            MPI_Recv(&localSize, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            localMessage.resize(localSize);
            MPI_Recv(&localMessage[0], localSize, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            decryptedMessage += localMessage;
        }

        // Выводим расшифрованное сообщение
        std::cout << "Decrypted Message: " << decryptedMessage << std::endl;
    }
    else {
        // Отправка локальной расшифрованной части в процесс 0
        MPI_Send(&localSize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&decryptedLocalMessage[0], localSize, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}