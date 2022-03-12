# SClock
Вдохновлено: https://github.com/AlexGyver/MeteoClock

Автор: Правдин Р.В. pravdin.rv@icloud.com

Описание: Настольные часы с датчиками температуры, влажности, давления, углекислого газа, а также с функцией будильника и возможностью настройки яркости экрана и светодиода.

Аппаратная часть: 1) Микроконтроллер ATMega328p
                  2) Дисплей LCD 2004
                  3) Датчик температуры, влажности и давления BME
                  4) Датчик углекислого газа
                  5) Часы реального времени RTC
                  6) Энкодер
                  7) Кнопка
                  8) Светодиод
                  9) Фоторезистор
                 10) Аккумуляторы (при желании)

Возможности: - Отображение времени, даты, текущие температуру, влажность, атмосферное давление, уровень углекислого газа (СО2).
             - Возможно установить до 3х будильников с тремя мелодиями на выбор.
             - Настройка яркости дисплея и светодиода, в том числе в зависимости от освещенности помещения (день/ночь).
             - Ручная настройка часов.

Управление: 
              Основной экран
- Зажать энкодер - Перейти на экран настроек
- Двойное нажатие на энкодер - Перейти на экран установки будильника
- Вращение энкодера - Регулировка яркости дисплея и светодиода

          Экран настройки будильника
- Вращение энкодера - Изменение выбранного параметра
- Нажатие на энкодер - Переключить на следующий параметр
- Двойное нажатие на энкодер - Записать параметры будильника
- Зажать сенсорную кнопку - Вернуться на основаной экран

              Экран настроек
- Вращение энкодера - Изменение выбранного параметра или его значения
- Нажатие на энкодер - Войити/выйти в режим изменения настройки
- Зажать энкодер - Сохранить значение параметра
- Зажать сенсорную кнопку - Вернуться на основаной экран
