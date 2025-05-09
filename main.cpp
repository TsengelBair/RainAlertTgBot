#include <stdio.h>
#include <tgbot/tgbot.h>
#include <nlohmann/json.hpp>

#include "config.h"
#include "Db/dbhandler.h"
#include "Request/requesthandler.h"
#include "Boundary/timeboundarymaker.h"
#include "Parser/parser.h"
#include "Sheduler/sheduler.h"

int main() {
    TgBot::Bot bot(bot_token);
    DbHandler* db = nullptr;

    Sheduler sheduler(bot);
    sheduler.start();

    bot.getEvents().onCommand("start", [&bot, &db](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id,
        "Привет! Я отправляю подписанным пользователям предупреждения о дожде, чтобы подписаться на уведомления, отправьте команду /subscribe ");
        db = DbHandler::getInstance();
    });

    bot.getEvents().onCommand("subscribe", [&bot, &db](TgBot::Message::Ptr message) {
        if (db->userExist(message->chat->id)) {
            bot.getApi().sendMessage(message->chat->id, "Вы уже подписаны на уведомления");
        } else {
            db->addUser(message->chat->id);
            bot.getApi().sendMessage(message->chat->id, "Вы подписались на уведомления, для того, чтобы отписаться, отправьте команду /reject");
        }
    });

    bot.getEvents().onCommand("reject", [&bot, &db](TgBot::Message::Ptr message) {
        if (!db->userExist(message->chat->id)) {
            bot.getApi().sendMessage(message->chat->id, "Вы не были подписаны, желаете подписаться? /subscribe");
        } else {
            db->removeUser(message->chat->id);
            bot.getApi().sendMessage(message->chat->id, "Уведомления больше не будут приходить, для того чтобы снова получать уведомления, отправьте /subscribe");
        }
    });

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
