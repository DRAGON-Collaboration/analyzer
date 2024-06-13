import pydragon
import pandas as pd
import numpy as np
from sys import argv


BUFSIZE = 0xffff

ihead = 0
itot = 0
d1 = {
  "esort0": np.empty(BUFSIZE)
}

outfile = 'test.h5'

if pydragon.setup(argv) == 0:
    while True:
        evt = pydragon.read_event()
        if pydragon.READ_EOF in evt[0]:
            break

        if pydragon.DRAGON_HEAD_EVENT in evt[0]:
            i = evt[0].index(pydragon.DRAGON_HEAD_EVENT)
            head = evt[1][i]
            d1['esort0'][ihead] = head.bgo.esort[0]
            ihead += 1
            itot += 1

            if ihead == BUFSIZE:
                pd.DataFrame(d1).to_hdf(
                    outfile,
                    key = 'df1',
                    append = True,
                    format = 'table'
                )
                ihead = 0
            if itot % 1000 == 0:
                print(itot, ' (', ihead, ')', sep='')
            
                   
    
pydragon.close_file()




