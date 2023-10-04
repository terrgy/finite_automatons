# Библиотека по формалкам

## Оглавление
[[_TOC_]]

## Сборка
Установите необходимые библиотеки
```shell
sudo apt-get install lcov openssl libssl-dev
```

Соберите проект CMake
```shell
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

Соберите проект
```shell
make -j<num_cores + 1>
```

Исполняемые файлы теперь можно найти в папке bin

## Построение НКА по регулярному выражению

| Исполняемый файл | build_nfa_from_regex               |
| ----- |------------------------------------|
| Ввод | Регулярное выражение одной строкой |
| Вывод | НКА в формате Dimple               |

Поддерживаемые операции для регулярных выражений

| Операция | Символ                    |
| ----- |---------------------------|
| Звезда Клини | *                         |
| Плюс Клини | %                         |
| Степень | ^<число>                  |
| Конкатенация | <пустота>                 |
| Сложение | +                         |
| Символы | Буквы латинского алфавита |

Пример
```text
(ab)*a%+((a+b)^2)*
```

## Удаление EPS-переходов

| Исполняемый файл | build_empty_edges_closure    |
| ----- |------------------------------|
| Ввод | НКА в формате Dimple |
| Вывод | НКА в формате Dimple |


## Тесты
Для выполнения сразу всех тестов, запустите bin/all_tests
Чтобы запустить каждый юнит-тест отдельно, запустите bin/*-test

## Покрытие тестами
Соберите покрытие тестами
```shell
make coverage_report -C build -j<num_cores + 1>
```

Теперь покрытие каждого юнит-теста можно найти в build/*-coverage
В каждой папке с покрытием будет index.html, откройте его в браузере