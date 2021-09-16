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
        build_common.remove_dirs(dirs=[lib_name], stderr=open(os.devnull, 'w'))
    subprocess.call(['tar', 'xfz', lib_name + '.tar.gz'])
    os.system('pushd .')
    os.system('cd ' + lib_name)

    prefix_dir = pwd_path + base.get_path('/build/android/') + abi
    if os.path.isdir(prefix_dir):
        build_common.remove_dirs(dirs=[prefix_dir], stderr=open(os.devnull, 'w'))
    build_common.mkdir_p(prefix_dir)

    output_root = tools_root + base.get_path('/build/android/') + abi
    build_common.mkdir_p(output_root + base.get_path('/log'))

    build_android_common.set_android_toolchain(name='curl', common_arch=arch, api=str(os.getenv('ANDROID_API', '')))
    build_android_common.set_android_cpu_feature(name='curl', common_arch=arch, api=str(os.getenv('ANDROID_API', '')))

    os.environ['ANDROID_NDK_HOME'] = os.getenv('ANDROID_NDK_ROOT')
    print ('ANDROID_NDK_HOME=%s' % os.environ['ANDROID_NDK_HOME'])

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

    log_file = output_root + base.get_path('/log/') + abi + '.log'

    command = ''
    command += './configure --host='
    command += build_host
    command += ' --prefix='
    command += prefix_dir
    command += ' --enable-ipv6 --with-ssl='
    command += openssl_out_dir
    command += ' --enable-static --disable-shared >'
    command += log_file
    command += ' 2>&1'
    os.system(command)

    build_common.log_info('make %s start...' % abi)

    command = ''
    command += 'make clean >>'
    command += log_file
    os.system(command)

    command = ''
    command += 'make -j '
    command += str(build_common.get_cpu_count())
    command += ' >>'
    command += log_file
    command += ' 2>&1'
    if os.system(command) == 0:
        command = 'make install >>' + log_file + ' 2>&1'
        os.system(command)

    os.system('popd')
    return True


def get_lib_name(v1, v2, v3, separator):
    return 'curl-' + str(v1) + separator + str(v2) + separator + str(v3)


def make(target_archs=None):
    if not build_android_common.check_android_env():
        print 'no android env'
        return False
    if not build_common.set_pkg_config_path():
        print 'unable to set PKG_CONFIG_PATH'
        return False
    build_android_common.set_env()

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
    build_common.remove_dirs(dirs=[lib_dest_dir, lib_name], stderr=open(os.devnull, 'w'))
    if not os.path.isfile(base.get_path('./') + lib_name + '.tar.gz'):
        # todo check what for >lib_name.tar.gz in original script
        os.system('curl -LO ' + download_url)

    build_android_common.set_android_toolchain_bin()

    build_common.log_info('%s %s start...' % (os.environ['PLATFORM_TYPE'], lib_name))

    for i in range(0, len(build_android_common.ARCHS)):
        # if target_archs and build_android_common.ARCHS[i] in target_archs:
        if True:
            configure_make(
                build_android_common.ARCHS[i]
                , build_android_common.ABIS[i]
                , build_android_common.ABI_TRIPLES[i]
                , lib_name=lib_name
                , pwd_path=pwd_path
                , tools_root=tools_root
            )

    build_common.log_info('%s %s end...' % (os.environ['PLATFORM_TYPE'], lib_name))
    return True


make()
