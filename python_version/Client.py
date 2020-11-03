from Server import SERVER_NAME, read_from_fifo, write_to_fifo


def register_user():
    my_fifo = None
    new_user = input('Welcome to the UAG MCC chat, type your name followed by your status: "OnLine" or "OffLine", '
                     'please use an slash "/" as separator, Example: "Alexis/OnLine"')
    if len(new_user.split('/')) <= 0 or len(new_user.split('/')) > 2:
        print('Please provide a valid User/State combination')
    else:
        my_fifo = str(new_user.split('/')[0])
        new_user = 'Register/{}'.format(new_user)
        write_to_fifo(SERVER_NAME, new_user)
    return my_fifo


def see_available_users(my_user):
    users_message = 'users'
    write_to_fifo(SERVER_NAME, users_message)
    # TODO: Maybe put a delay
    print(read_from_fifo(my_user))


def connect_with(connect_from):
    connect_to = input('Please type the name the user you want to talk with:\n')
    connect_message = 'connect/{0}/{1}'.format(connect_to, connect_from)
    write_to_fifo(SERVER_NAME, connect_message)


def exit_chat(this_fifo):
    print('Goodbye {}'.format(this_fifo))
    exit_message = 'exit'
    exit_message = '{0}/{1}'.format(exit_message, this_fifo)
    write_to_fifo(SERVER_NAME, exit_message)


if __name__ == '__main__':
    stay_in_loop = True

    # Registering into server
    client_fifo_name = register_user()

    while stay_in_loop:
        selection = input('Main Menu:\n1-See available users\n2-Connect with\n3-Exit')
        selection = int(selection)
        if selection == 1:
            see_available_users(client_fifo_name)
        elif selection == 2:
            connect_with(client_fifo_name)
        elif selection == 3:
            exit_chat(client_fifo_name)
            stay_in_loop = False
        else:
            print('Please choose a valid option')

