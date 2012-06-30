#include "givecommand.h"


GiveCommand::GiveCommand(Player *player, QObject *parent) :
    Command(player, parent) {

    setDescription("Give an item or gold from your inventory to another "
                   "character.\n"
                   "\n"
                   "Example: give stick earl");
}

GiveCommand::~GiveCommand() {
}

void GiveCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Give what?")) {
        return;
    }

    takeWord("the");

    GameObjectPtrList items;
    double gold = 0.0;

    QString word = takeWord();
    if (word.startsWith("$")) {
        QRegExp currencyRegExp("\\$\\d+(\\.5)?");
        if (!currencyRegExp.exactMatch(word)) {
            send(QString("%1 is not a valid currency description.").arg(word));
            return;
        }
        gold = word.mid(1).toDouble();
        if (gold == 0.0) {
            if (player()->gender() == "male") {
                send("Be a gentleman, and give at least some gold.");
            } else {
                send("Be a lady, and give at least some gold.");
            }
            return;
        }
        if (gold > player()->gold()) {
            send("You don't have that much gold.");
            return;
        }
    } else {
        prependWord(word);

        items = takeObjects(player()->inventory());
        if (!requireSome(items, "You don't have that.")) {
            return;
        }
    }

    takeWord("to", IfNotLast);

    GameObjectPtrList recipients = takeObjects(currentArea()->characters());
    if (!requireSome(recipients, "That recipient is not here.")) {
        return;
    }

    Character *recipient = recipients[0].cast<Character *>();
    QString description;

    if (gold > 0.0) {
        recipient->adjustGold(gold);
        player()->adjustGold(-gold);

        description = word;
    } else {
        for (const GameObjectPtr &item : items) {
            recipient->addInventoryItem(item);
            player()->removeInventoryItem(item);
        }

        description = items.joinFancy();
    }

    QString recipientName = recipient->definiteName(recipients);
    send(QString("You give %1 to %2.").arg(description, recipientName));
    recipient->send(QString("%1 gives you %2.").arg(player()->name(), description));

    GameObjectPtrList others = currentArea()->players();
    others.removeOne(player());
    others.removeOne(recipient);
    others.send(QString("%1 gives %2 to %3.")
                .arg(player()->name(), description, recipientName));
}
