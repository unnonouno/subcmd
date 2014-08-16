VERSION = '0.1.0'
APPNAME = 'subcmd'

def options(opt):
    opt.load('compiler_cxx')
    opt.load('unittest_gtest')

def configure(conf):
    conf.env.CXXFLAGS += ['-O2', '-Wall', '-g', '-pipe']
    conf.load('compiler_cxx')
    conf.load('unittest_gtest')

def build(bld):
    bld.program(
        source='sample.cpp',
        target='sample',
        use=''
    )

def cpplint(ctx):
    cpplint_args = '--filter=-runtime/references,-legal/copyright,-build/include_order --extensions=cpp,hpp'

    src_dir = ctx.path.find_node('.')
    files = []
    for f in src_dir.ant_glob('*.cpp *.hpp'):
        files.append(f.path_from(ctx.path))

    args = 'cpplint.py %s %s' % (cpplint_args,' '.join(files))
    result = ctx.exec_command(args)
    if result != 0:
        ctx.fatal('cpplint failed')

def gcovr(ctx):
    excludes = [
        '.*\.unittest-gtest.*',
        '.*_test\.cpp',
      ]

    args = 'gcovr --branches -r . '
    for e in excludes:
        args += ' -e "%s"' % e

    ctx.exec_command(args)
