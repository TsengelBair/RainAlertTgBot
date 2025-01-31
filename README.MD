## Телеграм бот уведомляющий о дожде

Данный бот отправляет уведомление о дожде (но только в случае, если он есть)

1) Запрос к открытому апи
2) Обработка json с извлечением погоды за текущий период
3) Интерфейс для сохранения/удаления пользователя в бд
4) Механизм отправки уведомлений всем подписанным пользователяем в определенное время
5) Деплой

### Запрос к открытому апи

В качестве api используется [openweathermap.org](https://openweathermap.org/api/one-call-api), причем именно бесплатная версия One Call Api 2.5

Запрос к данному api вернет json с погодой на 5 дней, каждый день разбит на три часа, соответственно объектов погоды будет 40.

**Пример возвращаемого json**

```json
{
  "city": {
    "coord": {
      "lat": 59.8944,
      "lon": 30.2642
    },
    "country": "RU",
    "id": 498817,
    "name": "Saint Petersburg",
    "population": 0,
    "sunrise": 1738217814,
    "sunset": 1738246045,
    "timezone": 10800
  },
  "cnt": 40,
  "cod": "200",
  "list": [
    {
      "clouds": {
        "all": 83
      },
      "dt": 1738227600,
      "dt_txt": "2025-01-30 09:00:00",
      "main": {
        "feels_like": 1.4,
        "grnd_level": 1005,
        "humidity": 91,
        "pressure": 1007,
        "sea_level": 1007,
        "temp": 5.14,
        "temp_kf": -0.23,
        "temp_max": 5.37,
        "temp_min": 5.14
      },
      "pop": 0,
      "sys": {
        "pod": "d"
      },
      "visibility": 10000,
      "weather": [
        {
          "description": "broken clouds",
          "icon": "04d",
          "id": 803,
          "main": "Clouds"
        }
      ],
      "wind": {
        "deg": 194,
        "gust": 11.39,
        "speed": 5.24
      }
    },
    {
      "clouds": {
        "all": 92
      },
      "dt": 1738238400,
      "dt_txt": "2025-01-30 12:00:00",
      "main": {
        "feels_like": 0.79,
        "grnd_level": 1005,
        "humidity": 92,
        "pressure": 1007,
        "sea_level": 1007,
        "temp": 4.96,
        "temp_kf": 0.03,
        "temp_max": 4.96,
        "temp_min": 4.93
      },
      "pop": 0.2,
      "rain": {
        "3h": 0.12
      },
      "sys": {
        "pod": "d"
      },
      "visibility": 10000,
      "weather": [
        {
          "description": "light rain",
          "icon": "10d",
          "id": 500,
          "main": "Rain"
        }
      ],
      "wind": {
        "deg": 201,
        "gust": 12.46,
        "speed": 6.14
      }
    },
}
```

Для отправки запроса используется класс RequestHandler, данный класс отправляет http запрос к апи и полученный json ответ передает в класс JsonHandler

```c++
nlohmann::json RequestHandler::getWeather()
{
    boost::asio::io_context ioc; /* объект ввода-вывода */
    boost::asio::ip::tcp::resolver resolver(ioc); /* resolver - преобразует домен а url */
    boost::asio::ip::tcp::socket socket(ioc);
    boost::asio::connect(socket, resolver.resolve(_host, "80"));
    /*
        тело запроса строка
        1 - get 
        2 - url
        3 - HTTP 1.1
    */    
    http::request<http::string_body> req(http::verb::get, _target, 11);           

    /* заголовок */
    req.set(http::field::host, _host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    http::write(socket, req);
    {
        boost::beast::flat_buffer buffer;
        http::response<http::dynamic_body>res;
        http::read(socket, buffer, res);

        auto jsonResponse = nlohmann::json::parse(boost::beast::buffers_to_string(res.body().data()));
        return jsonResponse;
    }                
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
}
```

### Обработка json с извлечением погоды за текущий период

Класс обработчик JsonHandler содержит один статический метод 

```c++
bool JsonHandler::checkIfRain(nlohmann::json& jsonObj)
{
    time_t curDateAndTime = time(0);
    tm* timeStruct = localtime(&curDateAndTime);

    timeStruct->tm_sec = 0;
    timeStruct->tm_min = 0;
    timeStruct->tm_hour = 0;

    /* текущая дата в секундах в GMT, но время полночь */
    time_t curDateMidnight = mktime(timeStruct);

    /* +3 часа для московского времени */
    time_t startTimeFromSixPM = curDateMidnight + 3600 * 9;
    time_t endTimeWhenNextDay = curDateMidnight + 3600 * 27;

    std::cout << "стартовое время " <<  startTimeFromSixPM << "\n";
    std::cout << "Конечное время " <<  endTimeWhenNextDay << "\n";

    for (int i = 0; i < jsonObj["list"].size(); ++i){
        const auto& forecastForThreeHours = jsonObj["list"][i];
        auto forecastTime = forecastForThreeHours["dt"];
        std::cout << "Время итерации " << i << " " << forecastTime << "\n";

        if (forecastTime >= startTimeFromSixPM && forecastTime <= endTimeWhenNextDay){
            std::cout << "Подходящий объект " << "\n"<< forecastForThreeHours;
            const auto& weatherArray = forecastForThreeHours["weather"];
            for (const auto& weather : weatherArray){
                if (weather["main"] == "Rain"){
                    return true;
                }
            }
        }
    }

    return false;
}
```

Получаем текущее время и преобразуем его в структуру, для того чтобы сбросить часы, минуты и секунды, после чего снова преобразуем в GMT формат (универсальный формат времени, отражающий количество секунд, прошедших с 1 января 1970)

В примере с json ответом выше, поле dt представлено как раз в формате GMT

```c++
    time_t curDateAndTime = time(0);
    tm* timeStruct = localtime(&curDateAndTime);

    timeStruct->tm_sec = 0;
    timeStruct->tm_min = 0;
    timeStruct->tm_hour = 0;

    /* текущая дата в секундах в GMT, но время полночь */
    time_t curDateMidnight = mktime(timeStruct);
```

Диапазон интересующей погоды это 06:00 - 00:00

```c++
    /* текущая дата (полночь) */
    time_t curDateMidnight = mktime(timeStruct);
    /* +6 часов, т.к. хотим получить 6 утра и еще +3 для московского времени */
    time_t startTimeFromSixPM = curDateMidnight + 3600 * 9;
    /* для полуночи +24 и +3 */
    time_t endTimeWhenNextDay = curDateMidnight + 3600 * 27;
```

В цикле итерируемся только по указанному диапазону времени и проверяем поле main, т.к. в случае дождя в нем будет лежать значение "Rain"

```c++
for (int i = 0; i < jsonObj["list"].size(); ++i){
        const auto& forecastForThreeHours = jsonObj["list"][i];
        auto forecastTime = forecastForThreeHours["dt"];
        std::cout << "Время итерации " << i << " " << forecastTime << "\n";

        if (forecastTime >= startTimeFromSixPM && forecastTime <= endTimeWhenNextDay){
            std::cout << "Подходящий объект " << "\n"<< forecastForThreeHours;
            const auto& weatherArray = forecastForThreeHours["weather"];
            for (const auto& weather : weatherArray){
                if (weather["main"] == "Rain"){
                    return true;
                }
            }
        }
    }
```

### Интерфейс для сохранения/удаления пользователя в бд

```c++
        bot.getEvents().onCommand("subscribe", [&bot, &dbManager](TgBot::Message::Ptr message) {
            if (dbManager.isUserExist(message->chat->id)) {
                bot.getApi().sendMessage(message->chat->id, "Вы уже подписаны на уведомления");
            } else {
                dbManager.addUser(message->chat->id);
                bot.getApi().sendMessage(message->chat->id, "Вы подписались на уведомления, для того, чтобы отписаться, отправьте команду /reject");
            }
        });

        bot.getEvents().onCommand("reject", [&bot, &dbManager](TgBot::Message::Ptr message) {
            if (!dbManager.isUserExist(message->chat->id)) {
                bot.getApi().sendMessage(message->chat->id, "Вы не были подписаны, желаете подписаться? /subscribe");
            } else {
                dbManager.removeUser(message->chat->id);
                bot.getApi().sendMessage(message->chat->id, "Уведомления больше не будут приходить, для того чтобы снова получать уведомления, отправьте /subscribe");
            }
        });
```

Взаимодействие с бд sqlite осуществляет класс DbManager

```c++
class DbManager
{
public:
    DbManager(const std::string& dbName);
    ~DbManager();

    void addUser(const int telegram_id);
    void removeUser(const int telegram_id);
    
    bool isUserExist(const int telegram_id);
    std::list<int>getAllUsers();

private:
    bool executeQuery(const std::string& query);
    void createTable();

private:
    sqlite3* _db; 
    std::string _dbName; 
};
```

### Создание рассылки всем подписанным пользователям, привязанной ко времени

Сам бот должен крутиться 24/7, чтобы была возможность добавлять/удалять пользователей, но непосредственно запрос на получение погоды должен выполняться один раз в 06:00 текущей даты.

Для этой цели используется класс Sheduler, идея в том, чтобы запустить в отдельном потоке в бесконечном цикле ежеминутную проверку текущего времени (плохой вариант, т.к. по хорошему это нужно настраивать на стороне сервера)

```c++
void Sheduler::run()
{
    while (running) {
        time_t curDateAndTime = time(0);
        tm* timeStruct = localtime(&curDateAndTime);
        /* это время рассылки, для теста задать любое удобное */
        if (timeStruct->tm_hour == 6 && timeStruct->tm_min == 0){
            std::string weatherMsg;

            bool rain = isRain();
            if (rain) weatherMsg = "Ожидается дождь";

            std::cout << "Отправляю пользователям погоду" << std::endl;

            if (!weatherMsg.empty()){
                auto users = db.getAllUsers();
                for (const auto& userId : users) {
                    bot.getApi().sendMessage(userId, weatherMsg);
                }       
            }
            /* чтобы избежать повторной отправки */
            std::this_thread::sleep_for(std::chrono::minutes(1));
        }
        /* ждем минуту перед следующей проверкой */
        std::this_thread::sleep_for(std::chrono::minutes(1));
    }
}
```


### Деплой

Бот будет задеплоен на виртуальную машину (Ubuntu), в качестве сервера используется Yandex Cloud

Зарегистрироваться на сервисе, далее вкладка Виртуальные Машины -> Создать

Настройка любая (можно взять минимальную, будет достаточно)

Во вкладке Доступ выбираем ssh подключение, после чего нужно будет сгенерировать ssh ключ

![1](/img/1.jpg)

Для генерации ssh ключа

```bash
ssh-keygen -t ed25519
```

После чего появятся два ключа (это скрытые файлы, для просмотра ls -a)

```bash
user@astra:~$ ls -a
.                        build-untitled1-Desktop-Debug  .dbus       .dotnet       .hgrc.save.2  .pki           .selected_editor  tmp                   .vscode               Загрузки
..                       build-untitled-Desktop-Debug   .designer   .fly          .local        .planshetarm   .sprut            UniversalRadarClient  .Xauthority           Изображения
.bash_history            .cache                         Desktop     .gitconfig    .mozilla      .profile       .ssh  
```

Переходим и открываем публичный ключ id_ed25519.pub 

```bash
user@astra:~/.ssh$ ls
id_ed25519  id_ed25519.pub  known_hosts
```

Теперь можем подключаться 

```bash
ssh логин_при_создании@ip_виртуальной_машины
```

Теперь необходимо установить все используемые пакеты (можно подсмотреть в CMakeLists.txt)

```bash
sudo apt install git g++ make binutils cmake libboost-system-dev libssl-dev zlib1g-dev libcurl4-openssl-dev libsqlite3-dev nlohmann-json-dev
```

Саму либу для тг бота забрать с гита и собрать

```bash
git clone https://github.com/reo7sp/tgbot-cpp
cd tgbot-cpp
cmake .
make -j4
sudo make install
```

После установки всех необходимых пакетов клонируем текущий репозиторий

```bash
git clone https://github.com/TsengelBair/RainAlertTgBot.git

mkdir build
cd build
cmake ..
make (опционально указать количество ядре -j)
/.main
```

Для того, чтобы бот крутился 24/7 без необходимости подключаться по ssh

```bash
nohup путь_до_исполняемого > output.log 2>&1 &
```

UPD

Если бот не работает, значит сервер перестал оплачиваться