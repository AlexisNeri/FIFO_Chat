import os
from pathlib import Path

SERVER_NAME = 'server'
AVAILABLE_LIST = []


def create_fifo(fifo_name):
    """
    This function creates the FIFO file
    :param fifo_name: Specifies the name the fifo file will have
    :return: The absolute location of the FIFO file
    """
    ret = ''
    fifo_path = os.path.join(os.getcwd(), fifo_name)
    fifo_mode = 0o666
    # Deleting existing FIFOs in order to avoid conflicts
    if os.path.exists(fifo_path):
        print('FIFO file already exist, proceeding to delete it')
        os.remove(fifo_path)
    os.mkfifo(fifo_path, fifo_mode)
    # Verifying if fifo was successfully created
    if os.path.exists(fifo_path):
        ret = fifo_path
    else:
        print('Something weird happen and FIFO was not created')
        raise FileNotFoundError

    return ret


def read_from_fifo(fifo):
    """
    :param fifo: Absolute route of fifo file to be read
    :return: Message string
    """
    file_descriptor = open(fifo, 'r')
    user_entry = file_descriptor.read()
    file_descriptor.close()
    if len(user_entry) > 1:
        return user_entry


def write_to_fifo(fifo, message):
    if os.path.exists(fifo):
        file_descriptor = open(fifo)
        file_descriptor.write(message)
        file_descriptor.close()
    else:
        print('FIFO does not exist, please provide an existing one')
        raise FileNotFoundError


def parse_message(message):
    message = message.split('/')
    if 'register' in message[0].lower():
        pass
    elif 'users' in message[0].lower():
        pass
    elif 'connect' in message[0].lower():
        pass
    elif 'exit' in message[0].lower():
        pass
    else:
        pass


if __name__ == '__main__':
    print('Creating server FIFO')
    create_fifo(SERVER_NAME)

