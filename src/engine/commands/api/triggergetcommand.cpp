#include "triggergetcommand.h"


#define super ApiCommand

TriggerGetCommand::TriggerGetCommand(Player *player, QObject *parent) :
    super(player, parent) {

    setDescription("Syntax: api-trigger-get <request-id> <object-id> <trigger-name>");
}

TriggerGetCommand::~TriggerGetCommand() {
}

void TriggerGetCommand::execute(const QString &command) {

    super::execute(command);

    GameObjectPtr object = takeObject(currentArea()->objects());
    if (object.isNull()) {
        sendError(404, "Object not found");
        return;
    }

    QString triggerName = takeWord().toLower();
    if (!triggerName.startsWith("on")) {
        sendError(400, "Invalid trigger name");
        return;
    }

    ScriptFunction trigger = object->trigger(triggerName);

    QVariantMap data;
    data["id"] = object->id();
    data["triggerSource"] = trigger.source;
    sendReply(data);
}