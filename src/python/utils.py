
'''
    Copyright 2024 Shaoheng Guan sg35@sanger.ac.uk, Wellcome Sanger Institute
'''

import sys, os, time


DEBUG = True if sys.gettrace() else False


def check_and_mkdir(*args):
    """
        making dirs

        :Input
            - a list of paths of the directories 
    """
    i = 0
    while (i<len(args)):
        dir_path = args[i]        
        if not os.path.exists(dir_path):
            os.mkdir(dir_path)
        i += 1
    return 


def timer(func):
    def wrapper(*args, **kwargs):
        if DEBUG:
            time_stamp = time.perf_counter()
            result = func(*args, **kwargs)
            print(f"FUNC {func.__name__} finished in: ({time.perf_counter() - time_stamp:.3e}) S")
        else:
            result = func(*args, **kwargs)
        return result
    return wrapper