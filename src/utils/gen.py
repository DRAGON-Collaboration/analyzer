
def gen_is_valid (npar):
    func =  'template <'
    for i in range(0, npar):
        func += 'class T' + str(i) + ', '
    func = func.rstrip(', ')
    func += '>\n'

    func += 'inline bool is_valid ('
    for i in range(0, npar):
        func += 'const T' + str(i) + '& t' + str(i) + ', '
    func = func.rstrip(', ')
    func += ')\n{\n  return ( '

    for i in range(0, npar):
        func += 't' + str(i) + ' != dragon::NO_DATA && '
    func = func.rstrip(' && ')
    func += ' );\n}'
    print func;

def gen_reset (npar):
    func =  'template <'
    for i in range(0, npar):
        func += 'class T' + str(i) + ', '
    func = func.rstrip(', ')
    func += '>\n'

    func += 'inline void reset_data ('
    for i in range(0, npar):
        func += 'T' + str(i) + '& t' + str(i) + ', '
    func = func.rstrip(', ')
    func += ')\n{\n'

    for i in range(0, npar):
        func += '  t' + str(i) + ' = (T' + str(i) + ')dragon::NO_DATA;\n'
    func += '}'
    print func;


for i in range(1,31):
    gen_is_valid(i)
    gen_reset(i)
print('\n#endif')

