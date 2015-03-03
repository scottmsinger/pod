import os
import tip.build.tipscons
from tip.build.tipscons import versionTag

CPP_FILES = [ 'TipPod.cpp',
              'TipPodNode.cpp',
              'TipPodValue.cpp',
              'TipPodBlockPodValue.cpp',
              'TipPodUtils.cpp',
              'lexer.cpp',
              'parser.cpp'
            ] + versionTag("TipPod")

Import( 'env' )

env_local = env.Clone()

libTipPod = env_local.StaticLibrary('TipPod', 
                                    CPP_FILES, 
                                    LIBPREFIX = 'lib' )

exportDir = tip.build.tipscons.getExportDir( "lib" )
TipPod = Install( exportDir, libTipPod )
Export( 'TipPod' )
Default( TipPod )
Return( 'TipPod' )

