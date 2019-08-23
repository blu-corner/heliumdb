from skbuild import setup

setup(name = 'heliumdb',
      version = '1.0',
      cmake_args = ['-DCMAKE_EXPORT_COMPILE_COMMANDS=on', '-DTESTS=on'],
      packages = [''],
      package_dir = {'': 'src/heliumdb'}
)
