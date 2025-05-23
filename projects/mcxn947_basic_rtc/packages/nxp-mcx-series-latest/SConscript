import os
from building import *

objs = []
cwd  = GetCurrentDir()

if GetDepend(['SOC_MCXA156']):
    objs = objs + SConscript('MCXA156/SConscript')

if GetDepend(['SOC_MCXA153']):
    objs = objs + SConscript('MCXA153/SConscript')

if GetDepend(['SOC_MCXC444']):
    objs = objs + SConscript('MCXC444/SConscript')

if GetDepend(['SOC_MCXN236']):
    objs = objs + SConscript('MCXN236/SConscript')

if GetDepend(['SOC_MCXN947']):
    objs = objs + SConscript('MCXN947/SConscript')

Return('objs')
