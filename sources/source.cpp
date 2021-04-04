// Copyright 2021 <rinamuka4@gmail.com>

#include <fstream>
#include <iostream>

#include "nlohmann/json.hpp"
#include "prepareSuggests.hpp"
#include <unistd.h>
using nlohmann::json;

auto get_id(const json& j) -> std::string { return j.get<std::string>(); }
auto get_name(const json& j) -> std::string { return j.get<std::string>(); }
auto get_cost(const json& j) -> int { return j.get<int>(); }

void from_json(const json& j, sug& s) {
    s.id = get_id(j.at("id"));
    s.name = get_name(j.at("name"));
    s.cost = get_cost(j.at("cost"));
}

void preparerSug::serveSuggestions() { //читает из файла каждый и обновляет раз в 15 минут
    while (true) {//бесконечный цикл
        mutex.lock();//для коллекции,т.к. серверу поступает много запросов от клиентов, и одновременно пытаются обратиться к коллекции
        std::ifstream file(filename_); //открываю файл
        suggestions.clear(); //очищаю перед обновление и заполняем новыми значениями
        if (filename_.empty())
            throw std::invalid_argument("file is empty");

        if (!file) { //если файла не существует то выводим сообщение о том что не можем открыть
            throw std::out_of_range{"unable to open json: " + filename_};
        }
        data.clear();// очищаю дату
        file >> data; //в дату пихаю обнавленным массивом
        file.close();
        for (auto const& item : data.at("suggestions")) { //иду по дате
            auto sugObj = item.get<sug>();//создаю объект типа саг и заново заполняю коллекцию
            suggestions.push_back(sugObj);
        }
        mutex.unlock();

        sleep(900);
    }
}

void to_json(json& j, const sugUnit& s) { //ф-ии чтобы распарсить джейсон
    j = json{{"text", s.text}, {"position", s.position}};
}

bool my_cmp(const sug& a, const sug& b) {
    // smallest comes first
    return a.cost > b.cost;
}

json preparerSug::getSuggestions(std::string input) { //клиент отправляет джейсон,передаем тело запроса
    auto j = json::parse(input); //создаем джисон и парсим джейсон клиента
    std::vector<json> goodSugs; //вектор джейсонов для отправки клиенту
    mutex.lock(); //перед обращение в коллекцию блокируем мутекс
    std::sort(suggestions.begin(), suggestions.end(), my_cmp); //сортировка коллекции по убыванию Коста
    for (size_t i = 0; i < suggestions.size(); ++i) {
        if (suggestions[i].id == j["input"].get<std::string>()) { //идем по каждому элементу коллекции, если Айди эл-та коллекции совпадает с инпутом отправленным клиентов
            json gsug; //создаем объект джисон
            sugUnit su = sugUnit{suggestions[i].name, i}; // создаем объект структуры сагюнит и инициализируем его поля при создании
            to_json(gsug, su);
            goodSugs.push_back(gsug);//гудсагс массив джейсонов который отправляем клиенту
        }
    }
    mutex.unlock();
    json ja;//объект джисон массив саджешенсов
    ja["suggestions"] = goodSugs; //ключу садешенс соответствует массив сагов
    // json j_goodSugs(goodSugs);

    return ja;
}