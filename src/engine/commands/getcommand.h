#ifndef GETCOMMAND_H
#define GETCOMMAND_H

#include "command.h"


class GetCommand : public Command {

    Q_OBJECT

    public:
        GetCommand(Player *player, QObject *parent = 0);
        virtual ~GetCommand();

        virtual void execute(const QString &command);
};

#endif // GETCOMMAND_H
