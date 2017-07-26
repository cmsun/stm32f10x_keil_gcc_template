#coding:utf-8
#上一行的“coding:utf-8”不是一个注释内容，而是一个声明，允许以utf-8进行注释。
#它必须放在python文件的第一行，然后下文就可以用中文进行注释了。否则python
#不支持中文注释，读取含有中文的python文件时出错。

#查找编译器默认包含路径的方法：g++ -E -x c++ - -v < /dev/null
#或者 clang++ -E -x c++ - -v < /dev/null
#然后输出的内容从 “#include <...> search starts here:” 到
#“End of search list.”的内容就是编译器的默认包含路径。

import os
import ycm_core
 
flags = [ 
    '-Wno-deprecated-declarations', 
    '-Wno-disabled-macro-expansion', 
    '-Wno-c++98-compat', 
    '-Wno-c++98-compat-pedantic', 
    '-Wno-global-constructors', 
    '-Wno-exit-time-destructors', 
    '-Wno-missing-prototypes', 
    '-Wno-padded',

    '-Wunused',
    '-Wuninitialized',
    '-Wall',
    '-Wextra',
    '-Wpointer-arith',
    '-Wshadow',
    '-Wfloat-equal', 

    '-x', 'c++',
    '-std=gnu++11',

    '-isystem', r'E:\programfiles\gnu tools arm embedded\5.2 2015q4\bin\../lib/gcc/arm-none-eabi/5.2.1/../../../../arm-none-eabi/include/c++/5.2.1',
    '-isystem', r'E:\programfiles\gnu tools arm embedded\5.2 2015q4\bin\../lib/gcc/arm-none-eabi/5.2.1/../../../../arm-none-eabi/include/c++/5.2.1/arm-none-eabi',
    '-isystem', r'E:\programfiles\gnu tools arm embedded\5.2 2015q4\bin\../lib/gcc/arm-none-eabi/5.2.1/../../../../arm-none-eabi/include/c++/5.2.1/backward',
    '-isystem', r'E:\programfiles\gnu tools arm embedded\5.2 2015q4\bin\../lib/gcc/arm-none-eabi/5.2.1/include',
    '-isystem', r'E:\programfiles\gnu tools arm embedded\5.2 2015q4\bin\../lib/gcc/arm-none-eabi/5.2.1/include-fixed',
    '-isystem', r'E:\programfiles\gnu tools arm embedded\5.2 2015q4\bin\../lib/gcc/arm-none-eabi/5.2.1/../../../../arm-none-eabi/include',

    '-I', r'.',
    '-I', r'../scmf10xlib',
    '-I', r'../cmsis',
    '-I', r'../cmsis/startup',
    '-I', r'../StdPeriphDriver/v3.5/inc',
    '-I', r'../utilities',
    '-I', r'../user',

    '-D__GNUC__',
    '-DDEBUG',
    '-DHSE_VALUE=8000000',
    '-DOS_USE_TRACE_SEMIHOSTING_DEBUG',
    '-DSTM32F10X_HD',
    '-DTRACE',
    '-DUSE_FULL_ASSERT',
    '-DUSE_STDPERIPH_DRIVER',
] 
compilation_database_folder = '' 
if compilation_database_folder: 
  database = ycm_core.CompilationDatabase( compilation_database_folder ) 
else: 
  database = None 
SOURCE_EXTENSIONS = [ '.cpp', '.cxx', '.cc', '.c', '.m', '.mm' ] 
def DirectoryOfThisScript(): 
  return os.path.dirname( os.path.abspath( __file__ ) ) 
def MakeRelativePathsInFlagsAbsolute( flags, working_directory ): 
  if not working_directory: 
    return list( flags ) 
  new_flags = [] 
  make_next_absolute = False 
  path_flags = [ '-isystem', '-I', '-iquote', '--sysroot=' ] 
  for flag in flags: 
    new_flag = flag 
    if make_next_absolute: 
      make_next_absolute = False 
      if not flag.startswith( '/' ): 
        new_flag = os.path.join( working_directory, flag ) 
    for path_flag in path_flags: 
      if flag == path_flag: 
        make_next_absolute = True 
        break 
      if flag.startswith( path_flag ): 
        path = flag[ len( path_flag ): ] 
        new_flag = path_flag + os.path.join( working_directory, path ) 
        break 
    if new_flag: 
      new_flags.append( new_flag ) 
  return new_flags 
def IsHeaderFile( filename ): 
  extension = os.path.splitext( filename )[ 1 ] 
  return extension in [ '.h', '.hxx', '.hpp', '.hh' ] 
def GetCompilationInfoForFile( filename ): 
  if IsHeaderFile( filename ): 
    basename = os.path.splitext( filename )[ 0 ] 
    for extension in SOURCE_EXTENSIONS: 
      replacement_file = basename + extension 
      if os.path.exists( replacement_file ): 
        compilation_info = database.GetCompilationInfoForFile(          replacement_file )
        if compilation_info.compiler_flags_: 
          return compilation_info 
    return None 
  return database.GetCompilationInfoForFile( filename ) 
def FlagsForFile( filename, **kwargs ): 
  if database: 
    compilation_info = GetCompilationInfoForFile( filename ) 
    if not compilation_info: 
      return None 
    final_flags = MakeRelativePathsInFlagsAbsolute( 
      compilation_info.compiler_flags_, 
      compilation_info.compiler_working_dir_ ) 
  else: 
    relative_to = DirectoryOfThisScript() 
    final_flags = MakeRelativePathsInFlagsAbsolute( flags, relative_to ) 
  return { 
    'flags': final_flags, 
    'do_cache': True 
  }
