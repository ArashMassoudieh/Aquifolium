#ifndef COMMAND_H
#define COMMAND_H


class Command
{
    public:
        Command();
        virtual ~Command();
        Command(const Command& other);
        Command& operator=(const Command& other);

    protected:

    private:
};

#endif // COMMAND_H
