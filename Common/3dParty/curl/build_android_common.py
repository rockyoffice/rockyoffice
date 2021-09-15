#!/usr/bin/env python

import os  # getenv
import platform  # machine
import sys  # path.append
sys.path.append('./../../../../build_tools/scripts')
import base  # host_platform
import build_common  # get_command_output


def set_env():
    os.environ['PLATFORM_TYPE'] = 'Android'
    os.environ['ANDROID_API'] = '21'
    global ARCHS
    global ABIS
    global ABI_TRIPLES
    ARCHS = ["arm", "arm64", "x86", "x86_64"]
    ABIS = ["armeabi-v7a", "arm64-v8a", "x86", "x86_64"]
    ABI_TRIPLES = ["arm-linux-androideabi", "aarch64-linux-android", "i686-linux-android", "x86_64-linux-android"]


def check_android_env():
    return os.getenv('ANDROID_NDK_ROOT') is not None and os.getenv('ANDROID_HOME') is not None


# easier than bringing regexp
def is_i_86(text):
    return len(text) == 4 and text[0] == 'i' and text[2] == '8' and text[3] == '6'


def get_host_arch():
    if 'windows' == base.host_platform():
        host_arch = platform.machine().lower()
    else:
        host_arch = build_common.get_command_output(['uname', '-m'])
    if not host_arch:
        return None

    if is_i_86(host_arch):
        return 'x86'
    if host_arch == 'amd64' or host_arch == 'x86_64':
        return 'x86_64'
    return None


def is_cygwin_os(text):
    return text.find('CYGWIN') == 0 or text.find('_NT-') != -1


def get_host_os():
    if 'windows' == base.host_platform():
        return 'windows'

    result = build_common.get_command_output(['uname', '-s'])
    if not result:
        return None

    if is_cygwin_os(result):
        return 'cygwin'

    # just return others
    if len(result) != 0:
        return result.lower()
    else:
        return None


def get_toolchain():
    host_os = get_host_os()
    if not host_os:
        return None
    host_arch = get_host_arch()
    if not host_arch:
        return None
    return host_os + '-' + host_arch


def get_android_arch(common_arch):
    if 'arm' == common_arch:
        return 'arm-v7a'
    elif 'arm64' == common_arch:
        return 'arm64-v8a'
    elif 'x86_64' == common_arch:
        return 'x86-64'
    else:
        return common_arch


def get_target_build(arch):
    if 'arm-v7a' == arch:
        return 'arm'
    elif 'arm64-v8a' == arch:
        return 'arm64'
    elif 'x86-64' == arch:
        return 'x86_64'
    else:
        return arch


def get_build_host_internal(arch):
    if 0 == arch.find('arm-v7a'):
        return 'arm-linux-androideabi'
    elif 'arm64-v8a' == arch:
        return 'aarch64-linux-android'
    elif 'x86' == arch:
        return 'i686-linux-android'
    elif 'x86-64' == arch:
        return 'x86_64-linux-android'
    else:
        return None


def android_get_build_host(common_arch):
    return get_build_host_internal(get_android_arch(common_arch))


def get_clang_target_host(arch, api):
    if 0 == arch.find('arm-v7a'):
        return 'armv7a-linux-androideabi' + api
    elif 'arm64-v8a' == arch:
        return 'aarch64-linux-android' + api
    elif 'x86' == arch:
        return 'i686-linux-android' + api
    elif 'x86-64' == arch:
        return 'x86_64-linux-android' + api
    else:
        return None


def set_android_toolchain_bin():
    old_path = os.getenv('PATH')
    android_ndk_root = os.getenv('ANDROID_NDK_ROOT')
    if not android_ndk_root:
        return
    separator = ';' if 'windows' == base.host_platform() else ':'
    toolchain = get_toolchain()
    if not toolchain:
        return
    # PATH=${ANDROID_NDK_ROOT}/toolchains/llvm/prebuilt/$(get_toolchain)/bin:$PATH
    new_path = ''
    new_path += android_ndk_root
    new_path += base.get_path('/toolchains/llvm/prebuilt/')
    new_path += toolchain
    new_path += base.get_path('/bin')
    new_path += separator
    new_path += old_path
    os.environ['PATH'] = new_path
    return


def set_android_toolchain(name, common_arch, api):
    arch = get_android_arch(common_arch)
    build_host = get_build_host_internal(arch)
    clang_target_host = get_clang_target_host(arch, api)

    os.environ['AR'] = build_host + '-ar'
    os.environ['CC'] = clang_target_host + '-clang'
    os.environ['CXX'] = clang_target_host + '-clang++'
    os.environ['AS'] = build_host + '-as'
    os.environ['LD'] = build_host + '-ld'
    os.environ['RANLIB'] = build_host + '-ranlib'
    os.environ['STRIP'] = build_host + '-strip'
    return


def get_common_includes():
    toolchain = get_toolchain()
    if not toolchain:
        return None
    android_ndk_root = os.getenv('ANDROID_NDK_ROOT')
    if not android_ndk_root:
        return None

    common_path = ''
    common_path += '-I'
    common_path += android_ndk_root
    common_path += base.get_path('/toolchains/llvm/prebuilt/')
    common_path += toolchain
    common_path += base.get_path('/sysroot/usr/')

    result = ''
    result += common_path + 'include' + ' '
    result += common_path + base.get_path('local/include')
    return result


def get_common_linked_libraries(api, arch):
    android_ndk_root = os.getenv('ANDROID_NDK_ROOT')
    if not android_ndk_root:
        return None
    toolchain = get_toolchain()
    if not toolchain:
        return None
    build_host = get_build_host_internal(arch)
    if not build_host:
        return None

    # -L${ANDROID_NDK_ROOT}/toolchains/llvm/prebuilt/${toolchain}/
    common_path = ''
    common_path += '-L'
    common_path += android_ndk_root
    common_path += base.get_path('/toolchains/llvm/prebuilt/')
    common_path += toolchain
    common_path += base.get_path('/')

    result = ''
    result += common_path + build_host + base.get_path('/lib') + ' '
    result += common_path + base.get_path('sysroot/usr/lib/') + build_host + base.get_path('/') + api + ' '
    result += common_path + base.get_path('/lib')
    return result


def set_android_cpu_feature(name, common_arch, api):
    arch = get_android_arch(common_arch)
    common_linked_libraries = get_common_linked_libraries(api, arch)
    if not common_linked_libraries:
        return
    common_includes = get_common_includes()
    if not common_includes:
        return

    cflags_common = ' -Wno-unused-function -fno-integrated-as -fstrict-aliasing -fPIC -DANDROID -D__ANDROID_API__=' + api + ' -Os -ffunction-sections -fdata-sections ' + common_includes
    ldflags_common = ' -Wl,--gc-sections -Os -ffunction-sections -fdata-sections ' + common_linked_libraries
    if 0 == arch.find('arm-v7a'):
        common_part = '-march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=softfp'
        os.environ['CFLAGS'] = common_part + cflags_common
        os.environ['LDFLAGS'] = common_part + ' -Wl,--fix-cortex-a8' + ldflags_common
    elif 'arm64-v8a' == arch:
        common_part = '-march=armv8-a'
        os.environ['CFLAGS'] = common_part + cflags_common
        os.environ['LDFLAGS'] = common_part + ldflags_common
    elif 'x86' == arch:
        common_part = '-march=i686'
        os.environ['CFLAGS'] = common_part + ' -mtune=intel -mssse3 -mfpmath=sse -m32' + cflags_common
        os.environ['LDFLAGS'] = common_part + ldflags_common
    elif 'x86-64' == arch:
        common_part = '-march=x86-64'
        os.environ['CFLAGS'] = common_part + ' -msse4.2 -mpopcnt -m64 -mtune=intel' + cflags_common
        os.environ['LDFLAGS'] = common_part + ldflags_common
    else:
        return

    os.environ['CXXFLAGS'] = '-std=c++11 -Os -ffunction-sections -fdata-sections'
    os.environ['CPPFLAGS'] = os.getenv('CFLAGS')
    return


def android_printf_global_params(arch, abi, abi_triple, in_dir, out_dir):
    print ('arch =          ' + arch)
    print ('abi =           ' + abi)
    print ('abi_triple =    ' + abi_triple)
    print ('PLATFORM_TYPE = ' + os.environ['PLATFORM_TYPE'])
    print ('ANDROID_API =   ' + os.environ['ANDROID_API'])
    print ('in_dir =        ' + in_dir)
    print ('out_dir =       ' + out_dir)
    print ('out_dir =       ' + out_dir)
    print ('AR =            ' + os.environ['AR'])
    print ('CC =            ' + os.environ['CC'])
    print ('CXX =           ' + os.environ['CXX'])
    print ('AS =            ' + os.environ['AS'])
    print ('LD =            ' + os.environ['LD'])
    print ('RANLIB =        ' + os.environ['RANLIB'])
    print ('STRIP =         ' + os.environ['STRIP'])
    print ('CFLAGS =        ' + os.environ['CFLAGS'])
    print ('CXXFLAGS =      ' + os.environ['CXXFLAGS'])
    print ('LDFLAGS =       ' + os.environ['LDFLAGS'])
    print ('CPPFLAGS =      ' + os.environ['CPPFLAGS'])
    return
