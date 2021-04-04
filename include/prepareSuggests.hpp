
#ifndef INCLUDE_PREPARESUGGESTS_HPP_
#define INCLUDE_PREPARESUGGESTS_HPP_
#include <shared_mutex>
#include <string>
#include <vector>
#include "nlohmann/json.hpp"
struct sug { //то что хранится в файле
    std::string id;
    std::string name;
    int cost;
};

struct sugUnit { //то что возвращаю клиенту
    std::string text;
    size_t position;
};

class preparerSug {
public:
    explicit preparerSug(std::string filename) : filename_(filename){} //передаем имя файла джейсон где массив саджешенсов,: это список инициализации

    void serveSuggestions(); //читает файл и обновляет коллекию каждые 15 минут, метод класса
    nlohmann::json getSuggestions(std::string input); //читает слово отправленное клиентом и ищет данные в коллекции,сортирует и возвращает массив джейсон

private:
    nlohmann::json data;

    std::shared_mutex mutex;//в мэйн потоке сервер,для обновления файла раз в 15 минут запускаю его в отдельный поток

    std::string filename_;
    std::vector<sug> suggestions;
};
#endif  // INCLUDE_PREPARESUGGESTS_HPP_