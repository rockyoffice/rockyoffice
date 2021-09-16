#!/usr/bin/env python

import os  # getcwd, chdir, path.isfile
import subprocess  # call
import build_common  #
import build_android_common  #
import sys  # path.append
sys.path.append('./../../../../build_tools/scripts')
import base  # get_path, get_script_dir, host_platform


def configure_make(arch, abi, abi_triple, lib_name, pwd_path, tools_root):
    build_common.log_info('configure %s start...' % abi)
    if os.path.isdir(base.get_path('./') + lib_name):
        subprocess.call(['rm', '-rf', lib_name])
    subprocess.call(['tar', 'xfz', lib_name + '.tar.gz'])
    os.system('pushd .')
    os.system('cd ' + lib_name)

    prefix_dir = pwd_path + base.get_path('/build/android/') + abi
    if os.path.isdir(prefix_dir):
        subprocess.call(['rm', '-rf', prefix_dir])
    os.system('mkdir -p ' + prefix_dir)

    output_root = tools_root + base.get_path('/build/android/') + abi
    os.system('mkdir -p ' + output_root + base.get_path('/log'))

    build_android_common.set_android_toolchain(name='curl', common_arch=arch, api=str(os.getenv('ANDROID_API', '')))
    build_android_common.set_android_cpu_feature(name='curl', common_arch=arch, api=str(os.getenv('ANDROID_API', '')))

    os.environ['ANDROID_NDK_HOME'] = os.getenv('ANDROID_NDK_ROOT')
    print ('ANDROID_NDK_HOME=%s' % os.environ[ANDROID_NDK_HOME])

    openssl_out_dir = pwd_path + base.get_path('/../openssl/build/android/') + abi

    old_ldflags = os.getenv('LDFLAGS')
    os.environ['LDFLAGS'] = old_ldflags + ' -L' + openssl_out_dir + base.get_path('/lib')

    build_android_common.android_printf_global_params(arch, abi, abi_triple, prefix_dir, output_root)

    if 'x86_64' != arch and 'x86' != arch and 'arm' != arch and 'arm64' != arch:
        build_common.log_error('not support')
        return False

    build_host = build_android_common.android_get_build_host(arch)
    if not build_host:
        build_common.log_error('build_host is None')
        return False

    command = ''
    command += './configure --host='
    command += build_host
    command += ' --prefix='
    command += prefix_dir
    command += ' --enable-ipv6 --with-ssl='
    command += openssl_out_dir
    command += ' --enable-static --disable-shared >'
    command += output_root
    command += base.get_path('/log/')
    command += abi
    command += '.log 2>&1'
    os.system(command)

    build_common.log_info('make %s start...' % abi)

    # todo make
    return


def get_lib_name(v1, v2, v3, separator):
    return 'curl-' + str(v1) + separator + str(v2) + separator + str(v3)


def make():


    # todo init
    tools_root = base.get_path(os.getcwd())
    pwd_path = base.get_path(base.get_script_dir(__file__))
    os.chdir(pwd_path)
    print('pwd_path = ' + pwd_path)
    print('tools_root = ' + tools_root)

    v1 = 7
    v2 = 68
    v3 = 0
    lib_version = get_lib_name(v1, v2, v3, '_')
    lib_name = get_lib_name(v1, v2, v3, '.')
    lib_dest_dir = pwd_path + base.get_path('/build/android/curl-universal')

    download_url = 'https://github.com/curl/curl/releases/download/' + lib_version + '/' + lib_name + '.tar.gz'
    print(download_url)

    # 'https://curl.haxx.se/download/' + lib_name + '.tar.gz'
    # https://github.com/curl/curl/releases/download/curl-7_69_0/curl-7.69.0.tar.gz
    # https://github.com/curl/curl/releases/download/curl-7_68_0/curl-7.68.0.tar.gz
    command = ['rd', '/s', '/q'] if 'windows' == base.host_platform() else ['rm', '-rf']
    command.append(lib_dest_dir)
    # command.append(lib_name)  # removed in configure_make
    subprocess.call(command, stderr=open(os.devnull, 'w'))
    if not os.path.isfile(base.get_path('./') + lib_name + '.tar.gz'):
        # todo check what for >lib_name.tar.gz in original script
        os.system('curl -LO ' + download_url)
    build_android_common.set_android_toolchain_bin()
    # todo
