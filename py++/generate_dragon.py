#! /usr/bin/python

import os
import sys
import glob
from pyplusplus import module_builder

DRAGON_HOME = os.path.abspath("..")
SRC = DRAGON_HOME + '/src'
DRAGON_HEADERS =  glob.glob(SRC + "/dragon/*.hxx")
DRAGON_HEADERS.append(SRC + '/utils/VariableStructs.hxx')

VME_HEADERS    =  glob.glob(SRC + "/vme/*.hxx")

MIDAS_HEADERS  =  glob.glob(SRC + "/midas/*.hxx")
MIDAS_HEADERS.remove(SRC + '/midas/Xml.hxx')
MIDAS_HEADERS.remove(SRC + '/midas/Odb.hxx')
MIDAS_HEADERS.remove(SRC + '/midas/Database.hxx')

def Pwd():
    return os.path.join(os.path.abspath('.'))

def create_module_builder(headers, Name, Includes = []):
    incTot = Includes + [SRC]
    mb = module_builder.module_builder_t(
        files=headers,
        include_paths=incTot,
        define_symbols=['PRIVATE=public', 'MIDASSYS', 'OS_LINUX', 'OS_DARWIN']
        )

    mb.add_declaration_code('#include \"Headers.hpp\"')
    mb.add_declaration_code('#include \"Constants.hpp\"')
    mb.build_code_creator(module_name=Name)
    mb.code_creator.user_defined_directories.append( os.path.abspath('.'))

    return mb

def write_module(mb, Name):
    mb.write_module(Pwd() + '/' +  Name)
                           


mbDragon = create_module_builder(DRAGON_HEADERS, 'dragon')
mbVme    = create_module_builder(VME_HEADERS, 'vme')
write_module(mbDragon, 'Dragon.cpp')
write_module(mbVme, 'Vme.cpp')

#mbMidas  = create_module_builder(MIDAS_HEADERS, 'midas', [os.path.abspath('/Users/gchristian/packages/midas/include')])


# OdbClass = mbMidas.classes( lambda decl: decl.name.startswith( 'Odb' ) )
# XmlClass = mbMidas.classes( lambda decl: decl.name.startswith( 'Xml' ) )
# OdbClass.exclude()
# XmlClass.exclude()
#tclass = mbMidas.classes(lambda decl: decl.name.startswith('TMidas'))
#tbank = mbMidas.classes(lambda decl: decl.name.startswith('Bank'))
#tclass.exclude()
#tbank.exclude()

#tme = mbMidas.class_('TMidasEvent')
#tme.exclude()

#write_module(mbMidas, 'Midas.cpp')
