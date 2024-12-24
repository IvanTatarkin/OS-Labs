#pragma once
#include <iostream>

/*
 * Функция Controller(std::istream& stream, bool test):
 * - Считывает команды (create, exec, pingall, exit)
 * - Управляет деревом узлов (через tools.h)
 * - Создает воркеры (Worker) через fork()
 */
void Controller(std::istream &stream, bool test);
