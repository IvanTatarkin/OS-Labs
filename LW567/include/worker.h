#pragma once

/*
 * Функция Worker(int id):
 *   Запускается в дочернем процессе (после fork()).
 *   Слушает команды на REP-сокете (порт 5555 + id).
 */
void Worker(int id);
