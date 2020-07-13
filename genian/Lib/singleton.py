# -*- coding: utf-8 -*-


class Singleton(type):
    _instances = {}

    def __call__(cls, *args, **kwargs):
        if cls not in cls._instances:
            cls._instances[cls] = super(Singleton, cls).__call__(*args, **kwargs)
        return cls._instances[cls]


# class Database(object):
#     __metaclass__ = Singleton
#
#     def __init__(self):
#         self.value = 0
#
# if __name__ == '__main__':
#     instance1 = Database()
#     instance2 = Database()
#
#     if instance1 == instance2:
#         print("same")
#     else:
#         print("differ")
#
#     instance1.value = 1
#     print(instance1.value, instance2.value)