from building import * 

# get current dir path
cwd = GetCurrentDir()

# init src and inc vars
src = []
inc = [cwd]

# add Misaka_at24cxx source code
src = src + ['Misaka_at24cxx.c']

if GetDepend('PKG_USING_MISAKA_AT24CXX_DEMO'):
    src = src + ['Examples/Misaka_at24cxx_port_rtt.c']
else:
	src = src + ['Misaka_at24cxx_port.c']
	
# add group to IDE project
group = DefineGroup('Misaka_at24cxx', src, depend = ['PKG_USING_MISAKA_AT24CXX'], CPPPATH = inc)

Return('group')
