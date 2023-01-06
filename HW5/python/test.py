import sys

argc = len(sys.argv)
print('read {} args'.format(argc))

for i, arg in enumerate(sys.argv):
    print('arg{}: {}, type: {}'.format(i, arg, type(arg) ))

