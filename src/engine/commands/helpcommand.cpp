#include "helpcommand.h"

#include "engine/util.h"


HelpCommand::HelpCommand(Player *character, const QMap<QString, Command *> &commands,
                         QObject *parent) :
    Command(character, parent),
    m_commands(commands) {

    setDescription("Show in-game help, like the one you are now reading.\n"
                   "\n"
                   "Examples: help, help buy");

    if (player()->isAdmin()) {
        m_triggers.insert("onactive : void",
                          "The onactive trigger is invoked on any character when it's no longer "
                          "stunned, ie. when it can perform a new action again.");
        m_triggers.insert("onattack(attacker : character) : bool",
                          "The onattack trigger is invoked on any character when it's being "
                          "attacked.");
        m_triggers.insert("onbuy(buyer : character, boughtItem : optional item) : bool",
                          "The onbuy trigger is invoked on any character when something is being "
                          "bought from it. When boughtItem is omitted, the buyer is requesting an "
                          "overview of the things for sale.");
        m_triggers.insert("oncharacterattacked(attacker : character, defendant : character) : void",
                          "The oncharacterattacked trigger is invoked on any character in an area, "
                          "except for the attacker and defendant themselves, when another "
                          "character in that area emerges into combat.");
        m_triggers.insert("oncharacterdied(defendant : character, attacker : optional character) : "
                          "bool",
                          "The oncharacterdied trigger is invoked on any character in an area, "
                          "when another character in that area dies. When attacker is omitted, the "
                          "defendant died because of a non-combat cause (for example, poison).");
        m_triggers.insert("oncharacterentered(activator : character) : void",
                          "The oncharacterentered trigger is invoked on any character in an area "
                          "when another character enters that area.");
        m_triggers.insert("onclose(activator : character) : bool",
                          "The onclose trigger is invoked on any item or exit when it's closed.");
        m_triggers.insert("ondie(attacker : optional character) : bool",
                          "The ondie trigger is invoked on any character when it dies. When "
                          "attacker is omitted, the character died because of a non-combat cause "
                          "(for example, poison).");
        m_triggers.insert("ondrink(activator : character) : bool",
                          "The ondrink trigger is invoked on any item when it's drunk.");
        m_triggers.insert("oneat(activator : character) : bool",
                          "The ondrink trigger is invoked on any item when it's eaten.");
        m_triggers.insert("onenter(activator : character) : bool",
                          "The onenter trigger is invoked on any exit when it's entered.");
        m_triggers.insert("onentered : void",
                          "The onentered trigger is invoked on any character when it entered a new "
                          "area.");
        m_triggers.insert("oncharacterexit(activator : character, exitName : string) : bool",
                          "The onexit trigger is invoked on any character in an area when another "
                          "character leaves that area.");
        m_triggers.insert("oninit : void",
                          "The oninit trigger is invoked once on every object when the game server "
                          "is started. Note: For characters that do have an onspawn trigger, but "
                          "no oninit trigger, onspawn is triggered instead.");
        m_triggers.insert("onopen(activator : character) : bool",
                          "The onopen trigger is invoked on any item or exit when it's opened.");
        m_triggers.insert("onreceive(giver : character, item : item or item list or amount) : bool",
                          "The onreceive trigger is invoked on any character when something is "
                          "being given to it. Note that item may be a number instead of an item "
                          "object when an amount of gold is being given, or a list of items when "
                          "multiple items are being given.");
        m_triggers.insert("onshout(activator : character, message : string) : void",
                          "The onshout trigger is invoked on any character when it hears someone "
                          "shout.");
        m_triggers.insert("onspawn : void",
                          "The onspawn trigger is invoked on any character when it respawns.");
        m_triggers.insert("ontalk(speaker : character, message : string) : void",
                          "The ontalk trigger is invoked on any character when talked to.");
        m_triggers.insert("onuse(activator : character) : void",
                          "The onuse trigger is invoked on any item when it's used.");
    }
}

HelpCommand::~HelpCommand() {
}

void HelpCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();

    QString m;

    if (hasWordsLeft()) {
        QString commandName = takeWord();

        if (commandName == "commands") {
            QStringList commandNames = m_commands.keys();

            m = "\n"
                "Here is a list of all the commands you can use:\n"
                "\n" +
                formatColumns(commandNames.filter(QRegExp("^\\w+$"))) +
                "\n"
                "Type *help <command>* to see help about a particular command.";

            if (player()->isAdmin()) {
                m += "\n"
                     "To see all the admin commands you can use, type *help admin-commands*.";
            }
        } else {
            if (m_commands.contains(commandName)) {
                m = "\n" +
                    Util::highlight(commandName) + "\n  " +
                    Util::splitLines(m_commands[commandName]->description(), 78).join("\n  ") +
                    "\n\n";
            } else if (player()->isAdmin()) {
                m = showAdminHelp(commandName);
            }

            if (m.isEmpty()) {
                m = QString("The command \"%1\" is not recognized.\n"
                            "Type *help commands* to see a list of all commands.").arg(commandName);
            }
        }
    } else {
        m = "\n"
            "Type *help commands* to see a list of all commands.\n"
            "Type *help <command>* to see help about a particular command.";
    }

    send(Util::processHighlights(m));
}

QString HelpCommand::showAdminHelp(const QString &commandName) {

    QString m;

    if (commandName == "admin-commands") {
        QStringList commandNames = m_commands.keys();

        m = "\n"
            "Here is a list of all the commands you can use as an admin:\n"
            "\n"
            "*Remember: With great power comes great responsibility!*\n"
            "\n" +
            formatColumns(commandNames.filter("-")) +
            "\n"
            "Type *help <command>* to see help about a particular command.\n"
            "Type *help admin-tips* to see some general tips for being an admin.";

    } else if (commandName == "admin-tips") {

        m = "\n"
            "*Admin Tips*\n"
            "\n"
            "Now that you are an admin, you can actively modify the game world. Obviously, great "
            "care should be taken, as many modifications are not revertable. Now, trusting you "
            "will do the right thing, here as some tips for you:\n"
            "\n"
            "When referring to an object, you can use *#<id>* rather than referring to them by "
            "name. For example, *set-prop #35 description This is the object with ID 35.*.\n"
            "\n"
            "Similarly, you can always use the word *area* to refer to the current area you are "
            "in. But, to make editing of areas even easier, you can use *@<property>* as a "
            "shortcut for *get-* or *set-prop area <property>*. Thus, you can simply type: "
            "*@id* to get the ID of the current area. Or, to set the description: *@description As "
            "you stand just outside the South Gate, ...*\n"
            "\n"
            "Not listed in the admin commands overview, but very useful: *edit-prop* and "
            "*edit-trigger* are available for more convenient editing of properties and triggers. "
            "The usage is the same as for *get-prop* and *get-trigger*. Note that these commands "
            "are only available if you use the web interface (not supported when using telnet).\n"
            "\n";

    } else if (commandName == "triggers") {

        m = "\n"
            "*Triggers*\n"
            "\n"
            "Writing triggers is pretty straightforward, but there are a few things you should "
            "know. Most importantly, any trigger is just a JavaScript function. So if you know "
            "JavaScript, you will be able to write triggers just fine. Still, there are some "
            "differences from writing JavaScript in a webbrowser of course. Here's an example of a "
            "very basic trigger:\n"
            "\n"
            "    function() {\n"
            "        this.go(this.currentArea.exits.named('north'));\n"
            "    }\n"
            "\n"
            "The above example is a trigger that works on a character, and will let him walk "
            "north, if possible.\n"
            "\n"
            "*Properties and methods*\n"
            "\n"
            "While writing some trigger you will often be interested to know which properties and "
            "methods are available on some object. In the example above, go() is a method that's "
            "available on character objects, currentArea is a property available on character "
            "objects, and exits is a property available on area objects. To inspect an object and "
            "list its properties or methods, use the *list-props* or *list-methods* commands, "
            "respectively.\n"
            "\n"
            "*Timers*\n"
            "\n"
            "Ever so often, you will want to write a trigger that performs some action in a "
            "delayed fashion. Possibly you will want to have that action be repeated at an "
            "interval. For these purposes, there are setTimeout() and setInterval() functions "
            "which work just like those in your webbrowser, with one difference only: The "
            "functions are attached to any game object rather than to the global window object as "
            "you may be used to. So, to make the character from above walk north with a delay of "
            "half a second, we could write this trigger:\n"
            "\n"
            "    function() {\n"
            "        this.setTimeout(function() {\n"
            "            this.go(this.currentArea.exits.named('north'));\n"
            "        }, 500);\n"
            "    }\n"
            "\n"
            "*Return values*\n"
            "\n"
            "In many cases, triggers have the ability to cancel the action that triggered them. "
            "Whenever this is the case, you can have your trigger return the value false, and the "
            "action will be canceled. Canceling the action is supported by any trigger that lists "
            "the bool return type in the overview given below. For example, if the following "
            "function is attached to an item's onopen trigger, the item cannot be opened:\n"
            "\n"
            "    function(activator) {\n"
            "        activator.send('The lid appears to be stuck.');\n"
            "        return false;\n"
            "    }\n"
            "\n"
            "*Testing scripts*\n"
            "\n"
            "If you simply want to test whether some script works as expected, you can use the "
            "*exec-script* command. In addition to testing, this command is also useful to clean "
            "up game data and objects when some script had unintended side-effects. If you want to "
            "execute some script on a specific object, you can use this function for getting the "
            "object: *$('<object-type>:<object-id>')*. Example:\n"
            "\n"
            "    exec-script $('character:167').say('Miauw')\n"
            "\n"
            "*Trigger Overview*\n"
            "\n"
            "Here is a list of all the triggers which are available:\n"
            "\n";
        for (const QString &triggerName : m_triggers.keys()) {
            m += "  " + Util::highlight(triggerName) + "\n";
        }
        m += "\n"
             "Type *help <trigger>* to see help about a particular trigger.\n";

    } else {
        for (const QString &triggerName : m_triggers.keys()) {
            if (triggerName.startsWith(commandName)) {
                m = "\n" +
                    Util::highlight(triggerName) + "\n  " +
                    Util::splitLines(m_triggers[triggerName], 78).join("\n  ") +
                    "\n\n";
                break;
            }
        }
    }

    return m;
}

QString HelpCommand::formatColumns(const QStringList &commandNames) {

    QString message;

    int length = commandNames.length();
    int halfLength = length / 2 + length % 2;
    for (int i = 0; i < halfLength; i++) {
        QString first = commandNames[i];
        QString second = i + halfLength < length ? commandNames[i + halfLength] : "";

        message += "  " + Util::highlight(first.leftJustified(30)) +
                   "  " + Util::highlight(second) + "\n";
    }

    return message;
}