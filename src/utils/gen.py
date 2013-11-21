
def gen_is_valid (npar):
    func =  '/// Check if ' + str(npar) + ' data points are valid\n'
    func +=  'template <'
    for i in range(0, npar):
        func += 'class T' + str(i) + ', '
    func = func.rstrip(', ')
    func += '>\n'

    func += 'inline bool is_valid ('
    for i in range(0, npar):
        func += 'const T' + str(i) + '& t' + str(i) + ', '
    func = func.rstrip(', ')
    func += ')\n{\n  ///\n  return ( '

    for i in range(0, npar):
        func += 't' + str(i) + ' != dragon::NoData<T' + str(i) + '>::value() && '
    func = func.rstrip(' && ')
    func += ' );\n}\n'
    print func;

def gen_reset (npar):
    func =  '/// Reset ' + str(npar) + ' data points\n'
    func +=  'template <'
    for i in range(0, npar):
        func += 'class T' + str(i) + ', '
    func = func.rstrip(', ')
    func += '>\n'

    func += 'inline void reset_data ('
    for i in range(0, npar):
        func += 'T' + str(i) + '& t' + str(i) + ', '
    func = func.rstrip(', ')
    func += ')\n{\n  ///\n'

    for i in range(0, npar):
        func += '  t' + str(i) + ' = dragon::NoData<T' + str(i) + '>::value();\n'
    func += '}\n'
    print func;


for i in range(1,31):
    gen_is_valid(i)
    gen_reset(i)
print('\n#endif')

