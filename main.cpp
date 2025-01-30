#include <iostream>
#include <tgbot/tgbot.h>
#include "Db/dbmanager.hpp"
#include "Sheduler/sheduler.hpp"
#include "config.hpp"

const std::string dbName = "test.db";

int main()
{
    TgBot::Bot bot(bot_token);
    DbManager dbManager(dbName);

    Sheduler sheduler(bot, dbManager);
    sheduler.start();

    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Привет! Я отправляю подписанным пользователям предупреждения о дожде, чтобы подписаться на уведомления, отправьте команду /subscribe ");
    });

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

    /* Запуск */
    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    } catch (TgBot::TgException& e) {
        printf("error: %s\n", e.what());
    }

    sheduler.stop();

    return 0;
}


